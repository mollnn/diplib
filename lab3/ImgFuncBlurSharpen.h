#ifndef IMGFUNCBLURSHARPEN_H
#define IMGFUNCBLURSHARPEN_H

#include "ImgAlgFilter.h"
#include <cmath>

template <typename T>
class ImgFuncBlurSharpen : public virtual ImgAlgFilter<T>
{
public:
    ImgFuncBlurSharpen() {}

    ImgData<T> gaussianBlur(float sigma)
    {
        int kernel_size = 1.0 / sigma + 1;
        if(kernel_size % 2 == 0) kernel_size += 1;

        ImgData<float> kernel = this->__getGaussianKernel2D(kernel_size, sigma);
        return this->_filter(kernel);
    }

    ImgData<T> laplacian(int kernel_size)
    {
        ImgData<float> kernel = this->__getLaplacianKernel2D(kernel_size);
        return this->_filter(kernel);
    }

    ImgData<T> _unsharpMasking(ImgData<float> kernel_hpf, float alpha)
    {
        ImgData<float> image_src(this->width(), this->height(), 1.0);

#pragma omp parallel for
        for(int i=0;i<this->height();i++)
        {
            for(int j=0;j<this->width();j++)
            {
                image_src.setPixel(j,i,1.0 * this->pixel(j,i) / this->range_);
            }
        }

        ImgData<float> image_hf = static_cast<ImgAlgFilter<float>>(image_src)._filter(kernel_hpf);
        ImgData<float> image_result = image_src._add(image_hf._amplify(alpha));
        ImgData<T> result(this->width(), this->height(), this->range());

#pragma omp parallel for
        for(int i=0;i<this->height();i++)
        {
            for(int j=0;j<this->width();j++)
            {
                result.setPixel(j,i,image_result.pixelClamped(j,i) * this->range_);
            }
        }

        return result;
    }
};

#endif // IMGFUNCBLURSHARPEN_H
