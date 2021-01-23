#include "renderer.h"
#include "../defs.h"
#include "texture.h"
#include "shader.h"
#include "../math/mat3.h"
#include "../math/math.h"
#include <iostream>
#include "../globals.h"

void Renderer::renderVAO(VertexArray* vao, const Texture *texture, const Mat3 &model, const Mat3 &view, RenderType renderType)
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
            GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_squareFillIBO));
            workingShader = shader;
            glRenderFlag = GL_TRIANGLES;
            texture->bind();
            break;
        }
        case RENDER_WIREFRAME:
        {
            numIndices = 10;
            GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_squareLineIBO));
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

void Renderer::renderLine(float x0, float y0, float x1, float y1)
{
}