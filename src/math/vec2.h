#pragma once

struct Vec2
{
    float x{0};
    float y{0};
    Vec2(float x, float y);
    Vec2() = default;
    float length() const;
    Vec2 normalize() const;
    Vec2 add(const Vec2 other) const;
    Vec2 subst(const Vec2 other) const;
    Vec2 mul(float value) const;
    float dot(const Vec2 other) const;
    bool equals(const Vec2 other) const;

    Vec2 operator+(const Vec2 other) const;
    Vec2& operator+=(const Vec2 other);
    Vec2 operator-(const Vec2 other) const;
    Vec2 &operator-=(const Vec2 other);
    Vec2 operator*(float value) const;
    Vec2 &operator*=(float value);
    float operator*(const Vec2 other) const;
    bool operator==(const Vec2 other) const;
    bool operator!=(const Vec2 other) const;
};