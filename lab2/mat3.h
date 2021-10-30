#ifndef MAT3_H
#define MAT3_H

#include <cmath>
#include "vec3.h"

// Matrix 3x3
struct mat3
{
    vec3 x, y, z;
    vec3 &operator[](int id);
    vec3 operator[](int id) const;
    vec3 &row(int id);
    vec3 row(int id) const;
    vec3 col(int id) const;
    mat3 operator+(const mat3 &rhs) const;
    mat3 operator-(const mat3 &rhs) const;
    mat3 operator*(const mat3 &rhs) const;
    mat3 &operator*=(const mat3 &rhs);
    vec3 operator*(const vec3 &rhs) const;
    mat3 operator*(double rhs) const;
    mat3 operator/(double rhs) const;
};


#endif // MAT3_H
