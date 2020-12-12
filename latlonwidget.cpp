#include "latlonwidget.h"

#include <QLineEdit>
#include <QLabel>
#include <QLayout>

#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include <QApplication>
#include <QStyle>

#include "utm.h"

#include <QDebug>

///
/// \brief Structure to hold a floating point value as a whole and fraction part separately.
///
struct FloatType
{
    FloatType(double value) { setValue(value); }
    FloatType(int32_t whole, int32_t fraction) { setValue(whole, fraction); }

    void setValue(double value) {
        // use 6 digit precision
        QStringList f = QString::number(value, 'f', 6).split('.');
        // break into whole and fraction part
        m_whole    = f[0].toInt();
        m_fraction = f[1].toInt();
    }

    void setValue(int32_t whole, int32_t fraction) {
        QStringList lst = QString::number(fraction/1000000.0, 'f', 6).split('.');
        bool found {};

        // look for x33333, x66666, x99999 pattern
        QStringList patterns {"\\d3{5}", "\\d6{5}", "\\d9{5}"};
        QRegularExpression re;
        QRegularExpressionMatch match;
        foreach(auto s, patterns) {
            re.setPattern(s);
            match = re.match(lst[1]);
            if(match.hasMatch()) {
                found = true;
                break;
            }
        }

        if( found ) {
            fraction += 1;
        }

        m_whole = whole;
        m_fraction = fraction;
    }

    double getValue() const {
        if( m_whole < 0 ) {
            return (double(m_whole) - (m_fraction/1000000.0));
        } else {
            return (double(m_whole) + (m_fraction/1000000.0));
        }
    }

    void getValue(int32_t &whole, int32_t &fraction) {
        whole = m_whole;
        fraction = m_fraction;
    }

private:
    int32_t m_whole;
    int32_t m_fraction;
};

///
/// \brief Custom LineEdit class
///
class MyLineEdit : public QLineEdit
{
public:
    QSize sizeHint() const;
};

QSize MyLineEdit::sizeHint() const
{
    return QSize(110, 20);
}

///
/// \brief LatLonWidget::LatLonWidget
/// Main Class for creating LatLonWidget.
///
/// \param parent
///
LatLonWidget::LatLonWidget(QWidget *parent) :
    QWidget(parent)
{
    m_latValidator = new QRegularExpressionValidator();
    m_lonValidator = new QRegularExpressionValidator();

    m_label1 = new QLabel;
    m_label2 = new QLabel;

    m_latLineEdit = new MyLineEdit();
    m_latLineEdit->setObjectName("Latitude");
    m_latLineEdit->setStyleSheet(lineEditStyle + validStyle);

    m_lonLineEdit = new MyLineEdit();
    m_lonLineEdit->setObjectName("Longitude");
    m_lonLineEdit->setStyleSheet(lineEditStyle + validStyle);

    m_latitude = new FloatType(0, 0);
    m_longitude = new FloatType(0, 0);

    // Depending on the position format setup the display
    if( m_posFormat == PositionFormatType::eDECIMAL_DEG) {
        setupDegDisplay();
    } else if( m_posFormat == PositionFormatType::eDMS) {
        setupDMSDisplay();
    } else {
        // UTM format
        setupUTMDisplay();
    }

    if( m_posFormat == PositionFormatType::eUTM) {
        setLabels("NRT: ", "EST: ");
    } else {
        setLabels("LAT: ", "LON: ");
    }

    // Arrange widgets
    m_layout = new QGridLayout();
    m_layout->addWidget(m_label1,      0, 0);
    m_layout->addWidget(m_latLineEdit, 0, 1);
    m_layout->addWidget(m_label2,      1, 0);
    m_layout->addWidget(m_lonLineEdit, 1, 1);
    m_layout->setMargin(0);
    m_layout->setSpacing(2);
    this->setLayout(m_layout);

    connect(m_latLineEdit, &QLineEdit::textChanged, this, &LatLonWidget::textChanged);
    connect(m_lonLineEdit, &QLineEdit::textChanged, this, &LatLonWidget::textChanged);
}

void LatLonWidget::setLabels(const QString &label1, const QString &label2)
{
    m_label1->setText(label1);
    m_label2->setText(label2);
}

QString LatLonWidget::format(PositionFormatType posFormat, LatLonWidget::ValueType type, FloatType *value)
{
    QString result = "";
    int32_t whole, fraction;
    int32_t prec {7};
    value->getValue(whole, fraction);

    if (posFormat == PositionFormatType::eDECIMAL_DEG) {
        int32_t degWidth = 3;
        prec = 6;
        QString tmp;

        if( type == eLATITUDE) {
            degWidth = 2;

            if( m_decimalDegNotation == NotationType::eSIGN ) {
                tmp = "+";
            } else {
                tmp = "N ";
            }

            if( whole < 0 ) {
                if( m_decimalDegNotation == NotationType::eSIGN ) {
                    tmp = "-";
                } else {
                    tmp = "S ";
                }
            }
        } else {
            if( m_decimalDegNotation == NotationType::eSIGN ) {
                tmp = "+";
            } else {
                tmp = "E ";
            }

            if( whole < 0 ) {
                if( m_decimalDegNotation == NotationType::eSIGN ) {
                    tmp = "-";
                } else {
                    tmp = "W ";
                }
            }
        }

        QStringList f = QString::number(value->getValue(), 'f', prec).split('.');
        result = QString("%1%2.%3\u00B0").arg(tmp).
                arg(abs(f[0].toInt()), degWidth, 10, QChar('0')).
                arg(f[1].toInt(), 6, 10, QChar('0'));

    } else if (posFormat == PositionFormatType::eDMS) {
        int32_t degFieldWidth = 3;
        QString direction = "N";

        if( type == eLATITUDE) {
            degFieldWidth = 2;
            if( whole < 0 )
                direction = "S";
        } else {
            degFieldWidth = 3;
            if( whole < 0 ) {
                direction = "W";
            } else {
                direction = "E";
            }
        }

        int32_t deg = whole;
        int32_t min = (fraction * 6) / 100000;
        double sec = ((fraction / 1000000.0) - (min/60.0)) * 3600;

        result = QString("%1 %2\u00B0 %3' %4\"").arg(direction).
                arg(abs(deg), degFieldWidth, 10, QChar('0')).
                arg(min, 2, 10, QChar('0')).
                arg(sec, 5, 'f', 2, QChar('0'));
    }
    return result;
}

void LatLonWidget::formatUTM()
{
    double latitude = m_latitude->getValue();
    double longitude = m_longitude->getValue();

    double northing, easting;
    char zone[5];
    UTM::LLtoUTM(latitude, longitude, northing, easting, &zone[0]);
    QString z(zone);
    m_latLineEdit->setText(QString("%1 %2 m").arg(z).arg(northing, 6, 'f', 0, QChar('0')));
    m_lonLineEdit->setText(QString("%1 m").arg(easting, 6, 'f', 0, QChar('0')));
}

void LatLonWidget::setNotation(NotationType notation)
{
    m_decimalDegNotation = notation;
    setupDegDisplay();
}

void LatLonWidget::setupDegDisplay()
{
    if( m_decimalDegNotation == NotationType::eSIGN ) {
        // Latitude (degree format)
        m_latLineEdit->setInputMask(latDegInputMask);
        m_latValidator->setRegularExpression(QRegularExpression(latDegRegExp));
        m_latLineEdit->setValidator(m_latValidator);
        m_latLineEdit->setText(format(eDECIMAL_DEG, eLATITUDE, m_latitude));

        // Longitude (degree format)
        m_lonLineEdit->setInputMask(lonDegInputMask);
        m_lonValidator->setRegularExpression(QRegularExpression(lonDegRegExp));
        m_lonLineEdit->setValidator(m_lonValidator);
        m_lonLineEdit->setText(format(eDECIMAL_DEG, eLONGITUDE, m_longitude));

    } else {
        m_latLineEdit->setInputMask(latDecimalDegMask);
        m_latValidator->setRegularExpression(QRegularExpression(latDecimalDegRegExp));
        m_latLineEdit->setValidator(m_latValidator);
        m_latLineEdit->setText(format(eDECIMAL_DEG, eLATITUDE, m_latitude));

        m_lonLineEdit->setInputMask(lonDecimalDegMask);
        m_lonValidator->setRegularExpression(QRegularExpression(lonDecimalDegRegExp));
        m_lonLineEdit->setValidator(m_lonValidator);
        m_lonLineEdit->setText(format(eDECIMAL_DEG, eLONGITUDE, m_longitude));
    }
}

void LatLonWidget::setupDMSDisplay()
{
    // Latitude (DMS format)
    m_latLineEdit->setInputMask(latDMSInputMask);
    m_latValidator->setRegularExpression(QRegularExpression(latDMSRegExp));
    m_latLineEdit->setValidator(m_latValidator);
    m_latLineEdit->setText(format(eDMS, eLATITUDE, m_latitude));

    // Longitude (DMS format)
    m_lonLineEdit->setInputMask(lonDMSInputMask);
    m_lonValidator->setRegularExpression(QRegularExpression(lonDMSRegExp));
    m_lonLineEdit->setValidator(m_lonValidator);
    m_lonLineEdit->setText(format(eDMS, eLONGITUDE, m_longitude));
}

void LatLonWidget::setupUTMDisplay()
{
    // Northing (UTM)
    m_latLineEdit->setInputMask(latUTMInputMask);
    m_latValidator->setRegularExpression(QRegularExpression(latUTMRegExp));
    m_latLineEdit->setValidator(m_latValidator);

    m_lonLineEdit->setInputMask(lonUTMInputMask);
    m_lonValidator->setRegularExpression(QRegularExpression(lonUTMRegExp));
    m_lonLineEdit->setValidator(m_lonValidator);

    formatUTM();
}


void LatLonWidget::textChanged()
{
    QLineEdit *lineEdit = reinterpret_cast<QLineEdit *>(sender());

    if( !lineEdit->hasFocus())
        return;

    // get current displayed text
    QString tmp = lineEdit->displayText();

    if( m_posFormat == PositionFormatType::eDECIMAL_DEG) {
        tmp.chop(1); // get rid of degree sign
        QStringList f = tmp.split('.');

        int32_t frac = f[1].toInt();
        int32_t whole;
        if( m_decimalDegNotation == NotationType::eSIGN ) {
            whole = f[0].toInt();
        } else {
            QStringList p = f[0].split(' ');
            whole = p[1].toInt();
            if( p[0] == "S" || p[0] == "s" || p[0] == "W" || p[0] == "w" )
                whole *= -1;
        }
        if( lineEdit->objectName() == "Latitude" ) {
            validateAndUpdatePosition(eLATITUDE, whole, frac, m_latitude, lineEdit);
        }
        else {
            validateAndUpdatePosition(eLONGITUDE, whole, frac, m_longitude, lineEdit);
        }
    } else if(m_posFormat == PositionFormatType::eDMS) {
        QStringList f = tmp.split(" ");
        f[1].chop(1);
        f[2].chop(1);
        f[3].chop(1);

        int32_t whole = f[1].toInt();
        // convert minutes and seconds
        int32_t fraction = static_cast<int32_t>(((f[2].toDouble() / 60.0) + (f[3].toDouble() / 3600.0)) * 1000000);

        if( lineEdit->objectName() == "Latitude" ) {
            if(f[0] == 'S' || f[0] == 's')
                whole *= -1;
            validateAndUpdatePosition(eLATITUDE, whole, fraction, m_latitude, lineEdit);
        } else {
            if(f[0] == 'W' || f[0] == 'w')
                whole *= -1;
            validateAndUpdatePosition(eLONGITUDE, whole, fraction, m_longitude, lineEdit);
        }
    } else {

        // UTM
        double northing, easting;
        double latitude, longitude;
        QByteArray zone;

        // Northing
        if( lineEdit->objectName() == "Latitude" ) {
            // get zone and northing
            QStringList f = tmp.split(' ');
            northing = f[1].toDouble();
            zone  = QByteArray::fromRawData(f[0].toUtf8(), f[0].length());

            QString s = m_lonLineEdit->displayText();
            QStringList f2 = s.split(' ');
            easting = f2[0].toDouble();
        } else { // Easting
            QStringList f = tmp.split(' ');
            easting = f[0].toDouble();

            QString s = m_latLineEdit->displayText();
            QStringList f2 = s.split(' ');
            northing = f2[1].toDouble();
            zone = QByteArray::fromRawData(f2[0].toUtf8(), f2[0].length());
        }
        // TODO: Validate northing and easting
        UTM::UTMtoLL(northing, easting, zone.constData(), latitude, longitude);
        m_latitude->setValue(latitude);
        m_longitude->setValue(longitude);
    }

    if( lineEdit->objectName() == "Latitude" ) {
        emit latitudeChanged(m_latitude->getValue());
    } else {
        emit longitudeChanged(m_longitude->getValue());
    }
}

void LatLonWidget::setPosition(const double &latitude, const double &longitude)
{
    m_latitude->setValue(latitude);
    m_longitude->setValue(longitude);

    if( m_posFormat == PositionFormatType::eDECIMAL_DEG) {
        m_latLineEdit->setText(format(eDECIMAL_DEG, eLATITUDE, m_latitude));
        m_lonLineEdit->setText(format(eDECIMAL_DEG, eLONGITUDE, m_longitude));
    }
    else if( m_posFormat == PositionFormatType::eDMS) {
        m_latLineEdit->setText(format(eDMS, eLATITUDE, m_latitude));
        m_lonLineEdit->setText(format(eDMS, eLONGITUDE, m_longitude));
    } else {
        formatUTM();
    }
}

void LatLonWidget::getPosition(double &latitude, double &longitude)
{
    latitude = m_latitude->getValue();
    longitude = m_longitude->getValue();
}

void LatLonWidget::setReadOnly(bool flag)
{
    m_isReadOnly = flag;
    m_latLineEdit->setReadOnly(m_isReadOnly);
    m_lonLineEdit->setReadOnly(m_isReadOnly);

    if(!m_isReadOnly) {
        if( m_isLatValid ) {
            m_latLineEdit->setProperty("Valid", true);
        } else {
            m_latLineEdit->setProperty("Valid", false);
        }

        if( m_isLonValid ) {
            m_lonLineEdit->setProperty("Valid", true);
        } else {
            m_lonLineEdit->setProperty("Valid", false);
        }
    }
}


bool LatLonWidget::isLatitudeValid(int32_t whole, int32_t frac)
{
    bool valid {true};
    if((whole > 90) || (whole < -90) || ( (whole == 90 || whole == -90) && (frac > 0)))
        valid = false;
    return valid;
}

bool LatLonWidget::isLongitudeValid(int32_t whole, int32_t frac)
{
    bool valid {true};
    if((whole > 180) || (whole < -180) || ( (whole == 180 || whole == -180) && (frac > 0)))
        valid = false;
    return valid;
}

void LatLonWidget::validateAndUpdatePosition(ValueType type, int32_t whole,
                                             int32_t frac, FloatType *value, QLineEdit *edit)
{
    bool isValid = true;
    if( type == eLATITUDE ) {
        isValid = isLatitudeValid(whole, frac);
    } else {
        isValid = isLongitudeValid(whole, frac);
    }

    value->setValue(whole, frac);
    if( isValid ) {
        edit->setStyleSheet(lineEditStyle + validStyle);

    } else {
        edit->setStyleSheet(lineEditStyle + invalidStyle);
    }
}

void LatLonWidget::setPositionFormat(int format)
{
    m_posFormat = static_cast<PositionFormatType>(format);

    if( m_posFormat == PositionFormatType::eDECIMAL_DEG) {
        setupDegDisplay();
    } else if( m_posFormat == PositionFormatType::eDMS) {
        setupDMSDisplay();
    } else {
        setupUTMDisplay();
    }

    if( m_posFormat == PositionFormatType::eUTM) {
        setLabels("NRT: ", "EST: ");
    } else {
        setLabels("LAT: ", "LON: ");
    }
}
