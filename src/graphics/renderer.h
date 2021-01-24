#pragma once
#include "../math/vec2.h"
#include "../math/mat3.h"
#include "vertexarray.h"

enum RenderType
{
    RENDER_SOLID,
    RENDER_WIREFRAME,
    RENDER_SOLID_AND_WIREFRAME
};

class Shader;
class Texture;

struct QuadData;

struct Renderer
{
    Shader*         shader {nullptr};
    Shader*         lineShader {nullptr};
    Shader*         quadShader {nullptr};
    VertexArray*    lineBatchVAO;
    VertexArray*    quadBatchVAO;
    Vec2*           lineData;
    unsigned int    numBatchLineVertices;
    QuadData*       quadData;
    unsigned int    numBatchQuadVertices;
    IndexBuffer*    quadBatchIBO;
    unsigned int    numBatchQuadIndices;
    Renderer();
    ~Renderer();
    void renderVAO(VertexArray* vao, const Texture* texture, const Mat3& model, const Mat3& view,  RenderType renderType = RENDER_SOLID);
    void renderLine(VertexArray* vao);
    void submitLine(float x0, float y0, float x1, float y1);
    void submitQuad(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
    void flush();
};