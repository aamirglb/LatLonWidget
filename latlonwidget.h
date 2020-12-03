#ifndef LATLONWIDGET_H
#define LATLONWIDGET_H

#include <QWidget>

class QLineEdit;
class QGridLayout;
class QRegularExpressionValidator;

class Position2D;

class LatLonWidget : public QWidget
{
    Q_OBJECT
public:

    enum PositionFormatType {
        eDECIMAL_DEG,
        eDMS,
        eUTM
    };

    // String Constants
    QString latDegInputMask = QStringLiteral("#00.000000\u00B0;0");
    QString latDmsInputMask = QStringLiteral("x 00\u00B0 00' 00.00\";0");
    QString latUtmInputMask = QStringLiteral("99A 0000000 m;0");

    QString latDegRegExp = QStringLiteral("(-|\\+)\\d{1,2}\\.\\d{0,6}\u00B0");
    QString latDmsRegExp = QStringLiteral("^(N|S) \\d{1,2}\u00B0 [0-5][0-9]' [0-5][0-9].\\d{1,2}\"");
    QString latUtmRegExp = QStringLiteral("[0-9][0-9][C-Z] \\d{0,7} m");

    QString lonDegInputMask = QStringLiteral("#000.000000\u00B0;0");
    QString lonDmsInputMask = QStringLiteral("x 000\u00B0 00' 00.00\";0");
    QString lonUtmInputMask = QStringLiteral("000000 m;0");

    QString lonDegRegExp = QStringLiteral("(-|\\+)[0-1]\\d{1,2}.\\d{0,6}\u00B0");
    QString lonDmsRegExp = QStringLiteral("^(E|W) [0-1]\\d{1,2}\u00B0 [0-5][0-9]' [0-5][0-9].\\d{1,2}\"");
    QString lonUtmRegExp = QStringLiteral("\\d{0,6} m");

    QString validStyle = QStringLiteral("border-width: 1px; border-color: white;");
    QString invalidStyle = QStringLiteral("border-width: 2px; border-color: red;");
    QString lineEditStyle = QStringLiteral("background-color: black; border-style: solid; font-weight: bold; color: lime;");

    explicit LatLonWidget(PositionFormatType format = eDECIMAL_DEG, QWidget *parent = nullptr);

    void setupForDegDisplay();
    void setupForDMSDisplay();
    void setupForUTMDisplay();

public slots:
    void textChanged();
    void positionFormatChanged(int format);
    void setPosition(const double &latitude, const double &longitude);
    void getPosition(double &latitude, double &longitude);

private:
    inline bool isLatitudeValid(int32_t exp, int32_t frac);
    inline bool isLongitudeValid(int32_t exp, int32_t frac);
    void validateAndUpdatePosition(int32_t exp, int32_t frac, Position2D *pos, QLineEdit *edit);

private:
    PositionFormatType m_posFormat;

    // Ui elements
    QLineEdit *m_latLineEdit{};
    QLineEdit *m_lonLineEdit{};
    QGridLayout *m_layout{};

    // RegEx validators
    QRegularExpressionValidator *m_latDegValidator{};
    QRegularExpressionValidator *m_latDmsValidator{};
    QRegularExpressionValidator *m_lonDegValidator{};
    QRegularExpressionValidator *m_lonDmsValidator{};

    QRegularExpressionValidator *m_latValidator{};
    QRegularExpressionValidator *m_lonValidator{};

    Position2D *m_latitude{};
    Position2D *m_longitude{};
    bool ignore{};
};

#endif // LATLONWIDGET_H
