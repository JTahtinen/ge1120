#pragma once
#include "vec2.h"

struct Mat3
{
    float units[9];

    Vec2 mul(const Vec2 vec) const;
    Mat3 mul(const Mat3& other) const;
    static Mat3 identity();
    static Mat3 rotation(float angle);
    static Mat3 translation(Vec2 vec);

    Mat3 operator*(const Mat3& other) const;
    Vec2 operator*(const Vec2 other) const;
};