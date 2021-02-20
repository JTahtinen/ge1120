#pragma once
#include "../globals.h"

template <typename T>
class Vector
{
    T *_data;
    size_t _size;
    size_t _capacity;

public:
    inline Vector(size_t capacity)
        : _size(0)
    {
        reserve(capacity);
    }

    inline Vector(const Vector& other)
        : _size(other._size)
        , _capacity(other._capacity)
    {
        if (_capacity > 0)
        {
            _data = (T*)g_memory.reserve(sizeof(T) * _capacity);
            memcpy(_data, other._data, _size);
        }
    }
    
    inline Vector()
        : Vector(50)
    {
    }

    inline ~Vector()
    {
        g_memory.release(_data);
    }

    inline void reserve(size_t amt)
    {
        if (_size == 0)
        {
            _data = (T*)g_memory.reserve(sizeof(T) * amt);
            _capacity = amt;
        }
    }

    inline void double_capacity()
    {
        if (_capacity > 0)
        {
            _capacity *= 2;
            T* newData = (T*)g_memory.reserve(sizeof(T) * _capacity);
            memmove(newData, _data, sizeof(T) * _size);
            g_memory.release(_data);
            _data = newData;
        }
    }

    inline void push_back(T value)
    {
        if (_size == _capacity)
        {
            double_capacity();
        }
        _data[_size++] = value;
    }

    inline void pop_back()
    {
        --_size;
    }

    inline void insert(T value, size_t index)
    {
        if (index > _size)
        {
            return;
        }
        if (index == _size)
        {
            push_back(unit);
            return;
        }
        if (_size == _capacity)
        {
            double_capacity();
        }
            for (size_t i = _size + 1; i > index; --i)
            {
                _data[i] = _data[i - 1];
            }
        _data[index] = value;
        ++_size;
    }

    inline void erase(size_t index, size_t amt)
    {
        if (index == _size - 1 && amt == 1)
        {
            --_size;
            return;
        }
        if (index < _size)
        {
            size_t finalAmt;
            if (index + amt - 1 < _size)
            {
                finalAmt = amt;
            }
            else
            {
                finalAmt = _size - index;
            }
            for (size_t i = index; i < _size - finalAmt; ++i)
            {
                _data[i] = _data[i + finalAmt];
            }
            _size -= finalAmt;
        }
    }

    inline void erase(size_t index)
    {
        erase(index, 1);
    }

    inline size_t size()
    {
        return _size;
    }

    inline size_t capacity()
    {
        return _capacity;
    }

    inline const T& operator[](size_t index) const
    {
        return _data[index];
    }

    inline T& operator[](size_t index)
    {
        return _data[index];
    }
};
