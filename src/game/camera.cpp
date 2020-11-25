#include "camera.h"
#include "../math/mat3.h"
#include "../math/math.h"

Vec2 Camera::getTopLeft() const
{
    Vec2 result = Mat3::translation(entity.pos) * Mat3::rotation(TO_RADIANS(entity.rotation)) * Vec2(-1.0f, 1.0f * (9.0f / 16.0f));
    return result;
}

Vec2 Camera::getTopRight() const
{
    Vec2 result = Mat3::translation(entity.pos) * Mat3::rotation(TO_RADIANS(entity.rotation)) * Vec2(1.0f, 1.0f * (9.0f / 16.0f));
    return result;
}

Vec2 Camera::getBottomLeft() const
{
    Vec2 result = Mat3::translation(entity.pos) * Mat3::rotation(TO_RADIANS(entity.rotation)) * Vec2(-1.0f, -1.0f * (9.0f / 16.0f));
    return result;
}

Vec2 Camera::getBottomRight() const
{
    Vec2 result = Mat3::translation(entity.pos) * Mat3::rotation(TO_RADIANS(entity.rotation)) * Vec2(1.0f, -1.0f * (9.0f / 16.0f));
    return result;
}