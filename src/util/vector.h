#pragma once
#include "../globals.h"
#include "../defs.h"
template <typename T>
struct Vector
{
    T *data;
    size_t size;
    size_t capacity;
/*
    Vector(const Vector<T>& other)
        : _size(other._size)
        , _capacity(other._capacity)
        , _data(NULL)
    {
        if (_capacity > 0)
        {
            _data = (T*)g_memory.reserve(sizeof(T) * _capacity);
            memcpy(_data, other._data, _size * sizeof(T));
        }
    }
*/
    
    inline bool init(size_t cap)
    {

        data = (T*)g_memory.reserve(sizeof(T) * cap);
        if (!data)
        {
            err("Could not reserve memory for vector!\n");
            return false;
        }
        capacity = cap;
        size = 0;
        return true;
    }

    inline ~Vector()
    {
        if (data)
            g_memory.release(data);
    }

    inline void reserve(size_t amt)
    {
        size = 0;
        data = (T*)g_memory.reserve(sizeof(T) * amt);
        capacity = amt;        
    }

    inline void double_capacity()
    {
        if (capacity > 0)
        {
            capacity *= 2;
            T* newData = (T*)g_memory.reserve(sizeof(T) * capacity);
            if (size > 0)
            {
                memmove(newData, data, sizeof(T) * size);
                g_memory.release(data);
            }
            data = newData;
        }
    }

    inline void push_back(T value)
    {
        if (capacity == 0)
        {
            reserve(10);
        }
        else if (size == capacity)
        {
            double_capacity();
        }
        data[size++] = value;
    }

    inline void pop_back()
    {
        if (size > 0)
        {
            --size;
        }
    }

    inline void insert(T value, size_t index)
    {
        if (index > size)
        {
            return;
        }
        if (index == size)
        {
            push_back(unit);
            return;
        }
        if (size == capacity)
        {
            double_capacity();
        }
        for (size_t i = size + 1; i > index; --i)
        {
            data[i] = data[i - 1];
        }
        data[index] = value;
        ++size;
    }

    inline void erase(size_t index, size_t amt)
    {
        if (index == size - 1 && amt == 1)
        {
            --size;
            return;
        }
        if (index < size)
        {
            size_t finalAmt;
            if (index + amt - 1 < size)
            {
                finalAmt = amt;
            }
            else
            {
                finalAmt = size - index;
            }
            for (size_t i = index; i < size - finalAmt; ++i)
            {
                data[i] = data[i + finalAmt];
            }
            size -= finalAmt;
        }
    }

    inline void erase(size_t index)
    {
        erase(index, 1);
    }

    inline void clear()
    {
        size = 0;
    }

    inline T& back()
    {
        return data[size - 1];
    }

    inline const T& operator[](size_t index) const
    {
        ASSERT(index < size);
        return data[index];
    }

    inline T& operator[](size_t index)
    {
        ASSERT(index < size);
        return data[index];
    }
};
