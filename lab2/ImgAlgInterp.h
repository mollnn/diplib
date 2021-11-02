#ifndef IMGALGINTERPOLATE_H
#define IMGALGINTERPOLATE_H

#include "ImgData.h"
#include <cmath>

#include <immintrin.h>
#include <xmmintrin.h>

template <typename T>
class ImgAlgInterp : public virtual ImgData<T>
{
public:
    ImgAlgInterp() {}
protected:
    T _pxInterpBilinear(float x, float y);
//    ImgData<T> _interpBilinear(float* coords);
};

///////////////////////////////////////////////

template <typename T>
T ImgAlgInterp<T>::_pxInterpBilinear(float x, float y)
{
    int x0 = floor(x);
    int x1 = x0 + 1;
    int y0 = floor(y);
    int y1 = y0 + 1;
    return ((x1 - x) * this->_pixel(x0, y0) + (x - x0) * this->_pixel(x1, y0)) * (y1 - y) +
            ((x1 - x) * this->_pixel(x0, y1) + (x - x0) * this->_pixel(x1, y1)) * (y - y0);
}

#endif // IMGALGINTERPOLATE_H
