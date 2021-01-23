#pragma once
#include <GL/glew.h>

struct BufferLayoutElement
{
    GLenum type;
    int count;
};

struct BufferLayout
{
    BufferLayoutElement* layoutElements;
    unsigned int numLayoutElements;
    int stride;
    public:
    BufferLayout();
    ~BufferLayout();
    void addLayoutElement(GLenum type, int count);
};

struct Buffer
{
    unsigned int id;
    public:
    Buffer();
    ~Buffer();
    void setData(const void* data, size_t size, GLenum usage = GL_STATIC_DRAW);
    void setLayout(const BufferLayout* layout);
    void bind();
};

struct IndexBuffer
{
    unsigned int id;
    unsigned int numIndices;
    public:
    IndexBuffer();
    void setData(unsigned int* data, int count, GLenum usage = GL_STATIC_DRAW);
    void bind();
};


struct VertexArray
{   
    unsigned int id;
    Buffer** buffers;
    unsigned int numBuffers;
    public:
    VertexArray();
    ~VertexArray();
    void addBuffer(Buffer* buffer);
    Buffer* getBuffer(unsigned int index);
    void bind();
};
