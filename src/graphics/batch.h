#pragma once
#include "../util/vector.h"
#include "vertexarray.h"
#include "sprite.h"
#include "../defs.h"
#include "../globals.h"
#include "renderdefs.h"
#include "texture.h"
#include "../math/mat3.h"
#include <vector>
struct Batch
{
    VertexArray*  vao;
    IndexBuffer*  ibo;
    unsigned int  numVertices;
    unsigned int  numIndices;
};

struct SpriteRenderable
{
    const Sprite* sprite;
    Mat3 model;
    Mat3 view;
    float layer;
};

struct SpriteBatch
{
    const Texture* reference;
    Vector<SpriteRenderable> renderables;
    Batch batch;
    SpriteData* dataLayout;
};

struct LineBatch
{
    Batch batch;
    Vector<Line> lines;
    LineData* dataLayout;
};

struct QuadBatch
{
    Batch batch;
    QuadData* dataLayout;
};

struct LetterBatch
{
    Batch batch;
    LetterData* dataLayout;
};

inline bool initBatch(Batch* target)
{
    if (!target)
    {
        err("Could not init batch - pointer was NULL!\n");
        return false;
    }

    target->vao = new VertexArray();
    target->ibo = new IndexBuffer();
    target->numVertices = 0;
    target->numIndices = 0;
    return true;
}

inline bool initSpriteBatch(SpriteBatch* target, const Texture* reference)
{
    if (!reference)
    {
        err("Tried to create sprite batch with NULL texture!\n");
        return false;
    }
    
    target->reference = reference;
    target->renderables.init(300);

    initBatch(&target->batch);
    target->batch.vao->bind();
 
    unsigned int *spriteIndices = new unsigned int[SPRITE_INDICES_SIZE];
    unsigned int offset = 0;
    for (int i = 0; i < SPRITE_INDICES_SIZE; i += 6)
    {
        spriteIndices[i + 0] = 0 + offset;
        spriteIndices[i + 1] = 1 + offset;
        spriteIndices[i + 2] = 2 + offset;
        spriteIndices[i + 3] = 2 + offset;
        spriteIndices[i + 4] = 3 + offset;
        spriteIndices[i + 5] = 0 + offset;
        offset += 4;
    }
    target->batch.ibo->bind();
    target->batch.ibo->setData(spriteIndices, SPRITE_INDICES_SIZE);
    delete[] spriteIndices;
    Buffer *spriteVBO = new Buffer();
    spriteVBO->setData(NULL, SPRITE_BUFFER_SIZE, GL_DYNAMIC_DRAW);
    BufferLayout spriteLayout;
    spriteLayout.addLayoutElement(GL_FLOAT, 3);
    spriteLayout.addLayoutElement(GL_FLOAT, 2);
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            spriteLayout.addLayoutElement(GL_FLOAT, 3);
        }
    }
        
    spriteVBO->setLayout(&spriteLayout);
    target->batch.vao->addBuffer(spriteVBO);
    return true;
}
