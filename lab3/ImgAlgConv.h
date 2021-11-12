#ifndef IMGALGCONV_H
#define IMGALGCONV_H

#include "ImgData.h"

template <typename T>
class ImgAlgConv : public virtual ImgData<T>
{
public:
    ImgAlgConv() {}
    ImgAlgConv(const ImgData<T> &img) : ImgData<T>(img) {}
    ImgAlgConv(ImgData<T> &&img) : ImgData<T>(img) {}

    ImgData<T> _conv2d(ImgData<float> kernel)
    {
        ImgData<T> result(this->width_, this->height_, this->range_);

        int kernel_width_half = (kernel.width()-1)/2;
        int kernel_height_half = (kernel.height()-1)/2;

#pragma omp parallel for
        for(int i=0;i<result.height();i++)
        {
#pragma omp parallel for
            for(int j=0;j<result.width();j++)
            {
                float sum = 0;
                for(int u=0;u<kernel.height();u++)
                {
                    for(int v=0;v<kernel.width();v++)
                    {
                        sum += this->_pixel(j+kernel_width_half-v, i+kernel_height_half-u) * kernel.pixel(v, u);
                    }
                }
                result.setPixel(j,i,sum);
            }
        }

        return result;
    }
protected:

};

#endif // IMGALGCONV_H
