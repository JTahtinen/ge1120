#include "renderer.h"
#include "../defs.h"
#include "texture.h"
#include "shader.h"
#include "../math/mat3.h"
#include "../math/math.h"
#include <iostream>

void Renderer::renderVAO(unsigned int vao, const Texture* texture, const Mat3& model, const Mat3& view)
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
    GLCALL(glBindVertexArray(vao));
    shader->setUniformMat3("u_Model", model);
    shader->setUniformMat3("u_View", view);
    texture->bind();
    GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
}

void Renderer::renderLine(VertexArray* vao)
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