#include "AffineTransform.h"

AffineTransform::AffineTransform(): m_({{1,0,0},{0,1,0},{0,0,1}}) {}
AffineTransform::AffineTransform(double x11, double x12, double x13, double x21, double x22, double x23): m_({{x11,x12,x13},{x21,x22,x23},{0,0,1}}) {}
AffineTransform::AffineTransform(double x11, double x12, double x21, double x22): m_({{x11,x12,0},{x21,x22,0},{0,0,1}}) {}
AffineTransform::AffineTransform(const AffineTransform& rhs): m_(rhs.m_) {}
AffineTransform::AffineTransform(const mat3& rhs): m_(rhs) {}

AffineTransform& AffineTransform::operator = (const AffineTransform& rhs) {m_=rhs.m_; return *this;}
AffineTransform& AffineTransform::operator = (const mat3& rhs) {m_=rhs; return *this;}

mat3 AffineTransform::to_mat3()
{
    return m_;
}

double AffineTransform::item(int i,int j)
{
    return m_[i][j];
}

void AffineTransform::setItem(int i,int j,double v)
{
    m_[i][j]=v;
}

AffineTransform AffineTransform::operator * (const AffineTransform& rhs)
{
    return AffineTransform(m_*rhs.m_);
}

AffineTransform& AffineTransform::operator *= (const AffineTransform& rhs)
{
    this->m_*=rhs.m_;
    return *this;
}

void AffineTransform::translate(double dx, double dy)
{
    this->m_=mat3(1.0,0.0,dx,0.0,1.0,dy,0.0,0.0,1.0)*(this->m_);
}

void AffineTransform::rotate(double angle)
{
    this->m_=mat3(cos(angle),-sin(angle),0,sin(angle),cos(angle),0,0,0,1)*this->m_;
}

void AffineTransform::scale(double scalar)
{
    this->m_=mat3(scalar,0,0,0,scalar,0,0,0,1)*this->m_;
}
