#ifndef POSITOIN2D_H
#define POSITOIN2D_H
#include <cinttypes>
#include <QString>

class Position2D
{
public:
    enum PositionType {
        eLATITUDE,
        eLONGITUDE,
        eUTM_NORTH,
        eUTM_EAST
    };

    Position2D(PositionType t);

    PositionType getType() const;
    void setValue(double value);
    void setValue(int32_t e, int32_t f);
    double getValue() const;
    QString getInDegFormat() const;
    QString getInDMSFormat() const;
//    QString getInUTMFormat() const;

private:
    int32_t m_exponent;
    int32_t m_fraction;

    PositionType m_posType;
};

#endif // POSITOIN2D_H
