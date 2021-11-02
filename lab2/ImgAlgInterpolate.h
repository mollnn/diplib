#ifndef IMGALGINTERPOLATE_H
#define IMGALGINTERPOLATE_H

#include "ImgData.h"
#include <cmath>

template <typename T>
class ImgAlgInterpolate : public virtual ImgData<T>
{
public:
    ImgAlgInterpolate() {}
protected:
    T _interpolateBilinear(float x, float y);
    T _interpolateBilinear_Baseline(float x, float y);
    T _interpolateBilinear_Fixedpoint(float x, float y);
};

///////////////////////////////////////////////

template <typename T>
T ImgAlgInterpolate<T>::_interpolateBilinear_Baseline(float x, float y)
{
    int x0 = floor(x);
    int x1 = x0 + 1;
    int y0 = floor(y);
    int y1 = y0 + 1;
    return ((x1 - x) * this->_pixel(x0, y0) + (x - x0) * this->_pixel(x1, y0)) * (y1 - y) +
            ((x1 - x) * this->_pixel(x0, y1) + (x - x0) * this->_pixel(x1, y1)) * (y - y0);
}

template <typename T>
T ImgAlgInterpolate<T>::_interpolateBilinear_Fixedpoint(float x, float y)
{
    int x0 = x;
    int x1 = x0 + 1;
    int y0 = y;
    int y1 = y0 + 1;

    int a00 = this->_pixel(x0, y0);
    int a01 = this->_pixel(x0, y1);
    int a10 = this->_pixel(x1, y0);
    int a11 = this->_pixel(x1, y1);

    int xf = x * 256;
    int yf = y * 256;

    int x0f = x0 << 8;
    int x1f = x1 << 8;
    int y0f = y0 << 8;
    int y1f = y1 << 8;

    int c00 = (x1f - xf)*(y1f - yf);
    int c01 = (x1f - xf)*(yf - y0f);
    int c10 = (xf - x0f)*(y1f - yf);
    int c11 = (xf - x0f)*(yf - y0f);

    int sum = a00 * c00 + a01 * c01 + a10 * c10 + a11 * c11;

    return sum >> 16;
}

template <typename T>
T ImgAlgInterpolate<T>::_interpolateBilinear(float x, float y)
{
    return _interpolateBilinear_Fixedpoint(x,y);
}

#endif // IMGALGINTERPOLATE_H
