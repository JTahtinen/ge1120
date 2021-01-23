#include "vec4.h"
#include <math.h>

Vec4::Vec4(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w)
{
}

float Vec4::length() const
{
    return sqrtf(x * x + y * y + z * z + w * w);
}

Vec4 Vec4::normalize() const
{
    return Vec4(x, y, z, w) * (1.0f / length());
}

Vec4 Vec4::add(const Vec4 other) const
{
    return {x + other.x, y + other.y, z + other.z, w + other.w};
}

Vec4 Vec4::subst(const Vec4 other) const
{
    return {x - other.x, y - other.y, z - other.z, w - other.w};
}

Vec4 Vec4::mul(float value) const
{
    return {x * value, y * value, z * value, w * value};
}

float Vec4::dot(const Vec4 other) const
{
    return x * other.x + y * other.y + z * other.z, w * other.w;
}

bool Vec4::equals(const Vec4 other) const
{
    return (x == other.x && y == other.y && z == other.z && w == other.w);
}

Vec4 Vec4::operator+(const Vec4 other) const
{
    return this->add(other);
}

Vec4& Vec4::operator+=(const Vec4 other)
{
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
    this->w += other.w;
    return *this;
}

Vec4 Vec4::operator-(const Vec4 other) const
{
    return this->subst(other);
}

Vec4 &Vec4::operator-=(const Vec4 other)
{
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
    this->w -= other.w;
    return *this;
}

Vec4 Vec4::operator*(float value) const
{
    return this->mul(value);
}

Vec4 &Vec4::operator*=(float value)
{
    this->x *= value;
    this->y *= value;
    return *this;
}

float Vec4::operator*(const Vec4 other) const
{
    return this->dot(other);
}

bool Vec4::operator==(const Vec4 other) const
{
    return this->equals(other);
}

bool Vec4::operator!=(const Vec4 other) const
{
    return !this->equals(other);
}