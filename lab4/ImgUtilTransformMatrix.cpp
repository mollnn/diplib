#include "ImgUtilTransformMatrix.h"

ImgUtilTransformMatrix::ImgUtilTransformMatrix() : m_({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}) {}
ImgUtilTransformMatrix::ImgUtilTransformMatrix(float x11, float x12, float x13, float x21, float x22, float x23) : m_({{x11, x12, x13}, {x21, x22, x23}, {0, 0, 1}}) {}
ImgUtilTransformMatrix::ImgUtilTransformMatrix(float x11, float x12, float x21, float x22) : m_({{x11, x12, 0}, {x21, x22, 0}, {0, 0, 1}}) {}
ImgUtilTransformMatrix::ImgUtilTransformMatrix(const ImgUtilTransformMatrix &rhs) : m_(rhs.m_) {}
ImgUtilTransformMatrix::ImgUtilTransformMatrix(const mat3 &rhs) : m_(rhs) {}

ImgUtilTransformMatrix &ImgUtilTransformMatrix::operator=(const ImgUtilTransformMatrix &rhs)
{
    m_ = rhs.m_;
    return *this;
}
ImgUtilTransformMatrix &ImgUtilTransformMatrix::operator=(const mat3 &rhs)
{
    m_ = rhs;
    return *this;
}

mat3 ImgUtilTransformMatrix::to_mat3()
{
    return m_;
}

float ImgUtilTransformMatrix::item(int i, int j)
{
    return m_[i][j];
}

void ImgUtilTransformMatrix::setItem(int i, int j, float v)
{
    m_[i][j] = v;
}

ImgUtilTransformMatrix ImgUtilTransformMatrix::operator*(const ImgUtilTransformMatrix &rhs)
{
    return ImgUtilTransformMatrix(m_ * rhs.m_);
}

ImgUtilTransformMatrix &ImgUtilTransformMatrix::operator*=(const ImgUtilTransformMatrix &rhs)
{
    this->m_ *= rhs.m_;
    return *this;
}

void ImgUtilTransformMatrix::translate(float dx, float dy)
{
    this->m_ = mat3(1.0, 0.0, dx, 0.0, 1.0, dy, 0.0, 0.0, 1.0) * (this->m_);
}

void ImgUtilTransformMatrix::rotate(float angle)
{
    this->m_ = mat3(cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0, 0, 0, 1) * this->m_;
}

void ImgUtilTransformMatrix::scale(float scalar)
{
    this->m_ = mat3(scalar, 0, 0, 0, scalar, 0, 0, 0, 1) * this->m_;
}
