#pragma once
#include "entity.h"
#include "../math/vec2.h"

struct Camera
{
    Entity entity;
    float zoom;
    float zoomTarget;
    
    Vec2 getTopLeft() const;
    Vec2 getTopRight() const;
    Vec2 getBottomLeft() const;
    Vec2 getBottomRight() const;
};

extern Mat3 calculateViewMatrix(const Camera* camera);
extern void updateCamera(Camera* camera);
extern void increaseZoom(Camera* camera);
extern void decreaseZoom(Camera* camera);
