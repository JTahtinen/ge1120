#pragma once
#include "../math/vec2.h"
#include "../math/mat3.h"
#include "../math/math.h"
#include "vertexarray.h"

enum RenderType
{
    RENDER_SOLID,
    RENDER_WIREFRAME,
    RENDER_SOLID_AND_WIREFRAME
};

class Shader;
class Texture;

struct LineData;
struct QuadData;

struct Renderer
{
    Shader*         shader {nullptr};
    Shader*         lineShader {nullptr};
    Shader*         quadShader {nullptr};
    VertexArray*    lineBatchVAO;
    VertexArray*    quadBatchVAO;
    LineData*       lineData;
    Mat3            viewMatrix;
    unsigned int    numBatchLineVertices;
    unsigned int    numBatchLineIndices;
    QuadData*       quadData;
    IndexBuffer*    quadBatchIBO;
    unsigned int    numBatchQuadVertices;
    unsigned int    numBatchQuadIndices;
    Renderer();
    ~Renderer();
    void renderVAO(VertexArray* vao, const Texture* texture, const Mat3& model, const Mat3& view,  RenderType renderType = RENDER_SOLID);
    void renderLine(VertexArray* vao);
    void submitLine(float x0, float y0, float x1, float y1, Vec2 offset);
    void submitLine(Vec2 point0, Vec2 point1, Vec2 offset);
    void submitQuad(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, Vec2 position, Vec4 color);
    void submitQuad(Vec2 point0, Vec2 point1, Vec2 point2, Vec2 point3, Vec2 position, Vec4 color);
    void submitQuad(Quad quad, Vec2 position, Vec4 color);
    void setView(Mat3 view);
    void flush();
};