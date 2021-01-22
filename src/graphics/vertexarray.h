#pragma once
#include <GL/glew.h>

struct BufferLayoutElement
{
    GLenum type;
    int count;
};

class BufferLayout
{
    public:
    BufferLayoutElement* layoutElements;
    unsigned int numLayoutElements;
    int stride;
    public:
    BufferLayout();
    ~BufferLayout();
    void addLayoutElement(GLenum type, int count);
};

class Buffer
{
    public:
    unsigned int id;
    public:
    Buffer();
    ~Buffer();
    void setData(const void* data, size_t size);
    void setLayout(const BufferLayout* layout);
    void bind();
};


class VertexArray
{   public:
    unsigned int id;
    Buffer** buffers;
    unsigned int numBuffers;
    public:
    VertexArray();
    ~VertexArray();
    void addBuffer(Buffer* buffer);
    void bind();
};
