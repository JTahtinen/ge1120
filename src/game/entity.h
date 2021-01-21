#pragma once
#include "../math/vec2.h"
#include "../graphics/vertexarray.h"
class Texture;

struct Entity
{
    Vec2 pos;
    Vec2 vel;
    float rotation;
};

struct Actor
{
    Entity entity;
    VertexArray* vao;
    const Texture* texture;
    float speed;
};