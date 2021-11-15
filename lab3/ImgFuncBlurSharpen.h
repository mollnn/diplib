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

        ImgData<float> kernel_x = this->__getGaussianKernel1Dx(kernel_size, sigma);
        ImgData<float> kernel_y = this->__getGaussianKernel1Dy(kernel_size, sigma);

        return this->_filter(kernel_x, kernel_y);
    }

    ImgData<T> laplacian(int kernel_size)
    {
        ImgData<float> kernel = this->__getLaplacianKernel2D(kernel_size);
        return this->_filter(kernel);
    }

    ImgData<T> _unsharpMasking(ImgData<float> kernel_hpf, float alpha, T threshold = 0)
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

        // USM 阈值：仅考虑高频分量绝对值超过 threshold 的部分
        float threshold_f = 1.0 * threshold / this->range_;
        for(int i=0;i<this->height();i++)
        {
            for(int j=0;j<this->width();j++)
            {
                image_hf.setPixel(j,i,image_hf.pixel(j,i) - std::max(-threshold_f, std::min(threshold_f, image_hf.pixel(j,i))));
            }
        }

//         USM 中值：对提取出的高频部分再做一次 median filter
//        ImgData<float> image_hf1(this->width(), this->height(), 1.0);
//        int median_size = 3;
//        int median_radius = median_size / 2;
//        for(int i=0;i<this->height();i++)
//        {
//            for(int j=0;j<this->width();j++)
//            {
//                std::vector<float> buf;
//                for(int u=-median_radius;u<=median_radius;u++)
//                {
//                    for(int v=-median_radius;v<=median_radius;v++)
//                    {
//                        buf.push_back(image_hf._pixel(j+v, i+u));
//                    }
//                }
//                std::sort(buf.begin(), buf.end());
//                image_hf1.setPixel(j,i,(buf[buf.size()/2-1] +buf[buf.size()/2] + buf[buf.size()/2+1])/3);
//            }
//        }

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
