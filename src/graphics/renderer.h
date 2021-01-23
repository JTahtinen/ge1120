#pragma once
#include "../math/vec2.h"
#include "../math/mat3.h"
#include "vertexarray.h"

class Shader;
class Texture;
struct Renderer
{
    Shader*     shader {nullptr};
    Shader*     lineShader {nullptr};
    
    void renderVAO(unsigned int vao, const Texture* texture, const Mat3& model, const Mat3& view);
    void renderLine(VertexArray* vao);
    void renderLine(float x0, float y0, float x1, float y1);
};