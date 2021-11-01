#ifndef AFFINETRANSFORM_H
#define AFFINETRANSFORM_H

#include "mathheader.h"

class AffineTransform
{
private:
    mat3 m_;
public:
    AffineTransform();
    AffineTransform(double x11, double x12, double x13, double x21, double x22, double x23);
    AffineTransform(double x11, double x12, double x21, double x22);
    AffineTransform(const AffineTransform& rhs);
    AffineTransform(const mat3& rhs);
    AffineTransform& operator = (const AffineTransform& rhs);
    AffineTransform& operator = (const mat3& rhs);
    mat3 to_mat3();
    double item(int i,int j);
    void setItem(int i,int j,double v);
    AffineTransform operator * (const AffineTransform& rhs);
    AffineTransform& operator *= (const AffineTransform& rhs);
    void translate(double dx, double dy);
    void rotate(double angle);
    void scale(double scalar);
};

#endif // AFFINETRANSFORM_H
