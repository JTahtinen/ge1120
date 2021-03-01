#include "renderer.h"
#include "../defs.h"
#include "texture.h"
#include "shader.h"
#include "../math/math.h"
#include <iostream>
#include "../globals.h"
#include "font.h"
#include "renderdefs.h"

Renderer::Renderer()
{
    viewMatrix = Mat3::identity();

    
    // LINE BATCH INIT
    initBatch(&lineBatch.batch);
    
    lineBatch.batch.vao->bind();
    
    unsigned int *lineIndices = new unsigned int[LINE_INDICES_SIZE];
    unsigned int offset = 0;
    for (int i = 0; i < LINE_INDICES_SIZE; i += 2)
    {
        lineIndices[i + 0] = 0 + offset;
        lineIndices[i + 1] = 1 + offset;
        offset += 2;
    }
    lineBatch.batch.ibo->bind();
    lineBatch.batch.ibo->setData(lineIndices, LINE_INDICES_SIZE);
    delete[] lineIndices;
    Buffer *lineVBO = new Buffer();
    lineVBO->setData(NULL, sizeof(float) * MAX_RENDERABLES, GL_DYNAMIC_DRAW);
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
    lineBatch.batch.vao->addBuffer(lineVBO);

    // QUAD BATCH INIT
    initBatch(&quadBatch.batch);
    
    quadBatch.batch.vao->bind();
    
    unsigned int *quadIndices = new unsigned int[QUAD_INDICES_SIZE];
    offset = 0;
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
    quadBatch.batch.ibo->bind();
    quadBatch.batch.ibo->setData(quadIndices, QUAD_INDICES_SIZE);
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
    quadBatch.batch.vao->addBuffer(quadVBO);

    //LETTER BATCH INIT

    initBatch(&letterBatch.batch);
    letterBatch.batch.vao->bind();
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
    letterBatch.batch.ibo->bind();
    letterBatch.batch.ibo->setData(letterIndices, LETTER_INDICES_SIZE);
    delete[] letterIndices;
    Buffer *letterVBO = new Buffer();
    letterVBO->setData(NULL, LETTER_BUFFER_SIZE, GL_DYNAMIC_DRAW);
    BufferLayout letterLayout;
    letterLayout.addLayoutElement(GL_FLOAT, 2);
    letterLayout.addLayoutElement(GL_FLOAT, 2);
    letterVBO->setLayout(&letterLayout);
    letterBatch.batch.vao->addBuffer(letterVBO);
}

Renderer::~Renderer()
{
    delete lineBatch.batch.vao;
    delete lineBatch.batch.ibo;
    delete quadBatch.batch.vao;
    delete quadBatch.batch.ibo;
    delete letterBatch.batch.vao;
    delete letterBatch.batch.ibo;
}
/*
void Renderer::renderVAO(VertexArray *vao, const Texture *texture, const Mat3 &model, const Mat3 &view, RenderType renderType)
{
    if (!shader)
    {
        err("Could not render vao %d - Shader was NULL!\n", vao->id);
        return;
    }
    shader->bind();
    if (!texture)
    {
        err("Could not render vao %d - Texture was NULL!\n", vao->id);
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
*/
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

void Renderer::submitSprite(const Sprite* sprite, Mat3 model, Mat3 view)
{
    if (!sprite)
    {
        err("Could not submit sprite - pointer was NULL!\n");
        return;
    }
    SpriteBatch* spriteBatch = NULL; 
    for (int i = 0; i < spriteBatches.size(); ++i)
    {
        if (spriteBatches[i].reference->id == sprite->texture->id)
        {
            spriteBatches[i].renderables.push_back({sprite, model, view});
            return;
        }
    }
     
    spriteBatches.push_back({});
    spriteBatch = &spriteBatches[spriteBatches.size() - 1];
    initSpriteBatch(spriteBatch, sprite->texture);
    spriteBatch->renderables.push_back({sprite, model, view});
}

void Renderer::submitLine(float x0, float y0, float x1, float y1, Vec2 offset)
{
    lineBatch.batch.vao->bind();
    Buffer *buffer = lineBatch.batch.vao->getBuffer(0);
    buffer->bind();
    Mat3 model = Mat3::translation(offset);
    if (!buffer)
    {
        std::cout << "[ERROR] Could not submit line to renderer. Buffer was NULL!" << std::endl;
        return;
    }
    LineData** lineData = &lineBatch.dataLayout;
    *lineData = (LineData *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY) + lineBatch.batch.numVertices;
    (*lineData)->pos.x = x0;
    (*lineData)->pos.y = y0;
    for (int i = 0; i < 3; ++i)
    {
        (*lineData)->model[i] = model.rows[i];
        (*lineData)->view[i] = viewMatrix.rows[i];
    }
    (*lineData)++;
    (*lineData)->pos.x = x1;
    (*lineData)->pos.y = y1;
    for (int i = 0; i < 3; ++i)
    {
        (*lineData)->model[i] = model.rows[i];
        (*lineData)->view[i] = viewMatrix.rows[i];
    }
    lineBatch.batch.numVertices += 2;

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Renderer::submitLine(Vec2 point0, Vec2 point1, Vec2 offset)
{
    submitLine(point0.x, point0.y, point1.x, point1.y, offset);
}

void Renderer::submitQuad(float x0, float y0,
                          float x1, float y1,
                          float x2, float y2,
                          float x3, float y3,
                          Vec2 position, Vec4 color)
{
    quadBatch.batch.vao->bind();
    Buffer *buffer = quadBatch.batch.vao->getBuffer(0);
    buffer->bind();
    Mat3 model = Mat3::translation(position);
    if (!buffer)
    {
        std::cout << "[ERROR] Could not submit quad to renderer. Buffer was NULL!" << std::endl;
        return;
    }
    QuadData** quadData = &(QuadData*)quadBatch.dataLayout;
    (*quadData) = (QuadData *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY) + quadBatch.batch.numVertices;
    (*quadData)->pos.x = x0;
    (*quadData)->pos.y = y0;
    for (int i = 0; i < 3; ++i)
    {
        (*quadData)->model[i] = model.rows[i];
        (*quadData)->view[i] = viewMatrix.rows[i];
    }
    (*quadData)->color = color;
    (*quadData)++;
    (*quadData)->pos.x = x1;
    (*quadData)->pos.y = y1;
    for (int i = 0; i < 3; ++i)
    {
        (*quadData)->model[i] = model.rows[i];
        (*quadData)->view[i] = viewMatrix.rows[i];
    }
    (*quadData)->color = color;
    (*quadData)++;
    (*quadData)->pos.x = x2;
    (*quadData)->pos.y = y2;
    for (int i = 0; i < 3; ++i)
    {
        (*quadData)->model[i] = model.rows[i];
        (*quadData)->view[i] = viewMatrix.rows[i];
    }
    (*quadData)->color = color;
    (*quadData)++;
    (*quadData)->pos.x = x3;
    (*quadData)->pos.y = y3;
    for (int i = 0; i < 3; ++i)
    {
        (*quadData)->model[i] = model.rows[i];
        (*quadData)->view[i] = viewMatrix.rows[i];
    }
    (*quadData)->color = color;
    quadBatch.batch.numVertices += 4;

    quadBatch.batch.numIndices += 6;

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


void Renderer::submitText(const std::string& text, Vec2 pos, float scale)
{
    if (!font)
    {
        message("[ERROR] Could not render text - Font was NULL!\n");
        return;
    }
    letterBatch.batch.vao->bind();
    Buffer* buffer = letterBatch.batch.vao->getBuffer(0);
    buffer->bind();
    LetterData** letterData = &letterBatch.dataLayout;
    *letterData = (LetterData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY) + letterBatch.batch.numVertices;
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
        
        (*letterData)->pos.x = xAdvance + pos.x + xOff * scale;
        (*letterData)->pos.y = pos.y + yOff * scale;        
        (*letterData)->texCoord = Vec2(l->x, l->y + l->height);
        ++(*letterData);
        
        (*letterData)->pos.x = xAdvance + pos.x + xOff * scale;
        (*letterData)->pos.y = pos.y + (l->height + yOff) * scale;        
        (*letterData)->texCoord = Vec2(l->x, l->y);
        ++(*letterData);
        
        (*letterData)->pos.x = xAdvance + pos.x + (l->width + xOff) * scale;
        (*letterData)->pos.y = pos.y + (l->height + yOff) * scale;
        (*letterData)->texCoord = Vec2(l->x + l->width, l->y);
        ++(*letterData);
        
        (*letterData)->pos.x = xAdvance + pos.x + (l->width + xOff) * scale;
        (*letterData)->pos.y = pos.y + yOff * scale;
        (*letterData)->texCoord = Vec2(l->x + l->width, l->y + l->height);
        ++(*letterData);

        xAdvance += l->xAdvance * scale;
        letterBatch.batch.numVertices += 4;
        letterBatch.batch.numIndices += 6;
    }
    
    GLCALL(glUnmapBuffer(GL_ARRAY_BUFFER));
}

void Renderer::setView(Mat3 view)
{
    viewMatrix = view;
}

void Renderer::setFont(Font* font)
{
    this->font = font;
}

void Renderer::flushSprites()
{

    for (unsigned int spriteBatchIndex = 0; spriteBatchIndex < spriteBatches.size(); ++spriteBatchIndex)
    {
        SpriteBatch* spriteBatch = &spriteBatches[spriteBatchIndex];
        spriteBatch->reference->bind();
        spriteBatch->batch.vao->bind();
        spriteBatch->batch.ibo->bind();
        shader->bind();


        Buffer* buffer = spriteBatch->batch.vao->getBuffer(0);
        buffer->bind();
        SpriteData** spriteData = &spriteBatch->dataLayout;

        *spriteData = (SpriteData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        for (unsigned int spriteIndex = 0; spriteIndex < spriteBatch->renderables.size(); ++spriteIndex)
        {
            const SpriteRenderable* renderable = &spriteBatch->renderables[spriteIndex];
            const Sprite* sprite = renderable->sprite;
            Mat3 model = renderable->model;
            Mat3 view = renderable->view;
            Quad dim = sprite->dimensions;
         
            (*spriteData)->pos.x = dim.point0.x;
            (*spriteData)->pos.y = dim.point0.y;        
            (*spriteData)->texCoord = Vec2(0, 1);
            for (int i = 0; i < 3; ++i)
            {
                (*spriteData)->model[i] = model.rows[i];
                (*spriteData)->view[i] = view.rows[i];
            }

            ++(*spriteData);

            (*spriteData)->pos.x = dim.point1.x;
            (*spriteData)->pos.y = dim.point1.y;        
            (*spriteData)->texCoord = Vec2(0, 0);
            for (int i = 0; i < 3; ++i)
            {
                (*spriteData)->model[i] = model.rows[i];
                (*spriteData)->view[i] = view.rows[i];
            }

            ++(*spriteData);

            (*spriteData)->pos.x = dim.point2.x;
            (*spriteData)->pos.y = dim.point2.y;        
            (*spriteData)->texCoord = Vec2(1, 0);
            for (int i = 0; i < 3; ++i)
            {
                (*spriteData)->model[i] = model.rows[i];
                (*spriteData)->view[i] = view.rows[i];
            }

            ++(*spriteData);

            (*spriteData)->pos.x = dim.point3.x;
            (*spriteData)->pos.y = dim.point3.y;        
            (*spriteData)->texCoord = Vec2(1, 1);
            for (int i = 0; i < 3; ++i)
            {
                (*spriteData)->model[i] = model.rows[i];
                (*spriteData)->view[i] = view.rows[i];
            }

            ++(*spriteData);
        
            spriteBatch->batch.numVertices += 4;
            spriteBatch->batch.numIndices += 6;
        }    
        GLCALL(glUnmapBuffer(GL_ARRAY_BUFFER));    
        GLCALL(glDrawElements(GL_TRIANGLES, spriteBatch->batch.numIndices, GL_UNSIGNED_INT, 0));
        spriteBatch->batch.numVertices = 0;
        spriteBatch->batch.numIndices = 0;
        spriteBatch->renderables.clear();
    }
    //spriteBatches.clear();
    
}
    void flushQuads();
    void flushLetters();
    void flushLines();

// Currently the flush method is arranged only for testing purposes

void Renderer::flush()
{
    flushSprites();
    
    lineBatch.batch.vao->bind();
    g_squareLineIBO->bind();
    lineShader->bind();
    GLCALL(glDrawArrays(GL_LINES, 0, lineBatch.batch.numVertices));
    lineBatch.batch.numVertices = 0;

    quadBatch.batch.vao->bind();
    quadBatch.batch.ibo->bind();
    quadShader->bind();
    GLCALL(glDrawElements(GL_TRIANGLES, quadBatch.batch.numIndices, GL_UNSIGNED_INT, 0));
    quadBatch.batch.numVertices = 0;
    quadBatch.batch.numIndices = 0;

    
    letterBatch.batch.vao->bind();
    letterBatch.batch.ibo->bind();
    font->_atlas->bind();
    letterShader->bind();    
    letterShader->setUniformMat3("u_View", Mat3::view(Vec2(), 0, g_aspect));
    GLCALL(glDrawElements(GL_TRIANGLES, letterBatch.batch.numIndices, GL_UNSIGNED_INT, 0));
    letterBatch.batch.numVertices = 0;
    letterBatch.batch.numIndices = 0;
}
