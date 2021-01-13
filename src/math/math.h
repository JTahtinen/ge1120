#pragma once
#include "vec2.h"
#include <math.h>

#define MATH_PI 3.14159265
#define TO_RADIANS(angle) ((MATH_PI/180.f) * angle)

inline float getXAtLinePoint(const Vec2 line, float y)
{
    if (line.y == 0) return line.x;
    float result = (line.x / line.y) * y;
    return result;
}

inline float getYAtLinePoint(const Vec2 line, float x)
{
    if (line.x == 0) return line.y;
    float result = (line.y / line.x) * x;
    return result;
}

inline int absINT(int val)
{
    if (val < 0) return -val;
    return val;
}