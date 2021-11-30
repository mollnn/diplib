#ifndef ImgFuncBlurSharpen_H
#define ImgFuncBlurSharpen_H

#include "ImgAlgFilter.h"
#include <cmath>

template <typename T>
class ImgFuncBlurSharpen : public virtual ImgAlgFilter<T>
{
public:
    ImgFuncBlurSharpen();
    ImgFuncBlurSharpen(const ImgData<T> &img);
    ImgFuncBlurSharpen(ImgData<T> &&img);

    ImgData<T> gaussianBlur(float sigma);
    ImgData<T> laplacian(int kernel_size);
    ImgData<T> unsharpMasking(ImgData<float> filter_kernel, float alpha, T threshold = 0);
    ImgData<T> backgroundCorrection(float gaussian_sigma = 0.01, bool log_space = true);
};

template <typename T>
ImgFuncBlurSharpen<T>::ImgFuncBlurSharpen()
{
}

template <typename T>
ImgFuncBlurSharpen<T>::ImgFuncBlurSharpen(const ImgData<T> &img) : ImgData<T>(img)
{
}

template <typename T>
ImgFuncBlurSharpen<T>::ImgFuncBlurSharpen(ImgData<T> &&img) : ImgData<T>(img)
{
}

template <typename T>
ImgData<T> ImgFuncBlurSharpen<T>::gaussianBlur(float sigma)
{
    int kernel_size = 1.0 / sigma + 1;
    kernel_size = std::min(kernel_size, 1000);
    if (kernel_size % 2 == 0)
        kernel_size += 1;

    ImgData<float> kernel_x = this->getGaussianKernel1Dx(kernel_size, sigma);
    ImgData<float> kernel_y = this->getGaussianKernel1Dy(kernel_size, sigma);

    return this->filter(kernel_x, kernel_y);
}

template <typename T>
ImgData<T> ImgFuncBlurSharpen<T>::laplacian(int kernel_size)
{
    ImgData<float> kernel = this->getLaplacianKernel2D(kernel_size);
    return this->filter(kernel);
}

template <typename T>
ImgData<T> ImgFuncBlurSharpen<T>::unsharpMasking(ImgData<float> filter_kernel, float alpha, T threshold)
{
    float threshold_f = 1.0f * threshold / this->range_;

    ImgAlgFilter<float> image_src = this->template cast<float>(1.0f / this->range_, 0);
    ImgData<float> image_hf = image_src.filter(filter_kernel);

    // USM 阈值：仅考虑高频分量绝对值超过 threshold 的部分
    ImgData<float> image_scrap = image_hf.clamp(-threshold_f, +threshold_f);
    ImgData<float> image_result = image_src + (image_hf - image_scrap) * alpha;
    return image_result.clamp(0, 1).cast<T>(this->range_, 0);
}

template <typename T>
ImgData<T> ImgFuncBlurSharpen<T>::backgroundCorrection(float gaussian_sigma, bool log_space)
{
    if (log_space)
    {
        ImgFuncBlurSharpen<float> a = this->template cast<float>();
        ImgFuncBlurSharpen<float> b = a.log(2);
        ImgFuncBlurSharpen<float> b1 = b.gaussianBlur(gaussian_sigma);
        ImgFuncBlurSharpen<float> b2 = b - b1;
        ImgFuncBlurSharpen<float> b3 = b2 + log2(this->range() / 2 + 1);
        ImgFuncBlurSharpen<float> c = b3.exp(2);
        ImgFuncBlurSharpen<float> c1 = c.clamp(0, this->range());
        return c1.cast<T>();
    }
    else
    {
        ImgFuncBlurSharpen<float> a = this->template cast<float>();
        ImgFuncBlurSharpen<float> b1 = a.gaussianBlur(gaussian_sigma);
        ImgFuncBlurSharpen<float> b2 = a - b1;
        ImgFuncBlurSharpen<float> b3 = b2 + this->range() / 2;
        ImgFuncBlurSharpen<float> c1 = b3.clamp(0, this->range());
        return c1.cast<T>();
    }
}

#endif // IMGFUNCBLURSHARPEN_H
