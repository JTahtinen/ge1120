#pragma once
#include <GL/glew.h>
#include <stdarg.h>
#include <windows.h>
#include <stdio.h>

#define VA_ARGS_IMPLEMENTATION \
    va_list ap; \
    va_start(ap, content); \
    char* sval; \
    int ival; \
    double dval; \
    char* p; \
    for (p = content; *p; ++p) \
    { \
        if (*p != '%') \
        { \
            putchar(*p); \
            continue; \
        } \
        switch (*++p) \
        { \
            case 'd': \
                ival = va_arg(ap, int); \
                printf("%d", ival); \
                break; \
            case 'f': \
                dval = va_arg(ap, double); \
                printf("%f", dval); \
                break; \
            case 's': \
                for (sval = va_arg(ap, char*); *sval; ++sval) \
                { \
                    putchar(*sval); \
                } \
                break; \
            default: \
                putchar(*p); \
                break; \
        } \
    } \
    va_end(ap);

inline void message(char* content, ...)
{
    VA_ARGS_IMPLEMENTATION;
}

inline void err(char* content, ...)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole,
                            FOREGROUND_RED);  
    printf("[ERROR] ");
  
    SetConsoleTextAttribute(hConsole, 
                            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    VA_ARGS_IMPLEMENTATION;
}

inline void warn(char* content, ...)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole,
                            FOREGROUND_RED | FOREGROUND_GREEN);  
    printf("[WARNING] ");
    SetConsoleTextAttribute(hConsole, 
                            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    VA_ARGS_IMPLEMENTATION;
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
        message("[OpenGL ERROR] (%d)\n", error);
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
