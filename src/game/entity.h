#pragma once
#include "../math/vec2.h"
#include "../graphics/sprite.h"
#include <string>
struct Entity
{
    unsigned int id;
    std::string name;
    Vec2 pos;
    Vec2 vel;
    float rotation;

};

struct Actor
{
    Entity entity;
    Sprite* sprite;
    float speed;
};
