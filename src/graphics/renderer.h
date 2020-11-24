#pragma once
#include "../math/vec2.h"
#include "../math/mat3.h"
class Shader;
class Texture;
struct Renderer
{
    Shader*     shader {nullptr};
    
    void renderVAO(unsigned int vao, const Texture* texture, const Mat3& model, const Mat3& view);
    void bind() const;
};