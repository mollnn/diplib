#ifndef IMGALGCONV_H
#define IMGALGCONV_H

#include "ImgData.h"
#include "ImgAlgCopy.h"

template <typename T>
class ImgAlgConv : public virtual ImgAlgCopy<T>
{
public:
    ImgAlgConv() {}
    ImgAlgConv(const ImgData<T> &img) : ImgData<T>(img) {}
    ImgAlgConv(ImgData<T> &&img) : ImgData<T>(img) {}

    ImgData<T> _conv2d(ImgData<float> kernel)
    {
        return this->_conv2d_Fast(kernel);
    }

    ImgData<T> _conv2d_Baseline(ImgData<float> kernel)
    {
        ImgData<T> result(this->width_, this->height_, this->range_);

        int kernel_width_half = (kernel.width()-1)/2;
        int kernel_height_half = (kernel.height()-1)/2;

#pragma omp parallel for
        for(int i=0;i<result.height();i++)
        {
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

    ImgData<T> _conv2d_Fast(ImgData<float> kernel)
    {
        ImgData<T> result(this->width_, this->height_, this->range_);

        int kernel_width_half = (kernel.width()-1)/2;
        int kernel_height_half = (kernel.height()-1)/2;

        int image_padding_width = this->width() + 2*kernel_width_half;
        int image_padding_height = this->height() + 2*kernel_height_half;

        int kernel_width = kernel.width();
        int kernel_height = kernel.height();

        int result_width = result.width();
        int result_height = result.height();

        // 先生成 padding 后的图像，以避免循环内层出现判断
        ImgData<T> image_padding = this->_copySubImg(-kernel_width_half, -kernel_height_half, image_padding_width, image_padding_height);

        T* image_padding_ptr = image_padding.bits();
        T* image_result_ptr = result.bits();
        float* kernel_ptr = kernel.bits();

#pragma omp parallel for
        for(int i=0;i<result_height;i++)
        {
            for(int j=0;j<result_width;j++)
            {
                T* image_result_px_ptr = image_result_ptr + i * result_width + j;
                float sum = 0;
                for(int u=0;u<kernel_height;u++)
                {
                    for(int v=0;v<kernel_width;v++)
                    {
                        T* image_padding_px_ptr = image_padding_ptr + (i + kernel_height - u - 1) * image_padding_width + j + kernel_width - v - 1;
                        float* kernel_px_ptr = kernel_ptr + u * kernel_width + v;
                        sum += (*kernel_px_ptr) * (*image_padding_px_ptr);
                    }
                }
                (*image_result_px_ptr) = sum;
            }
        }

        return result;
    }
protected:

};

#endif // IMGALGCONV_H
