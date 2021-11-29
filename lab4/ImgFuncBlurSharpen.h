#ifndef ImgFuncBlurSharpen_H
#define ImgFuncBlurSharpen_H

#include "ImgAlgFilter.h"
#include <cmath>

template <typename T>
class ImgFuncBlurSharpen : public virtual ImgAlgFilter<T>
{
public:
    ImgFuncBlurSharpen();

    ImgData<T> gaussianBlur(float sigma);
    ImgData<T> laplacian(int kernel_size);
    ImgData<T> unsharpMasking(ImgData<float> kernel_hpf, float alpha, T threshold = 0);
};

template <typename T>
ImgFuncBlurSharpen<T>::ImgFuncBlurSharpen()
{
}

template <typename T>
ImgData<T> ImgFuncBlurSharpen<T>::gaussianBlur(float sigma)
{
    int kernel_size = 1.0 / sigma + 1;
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
ImgData<T> ImgFuncBlurSharpen<T>::unsharpMasking(ImgData<float> kernel_hpf, float alpha, T threshold)
{
    float threshold_f = 1.0f * threshold / this->range_;

    ImgAlgFilter<float> image_src = this->template cast<float>(1.0f / this->range_, 0);
    ImgData<float> image_hf = image_src.filter(kernel_hpf);

    // USM 阈值：仅考虑高频分量绝对值超过 threshold 的部分
    ImgData<float> image_scrap = image_hf.clamp(-threshold_f, +threshold_f);
    ImgData<float> image_result = image_src + (image_hf - image_scrap) * alpha;
    return image_result.clamp(0, 1).cast<T>(this->range_, 0);
}

#endif // IMGFUNCBLURSHARPEN_H
