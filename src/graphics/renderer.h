#pragma once
#include "../math/vec2.h"
class Shader;
class Texture;
struct Renderer
{
    Shader*     shader {nullptr};
    
    void renderVAO(unsigned int vao, const Texture* texture, Vec2 offset);
    void bind() const;
};