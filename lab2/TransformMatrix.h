#ifndef AFFINETRANSFORM_H
#define AFFINETRANSFORM_H

#include "mathheader.h"

class TransformMatrix
{
private:
    mat3 m_;
public:
    TransformMatrix();
    TransformMatrix(double x11, double x12, double x13, double x21, double x22, double x23);
    TransformMatrix(double x11, double x12, double x21, double x22);
    TransformMatrix(const TransformMatrix& rhs);
    TransformMatrix(const mat3& rhs);
    TransformMatrix& operator = (const TransformMatrix& rhs);
    TransformMatrix& operator = (const mat3& rhs);
    mat3 to_mat3();
    double item(int i,int j);
    void setItem(int i,int j,double v);
    TransformMatrix operator * (const TransformMatrix& rhs);
    TransformMatrix& operator *= (const TransformMatrix& rhs);
    void translate(double dx, double dy);
    void rotate(double angle);
    void scale(double scalar);
};

#endif // AFFINETRANSFORM_H
