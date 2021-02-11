#include "renderer.h"
#include "../defs.h"
#include "texture.h"
#include "shader.h"
#include "../math/math.h"
#include <iostream>
#include "../globals.h"
#include "font.h"

struct QuadData
{
    Vec2 pos;
    Vec4 color;
    Vec3 model[3];
    Vec3 view[3];
};

struct LineData
{
    Vec2 pos;
    Vec3 model[3];
    Vec3 view[3];
};

struct LetterData
{
    Vec2 pos;
    Vec2 texCoord;
};

#define MAX_RENDERABLES (60000)
#define QUAD_VERTEX_SIZE (sizeof(QuadData))
#define QUAD_SIZE (QUAD_VERTEX_SIZE * 4)
#define QUAD_BUFFER_SIZE (QUAD_SIZE * MAX_RENDERABLES)
#define QUAD_INDICES_SIZE (MAX_RENDERABLES * 6)

#define LETTER_VERTEX_SIZE (sizeof(LetterData))
#define LETTER_SIZE (LETTER_VERTEX_SIZE * 4)
#define LETTER_BUFFER_SIZE (LETTER_SIZE * MAX_RENDERABLES)
#define LETTER_INDICES_SIZE (MAX_RENDERABLES * 6)

Renderer::Renderer()
    : numBatchLineVertices(0)
    , numBatchLineIndices(0)
    , numBatchQuadVertices(0)
    , numBatchQuadIndices(0)
    , numBatchLetterVertices(0)
    , numBatchLetterIndices(0)
{
    viewMatrix = Mat3::identity();

    // LINE BATCH INIT
    lineBatchVAO = new VertexArray();
    lineBatchVAO->bind();
    Buffer *lineVBO = new Buffer();
    lineVBO->setData(NULL, sizeof(float) * 200, GL_DYNAMIC_DRAW);
    BufferLayout lineLayout;
    lineLayout.addLayoutElement(GL_FLOAT, 2);
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            lineLayout.addLayoutElement(GL_FLOAT, 3);
        }
    }
    lineVBO->setLayout(&lineLayout);
    lineBatchVAO->addBuffer(lineVBO);

    // QUAD BATCH INIT
    quadBatchVAO = new VertexArray();
    quadBatchVAO->bind();
    quadBatchIBO = new IndexBuffer();
    unsigned int *quadIndices = new unsigned int[QUAD_INDICES_SIZE];
    unsigned int offset = 0;
    for (int i = 0; i < QUAD_INDICES_SIZE; i += 6)
    {
        quadIndices[i + 0] = 0 + offset;
        quadIndices[i + 1] = 1 + offset;
        quadIndices[i + 2] = 2 + offset;
        quadIndices[i + 3] = 2 + offset;
        quadIndices[i + 4] = 3 + offset;
        quadIndices[i + 5] = 0 + offset;
        offset += 4;
    }
    quadBatchIBO->bind();
    quadBatchIBO->setData(quadIndices, QUAD_INDICES_SIZE);
    delete[] quadIndices;
    Buffer *quadVBO = new Buffer();
    quadVBO->setData(NULL, QUAD_BUFFER_SIZE, GL_DYNAMIC_DRAW);
    BufferLayout quadLayout;
    quadLayout.addLayoutElement(GL_FLOAT, 2);
    quadLayout.addLayoutElement(GL_FLOAT, 4);
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            quadLayout.addLayoutElement(GL_FLOAT, 3);
        }
    }
    quadVBO->setLayout(&quadLayout);
    quadBatchVAO->addBuffer(quadVBO);

    //LETTER BATCH INIT
   
    letterBatchVAO = new VertexArray();
    letterBatchVAO->bind();
    letterBatchIBO = new IndexBuffer();
    unsigned int *letterIndices = new unsigned int[LETTER_INDICES_SIZE];
    offset = 0;
    for (int i = 0; i < LETTER_INDICES_SIZE; i += 6)
    {
        letterIndices[i + 0] = 0 + offset;
        letterIndices[i + 1] = 1 + offset;
        letterIndices[i + 2] = 2 + offset;
        letterIndices[i + 3] = 2 + offset;
        letterIndices[i + 4] = 3 + offset;
        letterIndices[i + 5] = 0 + offset;
        offset += 4;
    }
    letterBatchIBO->bind();
    letterBatchIBO->setData(letterIndices, LETTER_INDICES_SIZE);
    delete[] letterIndices;
    Buffer *letterVBO = new Buffer();
    letterVBO->setData(NULL, LETTER_BUFFER_SIZE, GL_DYNAMIC_DRAW);
    BufferLayout letterLayout;
    letterLayout.addLayoutElement(GL_FLOAT, 2);
    letterLayout.addLayoutElement(GL_FLOAT, 2);
    letterVBO->setLayout(&letterLayout);
    letterBatchVAO->addBuffer(letterVBO);
}

Renderer::~Renderer()
{
    delete lineBatchVAO;
    lineBatchVAO = nullptr;
}

void Renderer::renderVAO(VertexArray *vao, const Texture *texture, const Mat3 &model, const Mat3 &view, RenderType renderType)
{
    if (!shader)
    {
        std::cout << "[ERROR] Could not render vao " << vao << ". Shader was NULL!" << std::endl;
        return;
    }
    shader->bind();
    if (!texture)
    {
        std::cout << "[ERROR] Could not render vao " << vao << ". Texture was NULL!" << std::endl;
        return;
    }
    vao->bind();
    Shader *workingShader;
    GLenum glRenderFlag;
    unsigned int numIndices;
    switch (renderType)
    {
    case RENDER_SOLID:
    {
        numIndices = 6;
        g_squareFillIBO->bind();
        workingShader = shader;
        glRenderFlag = GL_TRIANGLES;
        texture->bind();
        break;
    }
    case RENDER_WIREFRAME:
    {
        numIndices = 10;
        g_squareLineIBO->bind();
        workingShader = lineShader;
        glRenderFlag = GL_LINE_STRIP;
        break;
    }
    case RENDER_SOLID_AND_WIREFRAME:
    {
        renderVAO(vao, texture, model, view, RENDER_SOLID);
        renderVAO(vao, texture, model, view, RENDER_WIREFRAME);
        return;
    }
    }

    workingShader->bind();
    workingShader->setUniformMat3("u_Model", model);
    workingShader->setUniformMat3("u_View", view);
    GLCALL(glDrawElements(glRenderFlag, numIndices, GL_UNSIGNED_INT, 0));
}

void Renderer::renderLine(VertexArray *vao)
{
    if (!vao)
    {
        std::cout << "[ERROR] Could not draw line. VAO was NULL!" << std::endl;
        return;
    }
    lineShader->bind();
    vao->bind();
    GLCALL(glDrawArrays(GL_LINES, 0, 2));
}

void Renderer::submitLine(float x0, float y0, float x1, float y1, Vec2 offset)
{
    lineBatchVAO->bind();
    Buffer *buffer = lineBatchVAO->getBuffer(0);
    buffer->bind();
    Mat3 model = Mat3::translation(offset);
    if (!buffer)
    {
        std::cout << "[ERROR] Could not submit line to renderer. Buffer was NULL!" << std::endl;
        return;
    }
    lineData = (LineData *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY) + numBatchLineVertices;
    lineData->pos.x = x0;
    lineData->pos.y = y0;
    for (int i = 0; i < 3; ++i)
    {
        lineData->model[i] = model.rows[i];
        lineData->view[i] = viewMatrix.rows[i];
    }
    lineData++;
    lineData->pos.x = x1;
    lineData->pos.y = y1;
    for (int i = 0; i < 3; ++i)
    {
        lineData->model[i] = model.rows[i];
        lineData->view[i] = viewMatrix.rows[i];
    }
    numBatchLineVertices += 2;

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Renderer::submitLine(Vec2 point0, Vec2 point1, Vec2 offset)
{
    submitLine(point0.x, point0.y, point1.x, point1.y, offset);
}

void Renderer::submitQuad(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, Vec2 position, Vec4 color)
{
    quadBatchVAO->bind();
    Buffer *buffer = quadBatchVAO->getBuffer(0);
    buffer->bind();
    Mat3 model = Mat3::translation(position);
    if (!buffer)
    {
        std::cout << "[ERROR] Could not submit quad to renderer. Buffer was NULL!" << std::endl;
        return;
    }
    quadData = (QuadData *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY) + numBatchQuadVertices;
    quadData->pos.x = x0;
    quadData->pos.y = y0;
    for (int i = 0; i < 3; ++i)
    {
        quadData->model[i] = model.rows[i];
        quadData->view[i] = viewMatrix.rows[i];
    }
    quadData->color = color;
    quadData++;
    quadData->pos.x = x1;
    quadData->pos.y = y1;
    for (int i = 0; i < 3; ++i)
    {
        quadData->model[i] = model.rows[i];
        quadData->view[i] = viewMatrix.rows[i];
    }
    quadData->color = color;
    quadData++;
    quadData->pos.x = x2;
    quadData->pos.y = y2;
    for (int i = 0; i < 3; ++i)
    {
        quadData->model[i] = model.rows[i];
        quadData->view[i] = viewMatrix.rows[i];
    }
    quadData->color = color;
    quadData++;
    quadData->pos.x = x3;
    quadData->pos.y = y3;
    for (int i = 0; i < 3; ++i)
    {
        quadData->model[i] = model.rows[i];
        quadData->view[i] = viewMatrix.rows[i];
    }
    quadData->color = color;
    numBatchQuadVertices += 4;

    numBatchQuadIndices += 6;

    GLCALL(glUnmapBuffer(GL_ARRAY_BUFFER));
}

void Renderer::submitText(const std::string& text, Vec2 pos, float scale)
{
    if (!font)
    {
        message("[ERROR] Could not render text - Font was NULL!\n");
        return;
    }
    letterBatchVAO->bind();
    Buffer* buffer = letterBatchVAO->getBuffer(0);
    buffer->bind();
    letterData = (LetterData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY) + numBatchLetterVertices;
    float xAdvance = 0;
    for (int i = 0; i < text.size(); ++i)
    {
        char c = text[i];
        if (c == ' ')
        {
            xAdvance += 0.05f * scale;
            continue;
        }
        Letter* l = font->getLetter(c);
        if (!l)
        {
            continue;
        }

        float xOff = l->xOffset;
        float yOff = (font->_base - l->height - l->yOffset);
        
        letterData->pos.x = xAdvance + pos.x + xOff * scale;
        letterData->pos.y = pos.y + yOff * scale;        
        letterData->texCoord = Vec2(l->x, l->y + l->height);
        ++letterData;
        
        letterData->pos.x = xAdvance + pos.x + xOff * scale;
        letterData->pos.y = pos.y + (l->height + yOff) * scale;        
        letterData->texCoord = Vec2(l->x, l->y);
        ++letterData;
        
        letterData->pos.x = xAdvance + pos.x + (l->width + xOff) * scale;
        letterData->pos.y = pos.y + (l->height + yOff) * scale;
        letterData->texCoord = Vec2(l->x + l->width, l->y);
        ++letterData;
        
        letterData->pos.x = xAdvance + pos.x + (l->width + xOff) * scale;
        letterData->pos.y = pos.y + yOff * scale;
        letterData->texCoord = Vec2(l->x + l->width, l->y + l->height);
        ++letterData;

        xAdvance += l->xAdvance * scale;
        numBatchLetterVertices += 4;
        numBatchLetterIndices += 6;
    }
    
    GLCALL(glUnmapBuffer(GL_ARRAY_BUFFER));
}

void Renderer::submitQuad(Vec2 point0, Vec2 point1, Vec2 point2, Vec2 point3, Vec2 position, Vec4 color)
{
    submitQuad(point0.x, point0.y, point1.x, point1.y, point2.x, point2.y, point3.x, point3.y, position, color);
}

void Renderer::submitQuad(Quad quad, Vec2 position, Vec4 color)
{
    submitQuad(quad.point0, quad.point1, quad.point2, quad.point3, position, color);
}

void Renderer::setView(Mat3 view)
{
    viewMatrix = view;
}

void Renderer::setFont(Font* font)
{
    this->font = font;
}

// Currently the flush method is arranged only for testing purposes

void Renderer::flush()
{
    lineBatchVAO->bind();
    g_squareLineIBO->bind();
    lineShader->bind();
    GLCALL(glDrawArrays(GL_LINES, 0, numBatchLineVertices));
    numBatchLineVertices = 0;

    quadBatchVAO->bind();
    quadBatchIBO->bind();
    quadShader->bind();
    GLCALL(glDrawElements(GL_TRIANGLES, numBatchQuadIndices, GL_UNSIGNED_INT, 0));
    numBatchQuadVertices = 0;
    numBatchQuadIndices = 0;

    letterBatchVAO->bind();
    letterBatchIBO->bind();
    font->_atlas->bind();
    letterShader->bind();
    GLCALL(glDrawElements(GL_TRIANGLES, numBatchLetterIndices, GL_UNSIGNED_INT, 0));
    numBatchLetterVertices = 0;
    numBatchLetterIndices = 0;
}
