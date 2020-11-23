#pragma once
#include "../math/vec2.h"
class Texture;

struct Entity
{
    Vec2 pos;
    Vec2 vel;
};

struct Actor
{
    Entity entity;
    unsigned int vao;
    const Texture* texture;
    float speed;
};