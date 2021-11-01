#ifndef IMGALGLINEARMAPPER_H
#define IMGALGLINEARMAPPER_H

#include "ImgData.h"

template <typename T>
class ImgAlgLinearMap: public virtual ImgData<T>
{
public:
    ImgAlgLinearMap(){}

protected:
    template <typename R=T>
    static R _pixelLinearMapSimple(T pixel, R target_range, T source_range);

    template <typename R=T>
    static R _pixelLinearMapRange(T pixel, R target_min, R target_max, T source_min, T source_max);

    template <typename R=T>
    ImgData<R> _linearMapSimple(R target_range, T source_range);

    template <typename R=T>
    ImgData<R> _linearMapRange_(R target_min, R target_max, T source_min, T source_max);

    template <typename R=T>
    ImgData<R> _linearMapRange_LUT(R target_min, R target_max, T source_min, T source_max);

    template <typename R=T>
    ImgData<R> _linearMapRange(R target_min, R target_max, T source_min, T source_max);
};


////////////////////////////

template <typename T>
template <typename R>
R ImgAlgLinearMap<T>::_pixelLinearMapSimple(T pixel, R target_range, T source_range)
{
    // For example, 65535 -> 255:
    //  0->0, 255->0, 256->1, 511->1, ..., 65535->255
    return 1ull * pixel * (target_range + 1) / (source_range + 1);
}

template <typename T>
template <typename R>
R ImgAlgLinearMap<T>::_pixelLinearMapRange(T pixel, R target_min, R target_max, T source_min, T source_max)
{
    int delta = 1 * pixel - source_min;
    if(delta>source_max - source_min) delta=source_max-source_min;
    if(delta<0) delta=0;
    delta *= target_max - target_min + 1;
    delta /= source_max - source_min + 1;
    return delta + target_min;
}

template <typename T>
template <typename R>
ImgData<R> ImgAlgLinearMap<T>::_linearMapSimple(R target_range, T source_range)
{
    R type_max = (1ull << (8 * sizeof(R))) - 1;
    ImgData<R> result(this->width_, this->height_, type_max);
#pragma omp parallel for
    for(int i=0;i<this->height_;i++)
    {
        for(int j=0;j<this->width_;j++)
        {
            result.setPixel(j,i,this->_pixelLinearMapSimple(this->pixel(j,i),target_range,source_range));
        }
    }
    return result;
}

template <typename T>
template <typename R>
ImgData<R> ImgAlgLinearMap<T>::_linearMapRange_(R target_min, R target_max, T source_min, T source_max)
{
    R type_max = (1ull << (8 * sizeof(R))) - 1;
    ImgData<R> result(this->width_, this->height_, type_max);

#pragma omp parallel for
    for(int i=0;i<this->height_;i++)
    {
        for(int j=0;j<this->width_;j++)
        {
            result.setPixel(j,i,this->_pixelLinearMapRange(this->pixel(j,i),target_min,target_max,source_min,source_max));
        }
    }
    return result;
}

template <typename T>
template <typename R>
ImgData<R> ImgAlgLinearMap<T>::_linearMapRange_LUT(R target_min, R target_max, T source_min, T source_max)
{
    R type_max = (1ull << (8 * sizeof(R))) - 1;
    ImgData<R> result(this->width_, this->height_, type_max);

    R* lut = new R[1ull + this->range_];

#pragma omp parallel for
    for(int i=0; i<=this->range_; i++)
    {
        lut[i]=this->_pixelLinearMapRange<R>(i, target_min, target_max, source_min, source_max);
    }

#pragma omp parallel for
    for(int i=0;i<this->height_;i++)
    {
        for(int j=0;j<this->width_;j++)
        {
            result.setPixel(j,i,lut[this->pixel(j,i)]);
        }
    }

    delete[] lut;

    return result;
}

template <typename T>
template <typename R>
ImgData<R> ImgAlgLinearMap<T>::_linearMapRange(R target_min, R target_max, T source_min, T source_max)
{
    return _linearMapRange_LUT(target_min, target_max, source_min, source_max);
}

#endif // IMGALGLINEARMAPPER_H
