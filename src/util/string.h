#pragma once
#include "../globals.h"
#include "../defs.h"
#include <memory.h>
#include <cstring>

struct String
{
    char* content {NULL};
    size_t size {0};
    size_t capacity {0};

    inline String()
    {
    }

    inline String(char* content, size_t size, size_t capacity)
        : content(content)
        , size(size)
        , capacity(capacity)
    {
        if (content)
        {
            content[size] = '\0';
        }          
    }
    
    inline String(const char* text)
    {
        size_t stringSize = strlen(text);

        if (stringSize == 0)
        {
            init(20);
        }
        else
        {
            if (stringSize > capacity)
            {
                init(stringSize);
            }
            size = stringSize;
            for (size_t i = 0; i < size; ++i)
            {
                content[i] = text[i];
            }
        }
    }

    inline String(size_t cap)
        : capacity(cap)
    {
        init(cap);
    }

    inline String(const String& other)
    {
        if (other.size > 0)
        {
            size = other.size;
            capacity = other.capacity;
            content = (char*)g_memory.reserve(sizeof(char) * (other.capacity + 1));

            memcpy(content, other.content, sizeof(char) * (size + 1));
        }
        else
        {
            init(20);
        }
    }

    inline void resize(size_t newCapacity)
    {
        //ASSERT(content);
        if (newCapacity == size) return;
        if (newCapacity < size)
        {
            warn("Resized string is smaller than original. Results in loss of data\n");        
        }
        char* newContent = (char*)g_memory.reserve(sizeof(char) * (newCapacity + 1));
        for (size_t i = 0; i < size; ++i)
        {
            newContent[i] = content[i];
        }
        
        newContent[size] = '\0';
        g_memory.release(content);
        content = newContent;
        capacity = newCapacity;
    }

    inline void setContent(char* str)
    {
        size_t length = strlen(str);
        if (capacity < length)
        {
            init(length);
        }
        else
        {
            content[length] = '\0';
        }
        for (size_t i = 0; i < length; ++i)
        {
            content[i] = str[i];
        }
        
    }
    
    inline static String create(const char* text)
    {
        size_t stringSize = strlen(text);

        if (stringSize == 0) return String();

        char* newContent = NULL;
        size_t newCapacity = stringSize;
        size_t newSize = stringSize;
              
        if (newCapacity > 0)
        {
            newContent = (char*)g_memory.reserve(sizeof(char) * (newCapacity + 1));
            for (size_t i = 0; i < newSize; ++i)
            {
                newContent[i] = text[i];
            }
            newContent[newSize] = '\0';
        }
        return String(newContent, newSize, newCapacity);
    }

    inline ~String()
    {
        //static int numDeallocs = 0;
        //numDeallocs++;
        //       message("String deallocation %d\n", numDeallocs); 
        if (content)
        {
            g_memory.release(content);
        }
    }

    inline void init(size_t cap)
    {
        release();
        capacity = cap;
        content = (char*)g_memory.reserve(sizeof(char) * (capacity + 1));
        content[capacity] = '\0';
    }

    inline void release()
    {
        if (content)
        {
            g_memory.release(content);
            content = NULL;
            capacity = 0;
            size = 0;
        }
    }

    inline String& append(const char letter)
    {
        if (capacity == size)
        {
            resize(capacity * 2);
        }
        ++size;
        content[size - 1] = letter;
        content[size] = '\0';
        return *this;
    }

    inline String& append(const char* text)
    {
        size_t appendSize = strlen(text);
        if (appendSize == 0) return *this;
        size_t newSize = size + appendSize;
        if (capacity < newSize)
        {
            char* newContent = (char*)g_memory.reserve(sizeof(char) * newSize);
            for (size_t i = 0; i < size; ++i)
            {
                newContent[i] = content[i];
            }
            g_memory.release(content);
            content = newContent;
            capacity = newSize;
        }
        for (size_t i = 0; i < appendSize; ++i)
        {
            content[size + i] = text[i];
        }
        content[newSize] = '\0';
        size = newSize;
        return *this;
    }

    inline String& append(const String& other)
    {
        return append(other.content);
    }

    inline void clear()
    {
        size = 0;
    }

    char* c_str() const
    {
        return content;
    }

    inline void print() const
    {
        if (content)
        {
            message("%s", c_str());
        }        
    }

    inline void println() const
    {
        if (content)
        {
            message("%s\n", c_str());
        }
    }

    inline String& operator=(const char* text)
    {
        size_t stringSize = strlen(text);;

        char* newContent = NULL;
        size_t newCapacity = stringSize;
        size_t newSize = stringSize;
              
        if (newCapacity > 0)
        {
            newContent = (char*)g_memory.reserve(sizeof(char) * (newCapacity + 1));
            for (size_t i = 0; i < newSize; ++i)
            {
                newContent[i] = text[i];
            }
            newContent[newSize] = '\0';
        }
        if (content)
        {
            g_memory.release(content);
        }
        content = newContent;
        size = newSize;
        capacity = newCapacity;
        return *this;
    }

    inline String& operator=(const String& other)
    {
        if (other.size > 0)
        {
            size = other.size;
            capacity = other.capacity;
            content = (char*)g_memory.reserve(sizeof(char) * (other.capacity + 1));

            memcpy(content, other.content, sizeof(char) * (size + 1));
        }
        else
        {
            init(20);
        }
        return *this;

    }

    inline bool operator==(const String& other) const
    {
        if (size != other.size) return false;
        for (size_t i = 0; i < size; ++i)
        {
            if (content[i] != other.content[i]) return false;
        }
        return true;
    }

    inline bool operator!=(const String& other) const
    {
        bool result = !(*this == other);
        return result;
    }

    inline String operator+(const char* text) const
    {
        size_t appendSize = strlen(text);
        if (size == 0 && appendSize == 0) return {NULL, 0, 0};
        if (appendSize == 0) return String::create(content);
        if (size == 0) return String::create(text);
        size_t newSize = size + appendSize;
        char* newContent = (char*)g_memory.reserve(sizeof(char) * (newSize + 1));
        size_t newCapacity = newSize;

        size_t i;
        for (i = 0; i < size; ++i)
        {
            newContent[i] = content[i];
        }
        for (i = 0; i < appendSize; ++i)
        {
            newContent[size + i] = text[i];
        }

        return String(newContent, newSize, newCapacity);
    }

/*    inline String operator+(const String& other) const
    {
        return *this + other.content;
    }
*/
    inline String& operator+=(const char* text)
    {
        return append(text);
    }
    
    inline char operator[](size_t index) const
    {
        return content[index];
    }

    inline char& operator[](size_t index)
    {
        return content[index];
    }

};

inline String operator+(const String& left, const String& right)
{
    if (left.size + right.size < 1) return String();

    size_t size = left.size + right.size;
    char* content = (char*)g_memory.reserve(sizeof(char) * (size + 1));
    size_t capacity = size;
    size_t i;
    for (i = 0; i < left.size; ++i)
    {
        content[i] = left[i];
    }
    for (i = 0; i < right.size; ++i)
    {
        content[left.size + i] = right[i];
    }
    content[size] = '\0';
    
    return String(content, size, capacity);
}


inline String toString(int val)
{  
    sprintf(stringBuffer, "%d", val);
    //message("%s\n", stringBuffer);
    return String(stringBuffer);
}

inline String toString(unsigned int val)
{  
    sprintf(stringBuffer, "%d", val);
    //message("%s\n", stringBuffer);
    return String(stringBuffer);
}

inline String toString(float val)
{
    sprintf(stringBuffer, "%f", val);
    //message("%s\n", stringBuffer);
    return String(stringBuffer);
}  

inline String toString(double val)
{
    sprintf(stringBuffer, "%f", val);
    //message("%s\n", stringBuffer);
    return String(stringBuffer);
}

inline String toString(size_t val)
{
    return toString((unsigned int)val);
}
