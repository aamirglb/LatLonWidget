#ifndef LATLONWIDGET_H
#define LATLONWIDGET_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QGridLayout;
class QRegularExpressionValidator;
struct FloatType;
class MyLineEdit;

class LatLonWidget : public QWidget
{
    Q_OBJECT
public:

    enum PositionFormatType {
        eDECIMAL_DEG,
        eDMS,
        eUTM
    };

    enum ValueType {
        eLATITUDE,
        eLONGITUDE
    };

    enum class NotationType {
        eSIGN,
        eDIRECTION
    };

    // String Constants
    QString latDegInputMask = QStringLiteral("#00.000000\u00B0;0");
    QString latDecimalDegMask = QStringLiteral("x 00.000000\u00B0;0");
    QString latDMSInputMask = QStringLiteral("x 00\u00B0 00' 00.00\";0");
    QString latUTMInputMask = QStringLiteral("99A 0000000 m;0");

    QString latDegRegExp = QStringLiteral("(-|\\+)\\d{1,2}\\.\\d{0,6}\u00B0");
    QString latDecimalDegRegExp = QStringLiteral("^(N|S|n|s) \\d{1,2}\\.\\d{0,6}\u00B0");
    QString latDMSRegExp = QStringLiteral("^(N|S|n|s)\\s \\d{1,2}\u00B0\\s [0-5][0-9]'\\s [0-5][0-9].\\d{1,2}\"");
    QString latUTMRegExp = QStringLiteral("[0-9][0-9][C-Z] \\d{0,7} m");

    QString lonDegInputMask = QStringLiteral("#000.000000\u00B0;0");
    QString lonDecimalDegMask = QStringLiteral("x 000.000000\u00B0;0");
    QString lonDMSInputMask = QStringLiteral("x 000\u00B0 00' 00.00\";0");
    QString lonUTMInputMask = QStringLiteral("000000 m;0");

    QString lonDegRegExp = QStringLiteral("(-|\\+)[0-1]\\d{1,2}.\\d{0,6}\u00B0");
    QString lonDecimalDegRegExp = QStringLiteral("^(E|W|e|w) [0-1]\\d{1,2}.\\d{0,6}\u00B0");
    QString lonDMSRegExp = QStringLiteral("^(E|W|e|w)\\s [0-1]\\d{1,2}\u00B0\\s [0-5][0-9]'\\s [0-5][0-9].\\d{1,2}\"");
    QString lonUTMRegExp = QStringLiteral("\\d{0,6} m");

    QString validStyle = QStringLiteral("border-width: 1px; border-color: white;");
    QString invalidStyle = QStringLiteral("border-width: 2px; border-color: red;");
    QString lineEditStyle = QStringLiteral("background-color: black; border-style: solid; font-weight: bold; color: lime;");


    explicit LatLonWidget(QWidget *parent = nullptr);

    // Setup methods
    void setupDegDisplay();
    void setupDMSDisplay();
    void setupUTMDisplay();
    void setLabels(const QString &label1, const QString &label2);

    QString format(PositionFormatType posFormat, ValueType type, FloatType *value);
    void formatUTM();

    void setNotation(NotationType notation);

public slots:
    void textChanged();
    void setPositionFormat(int format);
    void setPosition(const double &latitude, const double &longitude);
    void getPosition(double &latitude, double &longitude);

public:
    void setReadOnly(bool flag);


private:    
    inline bool isLatitudeValid(int32_t whole, int32_t frac);
    inline bool isLongitudeValid(int32_t whole, int32_t frac);
    void validateAndUpdatePosition(ValueType type, int32_t whole,
                                   int32_t frac, FloatType *value, QLineEdit *edit);

signals:
    void isPositionValid(bool);
    void latitudeChanged(double);
    void longitudeChanged(double);

private:
    PositionFormatType m_posFormat = PositionFormatType::eDECIMAL_DEG;

    // Ui elements
    MyLineEdit *m_latLineEdit{};
    MyLineEdit *m_lonLineEdit{};
    QLabel *m_label1;
    QLabel *m_label2;
    QGridLayout *m_layout{};

    // RegEx validators
    QRegularExpressionValidator *m_latValidator{};
    QRegularExpressionValidator *m_lonValidator{};

    FloatType *m_latitude{};
    FloatType *m_longitude{};

    NotationType m_decimalDegNotation {NotationType::eSIGN};

    bool m_isReadOnly {};
    bool m_isLatValid {true};
    bool m_isLonValid {true};    
};

#endif // LATLONWIDGET_H
