#pragma once
#include <GL/glew.h>
#include <iostream>

inline void GLClearError()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}

inline bool GLLogCall()
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL ERROR] (" << error << ")" << std::endl;
        return false;
    }
    return true;
}

#ifdef DEBUG
#define ASSERT(expr)    \
    if (expr)           \
    {                   \
    }                   \
    else                \
    {                   \
        __debugbreak(); \
    }
#define GLCALL(expr) \
    GLClearError();  \
    expr;            \
    ASSERT(GLLogCall())

#else

#define ASSERT(expr)
#define GLCALL(expr) expr

#endif