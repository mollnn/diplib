#include "mat3.h"

vec3 &mat3::operator[](int id) { return id == 0 ? x : id == 1 ? y
                                                              : z; }
vec3 mat3::operator[](int id) const { return id == 0 ? x : id == 1 ? y
                                                                   : z; }
vec3 &mat3::row(int id) { return (*this)[id]; }
vec3 mat3::row(int id) const { return (*this)[id]; }
vec3 mat3::col(int id) const { return {x[id], y[id], z[id]}; }
mat3 mat3::operator+(const mat3 &rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
mat3 mat3::operator-(const mat3 &rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
mat3 mat3::operator*(const mat3 &rhs) const { return {
    {this->row(0).dot(rhs.col(0)), this->row(0).dot(rhs.col(1)), this->row(0).dot(rhs.col(2))},
    {this->row(1).dot(rhs.col(0)), this->row(1).dot(rhs.col(1)), this->row(1).dot(rhs.col(2))},
    {this->row(2).dot(rhs.col(0)), this->row(2).dot(rhs.col(1)), this->row(2).dot(rhs.col(2))},
}; }
mat3 &mat3::operator*=(const mat3 &rhs) { return *this = *this * rhs; }
vec3 mat3::operator*(const vec3 &rhs) const
{
    return {this->row(0).dot(rhs), this->row(1).dot(rhs), this->row(2).dot(rhs)};
}
mat3 mat3::operator*(double rhs) const { return {x * rhs, y * rhs, z * rhs}; }
mat3 mat3::operator/(double rhs) const { return {x / rhs, y / rhs, z / rhs}; }
