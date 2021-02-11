#pragma once

template <typename T>
class Vector
{
    T *_data;
    size_t _size;
    size_t _capacity;

public:
    inline Vector(size_t capacity)
        : _size(0), _capacity(0)
    {
        reserve(capacity);
    }

    inline Vector(const Vector& other)
        : _size(other._size)
        , _capacity(other._capacity)
    {
        if (_capacity > 0)
        {
            _data = new T[_capacity];
            for (int i = 0; i < _size; ++i)
            {
                _data[i] = other[i];
            }
        }
    }
    
    inline Vector()
        : Vector(10)
    {
    }

    inline ~Vector()
    {
        delete[] _data;
    }

    inline void reserve(size_t amt)
    {
        if (_size == 0)
        {
            _data = new T[amt];
            _capacity = amt;
        }
    }

    inline void double_capacity()
    {
        if (_capacity > 0)
        {
            _capacity *= 2;
            T* newData = new T[_capacity];
            for (int i = 0; i < _size; ++i)
            {
                newData[i] = _data[i];
            }
            delete[] _data;
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

    inline void insert(T value, unsigned int index)
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
            for (int i = _size + 1; i > index; --i)
            {
                _data[i] = _data[i - 1];
            }
        _data[index] = value;
        ++_size;
    }

    inline void erase(unsigned int index, unsigned int amt)
    {
        if (index == _size - 1 && amt == 1)
        {
            --_size;
            return;
        }
        if (index < _size)
        {
            unsigned int finalAmt;
            if (index + amt - 1 < _size)
            {
                finalAmt = amt;
            }
            else
            {
                finalAmt = _size - index;
            }
            for (int i = index; i < _size - finalAmt; ++i)
            {
                _data[i] = _data[i + finalAmt];
            }
            _size -= finalAmt;
        }
    }

    inline void erase(unsigned int index)
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

    const T& operator[](unsigned int index) const
    {
        return _data[index];
    }

    T& operator[](unsigned int index)
    {
        return _data[index];
    }
};
