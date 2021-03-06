#include "camera.h"
#include "../math/mat3.h"
#include "../math/math.h"
#include "../globals.h"

Vec2 Camera::getTopLeft() const
{

    //Vec2 result = calculateViewMatrix(this) * Vec2(-1.0f, 1.0f);
    Vec2 result = Mat3::translation(entity.pos) * Mat3::rotation(TO_RADIANS(entity.rotation)) * Mat3::scale(Vec2(zoom, zoom)) * Vec2(-1.0f, 1.0f * (9.0f / 16.0f));
    return result;
}

Vec2 Camera::getTopRight() const
{
    Vec2 result = Mat3::translation(entity.pos) * Mat3::rotation(TO_RADIANS(entity.rotation)) * Mat3::scale(Vec2(zoom, zoom)) * Vec2(1.0f, 1.0f * (9.0f / 16.0f));
    return result;
}

Vec2 Camera::getBottomLeft() const
{
    Vec2 result = Mat3::translation(entity.pos) * Mat3::rotation(TO_RADIANS(entity.rotation)) * Mat3::scale(Vec2(zoom, zoom)) * Vec2(-1.0f, -1.0f * (9.0f / 16.0f));
    return result;
}

Vec2 Camera::getBottomRight() const
{
    Vec2 result = Mat3::translation(entity.pos) * Mat3::rotation(TO_RADIANS(entity.rotation)) * Mat3::scale(Vec2(zoom, zoom)) * Vec2(1.0f, -1.0f * (9.0f / 16.0f));
    return result;
}

Mat3 calculateViewMatrix(const Camera* camera)
{
    Mat3 result = Mat3::view(camera->entity.pos, TO_RADIANS(camera->entity.rotation), 1, g_aspect);
    return result;
}

void updateCamera(Camera* camera)
{
    float zoomDiff = camera->zoomTarget - camera->zoom;
    if (abs(zoomDiff) < 0.01f) camera->zoom = camera->zoomTarget;
    if (zoomDiff > 0)
    {
        camera->zoom += zoomDiff * g_frameTime * 4.0f;
        if (camera->zoom > camera->zoomTarget) camera->zoom = camera->zoomTarget;
    }
    else if (zoomDiff < 0)
    {
        camera->zoom -= -zoomDiff * g_frameTime * 4.0f;
        if (camera->zoom < camera->zoomTarget) camera->zoom = camera->zoomTarget;
    }
}

void increaseZoom(Camera* camera)
{
    camera->zoomTarget *= 1.5f;
}
void decreaseZoom(Camera* camera)
{
    camera->zoomTarget /= 1.5f;
}
