#ifndef IMGINTERPOLATE_H
#define IMGINTERPOLATE_H

#include "imgdata.h"
#include <cmath>

template <typename T>
class ImgInterpolate : public virtual ImgData<T>
{
    T _getPixelWithDefault(int x, int y, T default_value = 0)
    {
        if (x >= 0 && x < this->width_ && y >= 0 && y < this->height_)
            return this->pixel(x, y);
        return default_value;
    }

public:
    ImgInterpolate() {}
    T interpolateBilinear(double x, double y)
    {
        int x0 = floor(x), x1 = x0 + 1, y0 = floor(y), y1 = y0 + 1;
        return ((x1 - x) * _getPixelWithDefault(x0, y0) +
                (x - x0) * _getPixelWithDefault(x1, y0)) *
                   (y1 - y) +
               ((x1 - x) * _getPixelWithDefault(x0, y1) +
                (x - x0) * _getPixelWithDefault(x1, y1)) *
                   (y - y0);
    }
};

#endif // IMGINTERPOLATE_H
