#ifndef AFFINETRANSFORM_H
#define AFFINETRANSFORM_H

#include "mathheader.h"

class TransformMatrix
{
private:
    mat3 m_;
public:
    TransformMatrix();
    TransformMatrix(float x11, float x12, float x13, float x21, float x22, float x23);
    TransformMatrix(float x11, float x12, float x21, float x22);
    TransformMatrix(const TransformMatrix& rhs);
    TransformMatrix(const mat3& rhs);
    TransformMatrix& operator = (const TransformMatrix& rhs);
    TransformMatrix& operator = (const mat3& rhs);
    mat3 to_mat3();
    float item(int i,int j);
    void setItem(int i,int j,float v);
    TransformMatrix operator * (const TransformMatrix& rhs);
    TransformMatrix& operator *= (const TransformMatrix& rhs);
    void translate(float dx, float dy);
    void rotate(float angle);
    void scale(float scalar);
};

#endif // AFFINETRANSFORM_H
