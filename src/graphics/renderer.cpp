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
        std::cout << "[ERROR] Could not render vao " << vao << ". Shader was null!" << std::endl;
        return;
    }
    if (!texture)
    {
        std::cout << "[ERROR] Could not render vao " << vao << ". Texture was null!" << std::endl;
        return;
    }
    GLCALL(glBindVertexArray(vao));
    shader->setUniformMat3("u_Model", model);
    shader->setUniformMat3("u_View", view);
    texture->bind();
    GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
}

void Renderer::bind() const
{
    if (!shader)
    {
        std::cout << "[ERROR] Could not bind Renderer. Shader was null!" << std::endl;
        return;
    }

    shader->bind();
}