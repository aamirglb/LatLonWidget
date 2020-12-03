#include "latlonwidget.h"

#include <QLineEdit>
#include <QLabel>
#include <QLayout>
#include <QString>

#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "position2d.h"
#include "utm.h"

#include <QDebug>

LatLonWidget::LatLonWidget(PositionFormatType format, QWidget *parent) : QWidget(parent)
{
    m_posFormat = format;

    m_latDegValidator = new QRegularExpressionValidator();
    m_latDmsValidator = new QRegularExpressionValidator();
    m_lonDegValidator = new QRegularExpressionValidator();
    m_lonDmsValidator = new QRegularExpressionValidator();    

    m_latValidator = new QRegularExpressionValidator();
    m_lonValidator = new QRegularExpressionValidator();

    m_latLineEdit = new QLineEdit();
    m_latLineEdit->setObjectName("Latitude");
    m_latLineEdit->setStyleSheet(lineEditStyle + validStyle);

    m_lonLineEdit = new QLineEdit();
    m_lonLineEdit->setObjectName("Longitude");
    m_lonLineEdit->setStyleSheet(lineEditStyle + validStyle);

    m_latitude = new Position2D(Position2D::eLATITUDE);
    m_longitude = new Position2D(Position2D::eLONGITUDE);

    // Depending on the format setup the display
    if( m_posFormat == PositionFormatType::eDECIMAL_DEG) {
        setupForDegDisplay();
    } else if( m_posFormat == PositionFormatType::eDMS) {
        setupForDMSDisplay();
    } else {
        // UTM format
        setupForUTMDisplay();
    }

    // Arrange widgets
    m_layout = new QGridLayout();
    m_layout->addWidget(new QLabel("<b>Latitude:</b>"), 0, 0);
    m_layout->addWidget(m_latLineEdit, 0, 1);
    m_layout->addWidget(new QLabel("<b>Longitude:</b>"), 1, 0);
    m_layout->addWidget(m_lonLineEdit, 1, 1);
    m_layout->setMargin(0);
    this->setLayout(m_layout);

    connect(m_latLineEdit, &QLineEdit::textChanged, this, &LatLonWidget::textChanged);
    connect(m_lonLineEdit, &QLineEdit::textChanged, this, &LatLonWidget::textChanged);
}

void LatLonWidget::setupForDegDisplay()
{
    // Latitude (degree format)
    m_latLineEdit->setInputMask(latDegInputMask);
    m_latDegValidator->setRegularExpression(QRegularExpression(latDegRegExp));
    m_latLineEdit->setValidator(m_latDegValidator);
    m_latLineEdit->setText(m_latitude->getInDegFormat());

    // Longitude (degree format)
    m_lonLineEdit->setInputMask(lonDegInputMask);
    m_lonDegValidator->setRegularExpression(QRegularExpression(lonDegRegExp));
    m_lonLineEdit->setValidator(m_lonDegValidator);
    m_lonLineEdit->setText(m_longitude->getInDegFormat());
}

void LatLonWidget::setupForDMSDisplay()
{
    // Latitude (DMS format)
    m_latLineEdit->setInputMask(latDmsInputMask);
    m_latDmsValidator->setRegularExpression(QRegularExpression(latDmsRegExp));
    m_latLineEdit->setValidator(m_latDmsValidator);
    m_latLineEdit->setText(m_latitude->getInDMSFormat());

    // Longitude (DMS format)
    m_lonLineEdit->setInputMask(lonDmsInputMask);
    m_lonDmsValidator->setRegularExpression(QRegularExpression(lonDmsRegExp));
    m_lonLineEdit->setValidator(m_lonDmsValidator);
    m_lonLineEdit->setText(m_longitude->getInDMSFormat());
}

void LatLonWidget::setupForUTMDisplay()
{
    // Northing (UTM)
    m_latLineEdit->setInputMask(latUtmInputMask);
    m_latValidator->setRegularExpression(QRegularExpression(latUtmRegExp));
    m_latLineEdit->setValidator(m_latDmsValidator);

    m_lonLineEdit->setInputMask(lonUtmInputMask);
    m_lonValidator->setRegularExpression(QRegularExpression(lonUtmRegExp));
    m_lonLineEdit->setValidator(m_lonValidator);

    double latitude = m_latitude->getValue();
    double longitude = m_longitude->getValue();

    double northing, easting;
    char zone[5];
    // int zone = LatLonToUTMXY( DEG_2_RAD(latitude), DEG_2_RAD(longitude), 0, x, y);
    UTM::LLtoUTM(latitude, longitude, northing, easting, &zone[0]);
    QString z(zone);
    m_latLineEdit->setText(QString("%1 %2 m").arg(z).arg(northing, 6, 'f', 0, QChar('0')));
    m_lonLineEdit->setText(QString("%1 m").arg(easting, 6, 'f', 0, QChar('0')));
}


void LatLonWidget::textChanged()
{    
    QLineEdit *lineEdit = reinterpret_cast<QLineEdit *>(sender());

    if( !lineEdit->hasFocus())
        return;

    // get current displayed text
    QString tmp = lineEdit->displayText();

    if( m_posFormat == PositionFormatType::eDECIMAL_DEG) {
        tmp.chop(1);
        QStringList f = tmp.split('.');
        int32_t exp = f[0].toInt();
        int32_t frac = f[1].toInt();

        if( lineEdit->objectName() == "Latitude" ) {
            validateAndUpdatePosition(exp, frac, m_latitude, lineEdit);
        }
        else {
            validateAndUpdatePosition(exp, frac, m_longitude, lineEdit);
        }
    } else if(m_posFormat == PositionFormatType::eDMS) {
        QStringList f = tmp.split(" ");        
        f[1].chop(1);
        f[2].chop(1);
        f[3].chop(1);

        int32_t exp = f[1].toInt();
        int32_t fraction = static_cast<int32_t>(((f[2].toDouble() / 60.0) + (f[3].toDouble() / 3600.0)) * 1000000);

        if( lineEdit->objectName() == "Latitude" ) {
            if(f[0] == 'S')
                exp *= -1;

            validateAndUpdatePosition(exp, fraction, m_latitude, lineEdit);
//            m_latitude->setValue(exp, fraction);
        } else {
            if(f[0] == 'W')
                exp *= -1;
            validateAndUpdatePosition(exp, fraction, m_longitude, lineEdit);
//            m_longitude->setValue(exp, fraction);
        }
    } else {

        // UTM
        double newNorthing, newEasting;
        double latitude, longitude;

        if( lineEdit->objectName() == "Latitude" ) {
            // Northing updated
            // get zone and northing
            qDebug() << "textChange: latitude update";
            QStringList f = tmp.split(' ');
            newNorthing = f[1].toDouble();
            QByteArray newZone(f[0].toUtf8());
            qDebug() << f[0] << f[1];

            QString s = m_lonLineEdit->displayText();
            QStringList f2 = s.split(' ');
            double easting = f2[0].toDouble();

            qDebug() << newZone.constData() << "north: " << newNorthing << "east: " << easting;
            UTM::UTMtoLL(newNorthing, easting, newZone.constData(), latitude, longitude);
            ignore = true;
            setPosition(latitude, longitude);
            qDebug() << "Latitude Update: " << latitude <<  longitude;
        } else {
            QStringList f = tmp.split(' ');
            newEasting = f[0].toDouble();

            QString s = m_latLineEdit->displayText();
            QStringList f2 = s.split(' ');
            double northing = f2[1].toDouble();
            QByteArray zone(f2[0].toUtf8());


            qDebug() << zone << "north: " << northing << "east: " << newEasting;
            UTM::UTMtoLL(northing, newEasting, zone.constData(), latitude, longitude);
            ignore = true;
            setPosition(latitude, longitude);
            qDebug() << "Longitude Update: " << latitude <<  longitude;
        }

    }

    // Check if entered value in valid
//    if( !lineEdit->hasAcceptableInput() ) {
//        lineEdit->setStyleSheet(lineEditStyle + validStyle);
//    } else {
//        lineEdit->setStyleSheet(lineEditStyle + validStyle);
//    }
}

void LatLonWidget::positionFormatChanged(int format)
{
    m_posFormat = static_cast<PositionFormatType>(format);

    if( m_posFormat == PositionFormatType::eDECIMAL_DEG) {
        setupForDegDisplay();
    } else if( m_posFormat == PositionFormatType::eDMS) {
        setupForDMSDisplay();
    } else {
        setupForUTMDisplay();
    }
}

void LatLonWidget::setPosition(const double &latitude, const double &longitude)
{
    m_latitude->setValue(latitude);
    m_longitude->setValue(longitude);

    if( m_posFormat == PositionFormatType::eDECIMAL_DEG) {
        m_latLineEdit->setText(m_latitude->getInDegFormat());
        m_lonLineEdit->setText(m_longitude->getInDegFormat());
    }
    else if( m_posFormat == PositionFormatType::eDMS) {
        m_latLineEdit->setText(m_latitude->getInDMSFormat());
        m_lonLineEdit->setText(m_longitude->getInDMSFormat());
    } else {
        if(ignore) {
            ignore = false;
            return;
        }
        double lat = m_latitude->getValue();
        double lon = m_longitude->getValue();

        double northing, easting;
        char zone[5];
        UTM::LLtoUTM(lat, lon, northing, easting, &zone[0]);
        QString z(zone);
        m_latLineEdit->setText(QString("%1 %2 m").arg(z).arg(northing, 6, 'f', 0, QChar('0')));
        m_lonLineEdit->setText(QString("%1 m").arg(easting, 6, 'f', 0, QChar('0')));
    }
}

void LatLonWidget::getPosition(double &latitude, double &longitude)
{
    latitude = m_latitude->getValue();
    longitude = m_longitude->getValue();
}

bool LatLonWidget::isLatitudeValid(int32_t exp, int32_t frac)
{
    bool valid {true};
    if((exp > 90) || (exp < -90) || ( (exp == 90 || exp == -90) && (frac > 0)))
        valid = false;
    return valid;
}

bool LatLonWidget::isLongitudeValid(int32_t exp, int32_t frac)
{
    bool valid {true};
    if((exp > 180) || (exp < -180) || ( (exp == 180 || exp == -180) && (frac > 0)))
        valid = false;
    return valid;
}

void LatLonWidget::validateAndUpdatePosition(int32_t exp, int32_t frac, Position2D *pos, QLineEdit *edit)
{
    bool isValid = true;
    if( pos->getType() == Position2D::eLATITUDE ) {
        isValid = isLatitudeValid(exp, frac);
    } else {
        isValid = isLongitudeValid(exp, frac);
    }

    pos->setValue(exp, frac);
    if( isValid ) {
        edit->setStyleSheet(lineEditStyle + validStyle);

    } else {
        // invalid input don't update
        edit->setStyleSheet(lineEditStyle + invalidStyle);
    }
}

