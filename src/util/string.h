#pragma once
#include "../globals.h"

class String
{
    char* _content;
    size_t _size;
    size_t _capacity;
    static char*[5000]

public:
    inline String(const char* content)
    {
        int i = 0;
        for (; content[i] != 0; ++i){}
        if (i > 0)
        {
            _capacity = i;
            _size = i;
            _content = (char*)g_memory.reserve(sizeof(char) * _capacity);
            for (i = 0; i < _size; ;++i)
            {
                _content[i] = content[i];
            }
        }
        else
        {
            init(20);
        }
    }
    
    inline String()
        : _capacity(0)
        , _size(0)
        , _content(NULL)
    {
        init(20);
    }

    inline String(size_t cap)
    {
        init(cap);
    }

    inline ~String()
    {
        if (_capacity > 0)
        {
            g_memory.release(_content);
        }
    }

    inline void init(size_t cap)
    {
        release();
        _capacity = cap;
        _content = (char*)g_memory.reserve(sizeof(char) * _capacity);
    }

    inline void release()
    {
        if (_capacity > 0)
        {
            g_memory.release(_content);
            _content = NULL;
            _capacity = 0;
            _size = 0;
        }
    }

    inline size_t size() const
    {
        return _size;
    }

    inline size_t capacity() const
    {
        return _capacity;
    }

    String& append(const String& other)
    {
        if (other.size == 0) return;
        size_t newSize = _size + other._size;
        if (_capacity < newSize)
        {
            char* newContent = (char*)g_memory.reserve(sizeof(char) * newSize);
            for (size_t i = 0; i < _size; ++i)
            {
                newContent[i] = _content[i];
            }
            _content = newContent;
            _capacity = newSize;
        }
        for (size_t i = 0; i < other._size; ++i)
        {
            _content[_size + i] = other._content[i]
        }
        _size = newSize;
        return this;
    }

    inline String operator=(const char* content)
    {
        return String(content);
    }

    inline String operator+(const char* content)
    {
        //TODO: Stuff
        return String();
    }
};
