#ifndef VEC3_H
#define VEC3_H

#include <cmath>

struct vec3;
struct mat3;

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

#endif // VEC3_H
