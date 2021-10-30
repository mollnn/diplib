#ifndef VEC3_H
#define VEC3_H

#include <cmath>

// Vector 3D
struct vec3
{
    double x, y, z;
    double &operator[](int id);
    double operator[](int id) const;
    vec3 operator+(const vec3 &rhs) const;
    vec3 operator-(const vec3 &rhs) const;
    vec3 operator*(const vec3 &rhs) const;
    vec3 operator*(double rhs) const;
    vec3 operator/(double rhs) const;
    double dot(const vec3 &rhs) const;
    vec3 cross(const vec3 &rhs) const;
    double norm2() const;
    double norm() const;
    vec3 unit() const;
    double avg() const;
};
vec3 operator*(double lhs, const vec3 &rhs);

#endif // VEC3_H
