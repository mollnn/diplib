#ifndef MAT3_H
#define MAT3_H

#include <cmath>
#include "vec3.h"

// Matrix 3x3
struct mat3
{
    vec3 x, y, z;
    mat3();
    mat3(vec3 a, vec3 b,vec3 c);
    mat3(float ax, float ay, float az, float bx, float by, float bz,float cx,float cy,float cz);
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
    mat3 operator*(float rhs) const;
    mat3 operator/(float rhs) const;
    mat3& operator = (const mat3 &rhs);
    mat3 cominor() const;
    mat3 crossneg() const;
    mat3 alcominor() const;
    mat3 adjugate() const;
    float det() const;
    mat3 inverse() const;
    mat3 transpose() const;
};


#endif // MAT3_H
