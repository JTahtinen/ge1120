#include "renderer.h"
#include "../defs.h"
#include "texture.h"
#include "shader.h"
#include "../math/mat3.h"
#include "../math/math.h"
#include <iostream>
#include "../globals.h"

struct QuadData
{
    Vec2 pos;
    Vec4 color;
};

Renderer::Renderer()
    : numBatchLineVertices(0)
    , numBatchQuadVertices(0)
{
    lineBatchVAO = new VertexArray();
    lineBatchVAO->bind();
    Buffer *lineVBO = new Buffer();
    lineVBO->setData(NULL, sizeof(float) * 200, GL_DYNAMIC_DRAW);
    BufferLayout lineLayout;
    lineLayout.addLayoutElement(GL_FLOAT, 2);
    lineVBO->setLayout(&lineLayout);
    lineBatchVAO->addBuffer(lineVBO);

    quadBatchVAO = new VertexArray();
    quadBatchVAO->bind();
    Buffer *quadVBO = new Buffer();
    quadVBO->setData(NULL, sizeof(float) * 200, GL_DYNAMIC_DRAW);
    BufferLayout quadLayout;
    quadLayout.addLayoutElement(GL_FLOAT, 2);
    quadLayout.addLayoutElement(GL_FLOAT, 4);
    quadVBO->setLayout(&quadLayout);
    quadBatchVAO->addBuffer(quadVBO);
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

void Renderer::submitLine(float x0, float y0, float x1, float y1)
{
    lineBatchVAO->bind();
    Buffer *buffer = lineBatchVAO->getBuffer(0);
    buffer->bind();
    if (!buffer)
    {
        std::cout << "[ERROR] Could not submit line to renderer. Buffer was NULL!" << std::endl;
        return;
    }
    lineData = (Vec2 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY) + numBatchLineVertices;
    lineData->x = x0;
    lineData->y = y0;
    lineData++;
    lineData->x = x1;
    lineData->y = y1;
    numBatchLineVertices += 2;

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Renderer::submitQuad(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
{
    quadBatchVAO->bind();
    Buffer *buffer = quadBatchVAO->getBuffer(0);
    buffer->bind();
    if (!buffer)
    {
        std::cout << "[ERROR] Could not submit quad to renderer. Buffer was NULL!" << std::endl;
        return;
    }
    quadData = (QuadData *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY) + numBatchQuadVertices;
    quadData->pos.x = x0;
    quadData->pos.y = y0;
    quadData->color = Vec4(0, 1, 0, 1);
    quadData++;
    quadData->pos.x = x1;
    quadData->pos.y = y1;
    quadData->color = Vec4(0, 1, 0, 1);
    quadData++;
    quadData->pos.x = x2;
    quadData->pos.y = y2;
    quadData->color = Vec4(0, 1, 0, 1);
    quadData++;
    quadData->pos.x = x3;
    quadData->pos.y = y3;
    quadData->color = Vec4(0, 1, 0, 1);
    numBatchQuadVertices += 4;

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Renderer::flush()
{
    static Mat3 ident = Mat3::identity();
    // TODO: DRY cleaning
    lineBatchVAO->bind();
    g_squareLineIBO->bind();
    lineShader->bind();
    lineShader->setUniformMat3("u_Model", ident);
    lineShader->setUniformMat3("u_View", ident);
    GLCALL(glDrawArrays(GL_LINES, 0, numBatchLineVertices));
    numBatchLineVertices = 0;

    quadBatchVAO->bind();
    g_squareFillIBO->bind();
    quadShader->bind();
    quadShader->setUniformMat3("u_Model", ident);
    quadShader->setUniformMat3("u_View", ident);
    GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
    numBatchQuadVertices = 0;
}