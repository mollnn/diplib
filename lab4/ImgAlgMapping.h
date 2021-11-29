#ifndef ImgAlgMapping_H
#define ImgAlgMapping_H

#include "ImgData.h"

#include <immintrin.h>

template <typename T>
class ImgAlgMapping : public virtual ImgData<T>
{
public:
    ImgAlgMapping() {}

protected:
    template <typename R = T>
    ImgData<R> _linearMapSimple(R target_range, T source_range);

    template <typename R = T>
    ImgData<R> _linearMapRange(R target_min, R target_max, T source_min, T source_max);

    template <typename R = T>
    ImgData<R> _linearMapRange_Baseline(R target_min, R target_max, T source_min, T source_max);

    template <typename R = T>
    ImgData<R> _linearMapRange_LUT(R target_min, R target_max, T source_min, T source_max);
};

template <typename T>
template <typename R>
ImgData<R> ImgAlgMapping<T>::_linearMapSimple(R target_range, T source_range)
{
    R type_max = (1ull << (8 * sizeof(R))) - 1;
    ImgData<R> result(this->width_, this->height_, type_max);

#pragma omp parallel for
    for (int i = 0; i < this->height_; i++)
    {
        for (int j = 0; j < this->width_; j++)
        {
            T pixel = this->data_[i * this->width_ + j];
            float value = 1.0f * pixel * target_range / source_range;
            result.setPixel(j, i, value);
        }
    }
    return result;
}

template <typename T>
template <typename R>
ImgData<R> ImgAlgMapping<T>::_linearMapRange_Baseline(R target_min, R target_max, T source_min, T source_max)
{
    R type_max = (1ull << (8 * sizeof(R))) - 1;
    ImgData<R> result(this->width_, this->height_, type_max);

    auto target_data_ptr = result.bits();

#pragma omp parallel for
    for (int i = 0; i < this->height_; i++)
    {
        for (int j = 0; j < this->width_; j++)
        {
            T pixel = this->data_[i * this->width_ + j];
            float delta = 1.0f * pixel - source_min;
            if (delta > source_max - source_min)
                delta = source_max - source_min;
            if (delta < 0)
                delta = 0;
            delta *= target_max - target_min;
            delta /= source_max - source_min + 1e-8;
            target_data_ptr[i * this->width_ + j] = delta + target_min;
        }
    }
    return result;
}

template <typename T>
template <typename R>
ImgData<R> ImgAlgMapping<T>::_linearMapRange_LUT(R target_min, R target_max, T source_min, T source_max)
{
    R type_max = (1ull << (8 * sizeof(R))) - 1;
    ImgData<R> result(this->width_, this->height_, type_max);

    R *lut = new R[1ull + this->range_];

#pragma omp parallel for
    for (int i = 0; i <= this->range_; i++)
    {
        T pixel = i;
        float delta = 1.0f * pixel - source_min;
        if (delta > source_max - source_min)
            delta = source_max - source_min;
        if (delta < 0)
            delta = 0;
        delta *= target_max - target_min;
        delta /= source_max - source_min + 1e-8;
        lut[i] = delta + target_min;
    }

    auto target_data_ptr = result.bits();

#pragma omp parallel for
    for (int i = 0; i < this->height_; i++)
    {
        for (int j = 0; j < this->width_; j++)
        {
            target_data_ptr[i * this->width_ + j] = lut[this->data_[i * this->width_ + j]];
        }
    }

    delete[] lut;

    return result;
}

template <typename T>
template <typename R>
ImgData<R> ImgAlgMapping<T>::_linearMapRange(R target_min, R target_max, T source_min, T source_max)
{
    return _linearMapRange_LUT(target_min, target_max, source_min, source_max);
}

#endif // IMGALGLINEARMAPPER_H
