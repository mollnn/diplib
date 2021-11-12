#ifndef VEC3_H
#define VEC3_H

#include <cmath>

// Vector 3D
struct vec3
{
    float x, y, z;
    vec3();
    vec3(float a, float b, float c);
    float &operator[](int id);
    float operator[](int id) const;
    vec3 operator+(const vec3 &rhs) const;
    vec3 operator-(const vec3 &rhs) const;
    vec3 operator*(const vec3 &rhs) const;
    vec3 operator*(float rhs) const;
    vec3 operator/(float rhs) const;
    vec3 &operator=(const vec3 &rhs);
    float dot(const vec3 &rhs) const;
    vec3 cross(const vec3 &rhs) const;
    float norm2() const;
    float norm() const;
    vec3 unit() const;
    float avg() const;
};
vec3 operator*(float lhs, const vec3 &rhs);

#endif // VEC3_H
