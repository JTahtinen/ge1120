#include "Vec3.h"
#include <math.h>

Vec3::Vec3(float x, float y, float z)
    : x(x), y(y), z(z)
{
}

float Vec3::length() const
{
    return sqrtf(x * x + y * y + z * z);
}

Vec3 Vec3::normalize() const
{
    return Vec3(x, y, z) * (1.0f / length());
}

Vec3 Vec3::add(const Vec3 other) const
{
    return {x + other.x, y + other.y, z + other.z};
}

Vec3 Vec3::subst(const Vec3 other) const
{
    return {x - other.x, y - other.y, z - other.z};
}

Vec3 Vec3::mul(float value) const
{
    return {x * value, y * value, z * value};
}

float Vec3::dot(const Vec3 other) const
{
    return x * other.x + y * other.y + z * other.z;
}

bool Vec3::equals(const Vec3 other) const
{
    return (x == other.x && y == other.y && z == other.z);
}

Vec3 Vec3::operator+(const Vec3 other) const
{
    return this->add(other);
}

Vec3& Vec3::operator+=(const Vec3 other)
{
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
    return *this;
}

Vec3 Vec3::operator-(const Vec3 other) const
{
    return this->subst(other);
}

Vec3 &Vec3::operator-=(const Vec3 other)
{
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
    return *this;
}

Vec3 Vec3::operator*(float value) const
{
    return this->mul(value);
}

Vec3 &Vec3::operator*=(float value)
{
    this->x *= value;
    this->y *= value;
    this->z *= value;
    return *this;
}

float Vec3::operator*(const Vec3 other) const
{
    return this->dot(other);
}

bool Vec3::operator==(const Vec3 other) const
{
    return this->equals(other);
}

bool Vec3::operator!=(const Vec3 other) const
{
    return !this->equals(other);
}