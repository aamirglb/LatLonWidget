#include "position2d.h"
#include <QDebug>
#include <QRegularExpression>
#include "utm.h"

Position2D::Position2D(PositionType t)
{
    m_posType = t;
    m_exponent = 0;
    m_fraction = 0;
}

Position2D::PositionType Position2D::getType() const
{
    return m_posType;
}

void Position2D::setValue(double value)
{
    QStringList f = QString::number(value, 'f', 6).split('.');
    m_exponent = f[0].toInt();
    m_fraction = f[1].toInt();
}

void Position2D::setValue(int32_t e, int32_t f)
{
    QStringList lst = QString::number(f/1000000.0, 'f', 6).split('.');

    bool found {false};
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
        f += 1;
    }

    m_exponent = e;
    m_fraction = f;
}

double Position2D::getValue() const
{
    return (double(m_exponent) + (m_fraction/1000000.0));
}

QString Position2D::getInDegFormat() const
{
    int32_t fieldWidth = 10;
    int32_t degFieldWidth = 3;
    if( m_posType == eLATITUDE) {
        fieldWidth = 9;
        degFieldWidth = 2;
    }

    QString tmp = "+";
    if( m_exponent < 0 ) {
        tmp = "-";
    }
    QStringList f = QString::number(getValue(), 'f', 6).split('.');
    qDebug() << "Lat (Deg): " << QString("%1").arg(getValue(), fieldWidth, 'f', 6) << getValue();
    return QString("%1%2.%3\u00B0").arg(tmp).
            arg(f[0].toInt(), degFieldWidth, 10, QChar('0')).
            arg(f[1].toInt(), 6, 10, QChar('0'));

}

QString Position2D::getInDMSFormat() const
{
    int32_t degFieldWidth = 3;
    QString direction = "N";

    if( m_posType == eLATITUDE) {
        degFieldWidth = 2;
        if( m_exponent < 0 )
            direction = "S";
    } else {
        degFieldWidth = 3;
        if( m_exponent < 0 ) {
            direction = "W";
        } else {
            direction = "E";
        }
    }

    int32_t deg = m_exponent;
    int32_t min = (m_fraction * 6) / 100000;
    double sec = ((m_fraction / 1000000.0) - (min/60.0)) * 3600;

    return QString("%1 %2\u00B0 %3' %4\"").arg(direction).
                arg(deg, degFieldWidth, 10, QChar('0')).
                arg(min, 2, 10, QChar('0')).
            arg(sec, 5, 'f', 2, QChar('0'));
}


