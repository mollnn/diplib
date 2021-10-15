#include "vec3.h"

double& vec3::operator[](int id) {return id==0 ? x: id==1?y:z;}
double vec3::operator[](int id) const {return id==0 ? x: id==1?y:z;}
vec3 vec3::operator+(const vec3 &rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
vec3 vec3::operator-(const vec3 &rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
vec3 vec3::operator*(const vec3 &rhs) const { return {x * rhs.x, y * rhs.y, z * rhs.z}; }
vec3 vec3::operator*(double rhs) const { return {x * rhs, y * rhs, z * rhs}; }
vec3 vec3::operator/(double rhs) const { return {x / rhs, y / rhs, z / rhs}; }
double vec3::dot(const vec3 &rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
vec3 vec3::cross(const vec3 &rhs) const { return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x}; }
double vec3::norm2() const { return x * x + y * y + z * z; }
double vec3::norm() const { return sqrt(norm2()); }
vec3 vec3::unit() const { return (*this) / norm(); }
double vec3::avg() const { return (x + y + z) / 3; }

vec3 operator*(double lhs, const vec3 &rhs) { return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z}; }


vec3& mat3::operator[](int id) {return id==0 ? x: id==1?y:z;}
vec3 mat3::operator[](int id) const {return id==0 ? x: id==1?y:z;}
vec3& mat3::row(int id) {return (*this)[id];}
vec3 mat3::row(int id) const {return (*this)[id];}
vec3 mat3::col(int id) const {return {x[id],y[id],z[id]};}
mat3 mat3::operator+(const mat3 &rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
mat3 mat3::operator-(const mat3 &rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
mat3 mat3::operator*(const mat3 &rhs) const { return {{this->row(0).dot(rhs.col(0)), this->row(0).dot(rhs.col(1)), this->row(0).dot(rhs.col(2))},
        {this->row(1).dot(rhs.col(0)), this->row(1).dot(rhs.col(1)), this->row(1).dot(rhs.col(2))},
        {this->row(2).dot(rhs.col(0)), this->row(2).dot(rhs.col(1)), this->row(2).dot(rhs.col(2))},
    }; }
mat3& mat3::operator *= (const mat3& rhs) {return *this = *this * rhs;}
vec3 mat3::operator*(const vec3 &rhs) const {
    return {this->row(0).dot(rhs), this->row(1).dot(rhs), this->row(2).dot(rhs)};
}
mat3 mat3::operator*(double rhs) const { return {x * rhs, y * rhs, z * rhs}; }
mat3 mat3::operator/(double rhs) const { return {x / rhs, y / rhs, z / rhs}; }
