#pragma once
#include "../math/vec2.h"
class Texture;

struct Entity
{
    unsigned int vao;
    const Texture* texture;
    Vec2 pos;
    Vec2 vel;
    float speed;
};