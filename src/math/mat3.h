#pragma once
#include "vec2.h"
#include "vec3.h"

union Mat3
{
   
    float units[9];
    Vec3  rows[3];
   
    
    Mat3() {}
    Mat3(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2);
    Vec2 mul(const Vec2 vec) const;
    Mat3 mul(const Mat3& other) const;
    static Mat3 identity();
    static Mat3 rotation(float angle);
    static Mat3 translation(Vec2 vec);
    static Mat3 scale(Vec2 vec);
    
    static Mat3 view(Vec2 pos, float angle, float scalar, float aspect);

    Mat3 operator*(const Mat3& other) const;
    Vec2 operator*(const Vec2 other) const;
    float operator[](unsigned int index) const;
    float& operator[](unsigned int index);
};
