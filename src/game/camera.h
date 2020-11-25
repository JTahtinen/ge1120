#pragma once
#include "entity.h"
#include "../math/vec2.h"

struct Camera
{
    Entity entity;
    
    Vec2 getTopLeft() const;
    Vec2 getTopRight() const;
    Vec2 getBottomLeft() const;
    Vec2 getBottomRight() const;
};