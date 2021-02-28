#pragma once
#include "../math/math.h"
struct Texture;


struct Sprite
{
    const Texture* texture;
    Quad dimensions;
};
