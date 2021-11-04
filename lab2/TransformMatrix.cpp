#include "TransformMatrix.h"

TransformMatrix::TransformMatrix() : m_({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}) {}
TransformMatrix::TransformMatrix(float x11, float x12, float x13, float x21, float x22, float x23) : m_({{x11, x12, x13}, {x21, x22, x23}, {0, 0, 1}}) {}
TransformMatrix::TransformMatrix(float x11, float x12, float x21, float x22) : m_({{x11, x12, 0}, {x21, x22, 0}, {0, 0, 1}}) {}
TransformMatrix::TransformMatrix(const TransformMatrix &rhs) : m_(rhs.m_) {}
TransformMatrix::TransformMatrix(const mat3 &rhs) : m_(rhs) {}

TransformMatrix &TransformMatrix::operator=(const TransformMatrix &rhs)
{
    m_ = rhs.m_;
    return *this;
}
TransformMatrix &TransformMatrix::operator=(const mat3 &rhs)
{
    m_ = rhs;
    return *this;
}

mat3 TransformMatrix::to_mat3()
{
    return m_;
}

float TransformMatrix::item(int i, int j)
{
    return m_[i][j];
}

void TransformMatrix::setItem(int i, int j, float v)
{
    m_[i][j] = v;
}

TransformMatrix TransformMatrix::operator*(const TransformMatrix &rhs)
{
    return TransformMatrix(m_ * rhs.m_);
}

TransformMatrix &TransformMatrix::operator*=(const TransformMatrix &rhs)
{
    this->m_ *= rhs.m_;
    return *this;
}

void TransformMatrix::translate(float dx, float dy)
{
    this->m_ = mat3(1.0, 0.0, dx, 0.0, 1.0, dy, 0.0, 0.0, 1.0) * (this->m_);
}

void TransformMatrix::rotate(float angle)
{
    this->m_ = mat3(cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0, 0, 0, 1) * this->m_;
}

void TransformMatrix::scale(float scalar)
{
    this->m_ = mat3(scalar, 0, 0, 0, scalar, 0, 0, 0, 1) * this->m_;
}
