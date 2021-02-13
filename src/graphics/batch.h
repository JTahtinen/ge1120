#pragma once
#include "../util/vector.h"
#include "vertexarray.h"

template <typename T>
struct Batch
{
    VertexArray*  vao;
    IndexBuffer*  ibo;
    Vector<T>     data;
    unsigned int  numVertices;
    unsigned int  numIndices;
    void*         dataLayout;

    Batch()
        : vao(NULL)
        , ibo(NULL)
        , numVertices(0)
        , numIndices(0)
        , dataLayout(NULL)
    {
    }
    
    inline void push(T elem)
    {
        data.push_back(elem);
    }
};
