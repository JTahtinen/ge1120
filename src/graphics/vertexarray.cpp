#include "vertexarray.h"
#include "../defs.h"

#define MAX_LAYOUT_ELEMENTS 10

int getGLTypeSizeInBytes(GLenum type)
{
    switch(type)
    {
        case GL_FLOAT:
			return sizeof(GLfloat);
		case GL_DOUBLE:
			return sizeof(GLdouble);
		case GL_INT:
			return sizeof(GLint);
		case GL_UNSIGNED_INT:
			return sizeof(GLuint);
		case GL_BYTE:
			return sizeof(GLbyte);
		case GL_UNSIGNED_BYTE:
			return sizeof(GLubyte);
		case GL_BOOL:
			return sizeof(GLboolean);
		default:
			return 0;
    }
}

BufferLayout::BufferLayout()
    : numLayoutElements(0)
    , stride(0)
{
    layoutElements = new BufferLayoutElement[MAX_LAYOUT_ELEMENTS];
}

void BufferLayout::addLayoutElement(GLenum type, int count)
{
    if (count < 1) return;
    if (numLayoutElements < MAX_LAYOUT_ELEMENTS)
    {
        layoutElements[numLayoutElements] = {type, count};
        stride += getGLTypeSizeInBytes(type) * count; 
        ++numLayoutElements;
    }
}

Buffer::Buffer()
{
    GLCALL(glGenBuffers(1, &this->id));
    bind();   
}

void Buffer::setData(const void* data, size_t size)
{
    GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void Buffer::bind()
{
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id));
}

Buffer::~Buffer()
{
}

void Buffer::setLayout(const BufferLayout* layout)
{
    bind();
    uint64_t pointerToElem = 0;
    for (int i = 0; i < layout->numLayoutElements; ++i)
    {
        const BufferLayoutElement* elem = &layout->layoutElements[i];
        GLCALL(glEnableVertexAttribArray(i));
        GLCALL(glVertexAttribPointer(i, elem->count, elem->type, GL_FALSE, layout->stride, (const void *)(pointerToElem)));
        pointerToElem += elem->count * getGLTypeSizeInBytes(elem->type);
    }   
}

#define MAX_VERTEX_ARRAY_BUFFERS 10

VertexArray::VertexArray()
    : numBuffers(0)
{
    glGenVertexArrays(1, &this->id);
    bind();
    buffers = new Buffer*[MAX_VERTEX_ARRAY_BUFFERS];
}

VertexArray::~VertexArray()
{
    delete[] buffers;
    buffers = nullptr;
}

void VertexArray::addBuffer(Buffer* buffer)
{
    if (buffer && numBuffers < MAX_VERTEX_ARRAY_BUFFERS)
    {
        buffers[numBuffers++] = buffer;
    }
}

void VertexArray::bind()
{
    GLCALL(glBindVertexArray(id));
}