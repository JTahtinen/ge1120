#include "mat2.h"
#include <math.h>

Vec2 Mat3::mul(const Vec2 vec) const
{
    Vec2 result(vec.x * units[0] + vec.y * units[1] + 1 * units[2],
                vec.x * units[3] + vec.y * units[4] + 1 * units[5]);
    return result;
}

Mat3 Mat3::rotation(float angle) const
{
    Mat3 result = {cosf(angle), -sinf(angle), 0,
                    sin(angle), cos(angle), 0,
                    0, 0, 1};
    return result;
}

Mat3 Mat3::translation(Vec2 vec) const
{  
    Mat3 result = {1.0f, 0, vec.x,
                   0, 1.0f, vec.y,
                   0, 0, 1.0f};
    return result;

}