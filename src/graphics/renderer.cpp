#include "renderer.h"
#include "../defs.h"
#include "texture.h"
#include "shader.h"

#include <iostream>

void Renderer::renderVAO(unsigned int vao, const Texture* texture, Vec2 offset)
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
    shader->setUniform2f("u_Offset", offset.x, offset.y);
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