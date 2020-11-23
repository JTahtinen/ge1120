#pragma once
#include "vec2.h"

struct Mat3
{
    float units[9];

    Vec2 mul(const Vec2 vec) const;
    static Mat3 mul(const Mat3 other);
    static Mat3 rotation(float angle);
    static Mat3 translation(Vec2 vec);
};