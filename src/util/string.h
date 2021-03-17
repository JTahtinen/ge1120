#pragma once
#include "../globals.h"
#include "../defs.h"
#include <memory.h>
#include <cstring>

struct String
{
    char* content;
    size_t size;
    size_t capacity;



    inline String(char* content, size_t size, size_t capacity)
        : content(content)
        , size(size)
        , capacity(capacity)
    {
    }

    inline String()
        : String(NULL, 0, 0)
    {
    }
    
    inline String(const char* text)
        : content(NULL)
    {
        size_t stringSize = strlen(text);
        
        if (stringSize > 0)
        {
            init(0, stringSize);               
            append(text);
        }
        else
        {
            size = 0;
            capacity = 0;
        }
    }

    inline String(size_t cap)
        : size(0)
        , content(NULL)
    {
        
        if (cap > 0)
        {
            init(0, cap);
        }
        else
        {        
            capacity = 0;        
        }
    }

    inline String(const String& other)
        : content(NULL)
    {
        if (other.content && other.size > 0)
        {
            init(other.size, other.capacity);
            
            memcpy(content, other.content, sizeof(char) * size);
        }
        else
        {
            size = 0;
            capacity = 0;
            content = NULL;
        }
    }

    inline ~String()
    {
        //static int numDeallocs = 0;
        //numDeallocs++;
        //       message("String deallocation %d\n", numDeallocs); 
        release();
    }

    
    inline void init(size_t sz, size_t cap)
    {
        release();
        if (cap < sz)
        {
            size = cap;
        }
        else
        {
            size = sz;
        }
        
        capacity = cap;
        content = (char*)g_memory.reserve(sizeof(char) * (capacity + 1));
        content[size] = '\0';
    }

    inline void init(size_t cap)
    {
        init(0, cap);
    }

    inline void resize(size_t newCapacity)
    {
        //ASSERT(content);
        if (!content)
        {
            init(0, newCapacity);
            return;
        }
        if (newCapacity == capacity) return;
        if (newCapacity < size)
        {
            warn("Resized string is smaller than original. Results in loss of data\n");
            size = newCapacity;
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

    inline void setContent(const char* str)
    {
        size_t length = strlen(str);
        if (!content || capacity < length)
        {
            init(length, length);
        }
        
        for (size_t i = 0; i < size; ++i)
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
              
        
        newContent = (char*)g_memory.reserve(sizeof(char) * (newCapacity + 1));
        for (size_t i = 0; i < newSize; ++i)
        {
            newContent[i] = text[i];
        }
        newContent[newSize] = '\0';
        
        return String(newContent, newSize, newCapacity);
    }


    inline void release()
    {
        if (content)
        {
//            message("Releasing string\n");
            g_memory.release(content);
            content = NULL;
        }
        capacity = 0;
        size = 0;
    }

    inline String& append(const char letter)
    {
        if (!content)
        {
            init(0, 20);
        }
        else if (capacity == size)
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
        size_t oldSize = size;
        if (capacity < newSize)
        {
            resize(newSize);
        }    
        for (size_t i = 0; i < appendSize; ++i)
        {
            content[oldSize + i] = text[i];
        }
        size = newSize;
            
        content[size] = '\0';
        return *this;
    }

    inline String& append(const String& other)
    {
        return append(other.content);
    }

    inline void clear()
    {
        size = 0;
        if (content)
        {
            content[0] = '\0';
        }
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
        /*
        size_t stringSize = strlen(text);

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
        return *this;*/
        setContent(text);
        return *this;
    }

    inline String& operator=(const String& other)
    {
        /*
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
            }*/
        if (other.content)
        {
            setContent(other.content);
        }
        return *this;

    }

    inline bool operator==(const char* other) const
    {
        if (other[size] != '\0') return false;
        for (size_t i = 0; i < size; ++i)
        {
            if (content[i] != other[i]) return false;
        }
        return true;
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
        newContent[newSize] = '\0';

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
    if (!left.content || !right.content) return String();
    if (left.size + right.size == 0) return String();

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
