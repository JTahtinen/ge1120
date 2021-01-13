#include "mat3.h"
#include <math.h>


#define get(x, y) units[x + y * 3]

Vec2 Mat3::mul(const Vec2 vec) const
{
    Vec2 result(vec.x * units[0] + vec.y * units[1] + 1.0f * units[2],
                vec.x * units[3] + vec.y * units[4] + 1.0f * units[5]);
    return result;
}

Mat3 Mat3::mul(const Mat3& other) const
{
    Mat3 result;
    for (int row = 0; row < 3; ++row)
    {
        for (int column = 0; column < 3; ++column)
        {
            result.units[column + row * 3] = 
            get(0, row) * other.get(column, 0) + 
            get(1, row) * other.get(column, 1) + 
            get(2, row) * other.get(column, 2);
        }
    }
    return result;
   /*Mat3 result = {
                get(0,0) * other.get(0,0) + get(1,0) * other.get(0,1) + get(2,0) * other.get(0,2),
				get(0,0) * other.get(1,0) + get(1,0) * other.get(1,1) + get(2,0) * other.get(1,2),
				get(0,0) * other.get(2,0) + get(1,0) * other.get(2,1) + get(2,0) * other.get(2,2),
				

				// Second row
				get(0,1) * other.get(0,0) + get(1,1) * other.get(0,1) + get(2,1) * other.get(0,2),
				get(0,1) * other.get(1,0) + get(1,1) * other.get(1,1) + get(2,1) * other.get(1,2),
				get(0,1) * other.get(2,0) + get(1,1) * other.get(2,1) + get(2,1) * other.get(2,2),
				

				// Third row
				get(0,2) * other.get(0,0) + get(1,2) * other.get(0,1) + get(2,2) * other.get(0,2),
				get(0,2) * other.get(1,0) + get(1,2) * other.get(1,1) + get(2,2) * other.get(1,2),
				get(0,2) * other.get(2,0) + get(1,2) * other.get(2,1) + get(2,2) * other.get(2,2),

                };			
    return result;*/
}

Mat3 Mat3::identity()
{
    Mat3 result =   {1.0f, 0, 0,
                    0, 1.0f, 0,
                    0, 0, 1.0f};
    return result;
}

Mat3 Mat3::rotation(float angle)
{
    Mat3 result = {cosf(angle), -sinf(angle), 0,
                    sinf(angle), cosf(angle), 0,
                    0, 0, 1.0f};
    return result;
}

Mat3 Mat3::translation(Vec2 vec)
{  
    Mat3 result = {1.0f, 0, vec.x,
                   0, 1.0f, vec.y,
                   0, 0, 1.0f};
    return result;

}

Mat3 Mat3::operator*(const Mat3& other) const
{
    return this->mul(other);
}

Vec2 Mat3::operator*(const Vec2 vec) const
{
    return this->mul(vec);
}