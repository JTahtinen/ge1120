#pragma once
#include <GL/glew.h>
#include <iostream>
#include <stdarg.h>


inline void message(char* content, ...)
{
    va_list ap;

    va_start(ap, content);
    char* sval;
    int ival;
    double dval;
    char* p;

    for (p = content; *p; ++p)
    {
        if (*p != '%')
        {
            putchar(*p);
            continue;
        }
        switch (*++p)
        {
            case 'd':
                ival = va_arg(ap, int);
                printf("%d", ival);
                break;
            case 'f':
                dval = va_arg(ap, double);
                printf("%f", dval);
                break;
            case 's':
                for (sval = va_arg(ap, char*); *sval; ++sval)
                {
                    putchar(*sval);
                }
                break;
            default:
                putchar(*p);
                break;
        }
    }
    va_end(ap);
}

inline void err(char* content, ...)
{
    va_list ap;

    va_start(ap, content);
    char* sval;
    int ival;
    double dval;
    char* p;
    printf("[ERROR] ");
    for (p = content; *p; ++p)
    {
        if (*p != '%')
        {
            putchar(*p);
            continue;
        }
        switch (*++p)
        {
            case 'd':
                ival = va_arg(ap, int);
                printf("%d", ival);
                break;
            case 'f':
                dval = va_arg(ap, double);
                printf("%f", dval);
                break;
            case 's':
                for (sval = va_arg(ap, char*); *sval; ++sval)
                {
                    putchar(*sval);                 
                }
                break;
            default:
                putchar(*p);
                break;
        }
    }
    va_end(ap);
}


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
