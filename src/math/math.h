#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat3.h"
#include <math.h>

#define MATH_PI 3.14159265
#define TO_RADIANS(angle) ((MATH_PI/180.f) * angle)

struct Quad
{
    Vec2 point0;
    Vec2 point1;
    Vec2 point2;
    Vec2 point3;
    Quad() {}
    Quad(Vec2 point0, Vec2 point1, Vec2 point2, Vec2 point3) 
        : point0(point0), point1(point1), point2(point2), point3(point3) {}
    Quad(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3) 
        : Quad({x0, y0}, {x1, y1}, {x2, y2}, {x3, y3}) {}
};

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

inline int abs(int val)
{
    if (val >= 0) return val;
    return -val;
}
