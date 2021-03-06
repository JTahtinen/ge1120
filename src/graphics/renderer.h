#pragma once
#include "../math/vec2.h"
#include "../math/mat3.h"
#include "../math/math.h"
#include "batch.h"
#include "sprite.h"
#include <string>
#include <vector>
#include "../util/vector.h"
struct Font;

enum RenderType
{
    RENDER_SOLID,
    RENDER_WIREFRAME,
    RENDER_SOLID_AND_WIREFRAME
};

class Shader;

struct Renderer
{
    Shader*             shader {nullptr};
    Shader*             lineShader {nullptr};
    Shader*             quadShader {nullptr};
    Shader*             letterShader {nullptr};
    Vector<SpriteBatch> spriteBatches;
    LineBatch           lineBatch;
    QuadBatch           quadBatch;
    LetterBatch         letterBatch;
    Mat3                viewMatrix;
    Font*               font;
    Renderer();
    ~Renderer();
    //void renderVAO(VertexArray* vao, const Texture* texture, const Mat3& model,
    //               const Mat3& view, RenderType renderType = RENDER_SOLID);
    void renderLine(VertexArray* vao);
    void submitLine(float x0, float y0, float x1, float y1, Vec2 offset);
    void submitLine(Vec2 point0, Vec2 point1, Vec2 offset);
    void submitSprite(const Sprite* sprite, Mat3 model, Mat3 view);
    void submitQuad(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, Vec2 position, Vec4 color);
    void submitQuad(Vec2 point0, Vec2 point1, Vec2 point2, Vec2 point3, Vec2 position, Vec4 color);
    void submitQuad(Quad quad, Vec2 position, Vec4 color);
    void submitText(const std::string& text, Vec2 pos, Mat3 view, float scale);
    void submitText(const std::string& text, Vec2 pos, float scale);
    void submitText(const std::string& text, Vec2 pos);
    void setView(Mat3 view);
    void setFont(Font* font);
    void flushSprites();
    void flushQuads();
    void flushLetters();
    void flushLines();
    void flush();
};
