#ifndef IMGALGINTERPOLATE_H
#define IMGALGINTERPOLATE_H

#include "imgdata.h"
#include <cmath>

template <typename T>
class ImgAlgInterpolate : public virtual ImgData<T>
{
public:
    ImgAlgInterpolate() {}
protected:
    T interpolateBilinear(double x, double y)
    {
        int x0 = floor(x), x1 = x0 + 1, y0 = floor(y), y1 = y0 + 1;
        return ((x1 - x) * this->_getPixelWithDefault(x0, y0) +
                (x - x0) * this->_getPixelWithDefault(x1, y0)) *
                   (y1 - y) +
               ((x1 - x) * this->_getPixelWithDefault(x0, y1) +
                (x - x0) * this->_getPixelWithDefault(x1, y1)) *
                   (y - y0);
    }
};

#endif // IMGALGINTERPOLATE_H
