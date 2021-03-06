#ifndef ImgFuncGrayWindow_H
#define ImgFuncGrayWindow_H

#include "ImgData.h"
#include "ImgAlgMapping.h"
#include <cmath>

template <typename T>
class ImgFuncGrayWindow : public virtual ImgAlgMapping<T>
{
public:
    ImgFuncGrayWindow() {}

public:
    template <typename R>
    ImgData<R> applyRangeScaler(float in_l, float in_r, R out_l = 0, R out_r = (1ull << (8 * sizeof(R))) - 1);

    template <typename R>
    ImgData<R> applyGrayWindow(T breadth, T position);
};

template <typename T>
template <typename R>
ImgData<R> ImgFuncGrayWindow<T>::applyRangeScaler(float in_l, float in_r, R out_l, R out_r)
{
    int type_max_out = (1ull << (8 * sizeof(R))) - 1;
    // 不考虑 in_l > in_r 的情况
    // 求等效参数，使得 in_l, in_r 在值域内
    float slope = (out_r - out_l) * 1.0 / (1e-8 + in_r - in_l);
    if (in_l < 0)
    {
        out_l = std::max(0.0f, out_l - in_l * slope);
        in_l = 0;
    }
    if (in_r > this->range_)
    {
        out_r = std::min(type_max_out + 0.0f, out_r - (in_r - this->range_) * slope);
        in_r = this->range_;
    }
    return this->template _linearMapRange<R>(out_l, out_r, in_l, in_r);
}

template <typename T>
template <typename R>
ImgData<R> ImgFuncGrayWindow<T>::applyGrayWindow(T breadth, T position)
{
    float in_l = position - breadth / 2;
    float in_r = position + breadth / 2;
    return this->applyRangeScaler<R>(in_l, in_r);
}

#endif // IMGGRAYWINDOW_H
