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
    mat3(double ax, double ay, double az, double bx, double by, double bz,double cx,double cy,double cz);
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
    mat3& operator = (const mat3 &rhs);
    mat3 cominor() const;
    mat3 crossneg() const;
    mat3 alcominor() const;
    mat3 adjugate() const;
    double det() const;
    mat3 inverse() const;
    mat3 transpose() const;
};


#endif // MAT3_H
