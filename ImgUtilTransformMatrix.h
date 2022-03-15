#ifndef ImgUtilTransformMatrix_H_
#define ImgUtilTransformMatrix_H_

#include "ImgUtil_mathheader.h"

class ImgUtilTransformMatrix
{
private:
    mat3 m_;

public:
    ImgUtilTransformMatrix();
    ImgUtilTransformMatrix(float x11, float x12, float x13, float x21, float x22, float x23);
    ImgUtilTransformMatrix(float x11, float x12, float x21, float x22);
    ImgUtilTransformMatrix(const ImgUtilTransformMatrix &rhs);
    ImgUtilTransformMatrix(const mat3 &rhs);
    ImgUtilTransformMatrix &operator=(const ImgUtilTransformMatrix &rhs);
    ImgUtilTransformMatrix &operator=(const mat3 &rhs);
    mat3 to_mat3();
    float item(int i, int j);
    void setItem(int i, int j, float v);
    ImgUtilTransformMatrix operator*(const ImgUtilTransformMatrix &rhs);
    ImgUtilTransformMatrix &operator*=(const ImgUtilTransformMatrix &rhs);
    void translate(float dx, float dy);
    void rotate(float angle);
    void scale(float scalar);
};

#endif // AFFINETRANSFORM_H
