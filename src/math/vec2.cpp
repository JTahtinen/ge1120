#include "vec2.h"
#include <math.h>

Vec2::Vec2(float x, float y)
    : x(x), y(y)
{
}

float Vec2::length() const
{
    return sqrtf(x * x + y * y);
}

Vec2 Vec2::normalize() const
{
    return Vec2(x, y) * (1.0f / length());
}

Vec2 Vec2::add(const Vec2 other) const
{
    return {x + other.x, y + other.y};
}

Vec2 Vec2::subst(const Vec2 other) const
{
    return {x - other.x, y - other.y};
}

Vec2 Vec2::mul(float value) const
{
    return {x * value, y * value};
}

float Vec2::dot(const Vec2 other) const
{
    return x * other.x + y * other.y;
}

bool Vec2::equals(const Vec2 other) const
{
    return (x == other.x && y == other.y);
}

Vec2 Vec2::operator+(const Vec2 other) const
{
    return this->add(other);
}

Vec2& Vec2::operator+=(const Vec2 other)
{
    this->x += other.x;
    this->y += other.y;
    return *this;
}

Vec2 Vec2::operator-(const Vec2 other) const
{
    return this->subst(other);
}

Vec2 &Vec2::operator-=(const Vec2 other)
{
    this->x -= other.x;
    this->y -= other.y;
    return *this;
}

Vec2 Vec2::operator*(float value) const
{
    return this->mul(value);
}

Vec2 &Vec2::operator*=(float value)
{
    this->x *= value;
    this->y *= value;
    return *this;
}

float Vec2::operator*(const Vec2 other) const
{
    return this->dot(other);
}

bool Vec2::operator==(const Vec2 other) const
{
    return this->equals(other);
}

bool Vec2::operator!=(const Vec2 other) const
{
    return !this->equals(other);
}