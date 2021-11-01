#ifndef IMGGRAYWINDOW_H
#define IMGGRAYWINDOW_H

#include "ImgData.h"
#include "ImgAlgLinearMap.h"
#include <cmath>

template <typename T>
class ImgGrayWindow: public virtual ImgAlgLinearMap<T>
{
public:
    ImgGrayWindow() {}

public:
    template <typename R>
    ImgData<R> applyRangeScalar(int in_l, int in_r, R out_l = 0, R out_r = (1ull<<(8*sizeof(R)))-1)
    {
        int type_max_out = (1ull<<(8*sizeof(R)))-1;
        // in_l, in_r 采用普通整型数：为了支持超出值域的 in_l, in_r；不考虑 in_l > in_r 的情况
        double slope = (out_r - out_l) * 1.0 / (1e-8 + in_r - in_l);
        if(in_l<0)
        {
            out_l = std::max(0.0, out_l - in_l * slope);
            in_l=0;
        }
        if(in_r>this->range_)
        {
            out_r = std::min(type_max_out + 0.0, out_r - (in_r-this->range_)*slope);
            in_r=this->range_;
        }
        return this->template _linearMapRange<R>(out_l, out_r, in_l, in_r);
    }

    template <typename R>
    ImgData<R> applyGrayWindow(T breadth, T position)
    {
        int in_l = (int)position - breadth / 2;
        int in_r = (int)position + (breadth - 1) / 2;
        return this->applyRangeScalar<R>(in_l, in_r);
    }
};

#endif // IMGGRAYWINDOW_H
