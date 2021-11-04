#include "mat3.h"

mat3::mat3()
{
}

mat3::mat3(vec3 a, vec3 b, vec3 c) : x(a), y(b), z(c)
{
}

mat3::mat3(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz) : x(ax, ay, az), y(bx, by, bz), z(cx, cy, cz)
{
}

vec3 &mat3::operator[](int id)
{
    return id == 0 ? x : id == 1 ? y
                                 : z;
}

vec3 mat3::operator[](int id) const
{
    return id == 0 ? x : id == 1 ? y
                                 : z;
}

vec3 &mat3::row(int id)
{
    return (*this)[id];
}

vec3 mat3::row(int id) const
{
    return (*this)[id];
}

vec3 mat3::col(int id) const
{
    return {x[id], y[id], z[id]};
}

mat3 mat3::operator+(const mat3 &rhs) const
{
    return {x + rhs.x, y + rhs.y, z + rhs.z};
}

mat3 mat3::operator-(const mat3 &rhs) const
{
    return {x - rhs.x, y - rhs.y, z - rhs.z};
}

mat3 mat3::operator*(const mat3 &rhs) const
{
    return {

        {this->row(0).dot(rhs.col(0)), this->row(0).dot(rhs.col(1)), this->row(0).dot(rhs.col(2))},
        {this->row(1).dot(rhs.col(0)), this->row(1).dot(rhs.col(1)), this->row(1).dot(rhs.col(2))},
        {this->row(2).dot(rhs.col(0)), this->row(2).dot(rhs.col(1)), this->row(2).dot(rhs.col(2))},
    };
}

mat3 &mat3::operator*=(const mat3 &rhs)
{
    return *this = *this * rhs;
}

vec3 mat3::operator*(const vec3 &rhs) const
{
    return {this->row(0).dot(rhs), this->row(1).dot(rhs), this->row(2).dot(rhs)};
}

mat3 mat3::operator*(float rhs) const
{
    return {x * rhs, y * rhs, z * rhs};
}

mat3 mat3::operator/(float rhs) const
{
    return {x / rhs, y / rhs, z / rhs};
}

mat3 &mat3::operator=(const mat3 &rhs)
{
    x = rhs[0];
    y = rhs[1];
    z = rhs[2];
    return *this;
}

mat3 mat3::cominor() const
{
    return {{this->y.y * this->z.z - this->y.z * this->z.y,
             this->y.x * this->z.z - this->y.z * this->z.x,
             this->y.x * this->z.y - this->y.y * this->z.x},
            {this->x.y * this->z.z - this->x.z * this->z.y,
             this->x.x * this->z.z - this->x.z * this->z.x,
             this->x.x * this->z.y - this->x.y * this->z.x},
            {this->x.y * this->y.z - this->x.z * this->y.y,
             this->x.x * this->y.z - this->x.z * this->y.x,
             this->x.x * this->y.y - this->x.y * this->y.x}};
}

mat3 mat3::crossneg() const
{
    return {{this->x.x, -this->x.y, this->x.z},
            {-this->y.x, this->y.y, -this->y.z},
            {this->z.x, -this->z.y, this->z.z}};
}

mat3 mat3::alcominor() const
{
    return cominor().crossneg();
}

mat3 mat3::adjugate() const
{
    return alcominor().transpose();
}

float mat3::det() const
{
    return this->x.x * this->y.y * this->z.z +
           this->x.y * this->y.z * this->z.x +
           this->x.z * this->y.x * this->z.y -
           this->x.x * this->y.z * this->z.y -
           this->x.y * this->y.x * this->z.z -
           this->x.z * this->y.y * this->z.x;
}

mat3 mat3::inverse() const
{
    return adjugate() / (det() + 1e-12);
}

mat3 mat3::transpose() const
{
    return {{this->x.x, this->y.x, this->z.x},
            {this->x.y, this->y.y, this->z.y},
            {this->x.z, this->y.z, this->z.z}};
}

void mat3::to_floats(float *a) const
{
    a[0 + 0] = this->x.x;
    a[0 + 1] = this->x.y;
    a[0 + 2] = this->x.z;
    a[3 + 0] = this->y.x;
    a[3 + 1] = this->y.y;
    a[3 + 2] = this->y.z;
    a[6 + 0] = this->z.x;
    a[6 + 1] = this->z.y;
    a[6 + 2] = this->z.z;
}
