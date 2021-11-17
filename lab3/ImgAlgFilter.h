#ifndef IMGALGFILTER_H
#define IMGALGFILTER_H

#include "imgalgconv.h"
#include <cmath>

template <typename T>
class ImgAlgFilter : public virtual ImgAlgConv<T>
{
public:
    ImgAlgFilter();
    ImgAlgFilter(const ImgData<T> &img);
    ImgAlgFilter(ImgData<T> &&img);

    static ImgData<float> __getGaussianKernel1Dx(int kernel_size, float sigma);
    static ImgData<float> __getGaussianKernel1Dy(int kernel_size, float sigma);
    static ImgData<float> __getGaussianKernel2D(int kernel_size, float sigma);
    static ImgData<float> __getGaussianHpKernel1Dx(int kernel_size, float sigma);
    static ImgData<float> __getGaussianHpKernel1Dy(int kernel_size, float sigma);
    static ImgData<float> __getGaussianHpKernel2D(int kernel_size, float sigma);
    static ImgData<float> __getLaplacianKernel2D(int kernel_size);

    ImgData<T> _filter(ImgData<float> kernel);
    ImgData<T> _filter(ImgData<float> kernel_x, ImgData<float> kernel_y);

protected:
};

template <typename T>
ImgAlgFilter<T>::ImgAlgFilter()
{
}

template <typename T>
ImgAlgFilter<T>::ImgAlgFilter(const ImgData<T> &img) : ImgData<T>(img)
{
}

template <typename T>
ImgAlgFilter<T>::ImgAlgFilter(ImgData<T> &&img) : ImgData<T>(img)
{
}

template <typename T>
ImgData<float> ImgAlgFilter<T>::__getGaussianKernel1Dx(int kernel_size, float sigma)
{
    // 建议 kernel_size > 1 / sigma
    // sigma 是**频域**高斯函数的标准差
    assert(kernel_size % 2 == 1);
    ImgData<float> kernel(kernel_size, 1, 1.0f);

    // y = sqrt(2*pi) * s * exp(-2*pi*pi*s*s*power(x,2))  抽样后归一化
    float pi = 3.14159265;
    float coef = 2 * pi * pi * sigma * sigma;
    float sum = 1e-8; // 防止除零

    for (int i = 0; i < kernel_size; i++)
    {
        float x = i - (kernel_size / 2);
        float y = exp(-coef * x * x);
        kernel.setPixel(i, 0, y);
        sum += y;
    }

    for (int i = 0; i < kernel_size; i++)
    {
        kernel.setPixel(i, 0, kernel.pixel(i, 0) / sum);
    }

    return kernel;
}

template <typename T>
ImgData<float> ImgAlgFilter<T>::__getGaussianKernel1Dy(int kernel_size, float sigma)
{
    ImgData<float> kernel_x = __getGaussianKernel1Dx(kernel_size, sigma);
    return kernel_x._transpose();
}

template <typename T>
ImgData<float> ImgAlgFilter<T>::__getGaussianKernel2D(int kernel_size, float sigma)
{
    // 可以进一步扩充非各项同性的情况
    ImgData<float> kernel_x = __getGaussianKernel1Dx(kernel_size, sigma);
    ImgData<float> kernel_y = __getGaussianKernel1Dy(kernel_size, sigma);
    return kernel_y._multiply(kernel_x);
}

template <typename T>
ImgData<float> ImgAlgFilter<T>::__getGaussianHpKernel1Dx(int kernel_size, float sigma)
{
    // 建议 kernel_size > 1 / sigma
    // sigma 是**频域**高斯函数的标准差
    assert(kernel_size % 2 == 1);

    ImgData<float> kernel = __getGaussianKernel1Dx(kernel_size, sigma);
    int center = kernel_size / 2;
    kernel.setPixel(center, 0, kernel.pixel(center, 0) - 1);

    return kernel._amplify(-1);
}

template <typename T>
ImgData<float> ImgAlgFilter<T>::__getGaussianHpKernel1Dy(int kernel_size, float sigma)
{
    ImgData<float> kernel_x = __getGaussianHpKernel1Dx(kernel_size, sigma);
    return kernel_x._transpose();
}

template <typename T>
ImgData<float> ImgAlgFilter<T>::__getGaussianHpKernel2D(int kernel_size, float sigma)
{
    // 可以进一步扩充非各项同性的情况
    ImgData<float> kernel_x = __getGaussianHpKernel1Dx(kernel_size, sigma);
    ImgData<float> kernel_y = __getGaussianHpKernel1Dy(kernel_size, sigma);
    return kernel_y._multiply(kernel_x);
}

template <typename T>
ImgData<float> ImgAlgFilter<T>::__getLaplacianKernel2D(int kernel_size)
{
    assert(kernel_size > 1 && kernel_size % 2 == 1);
    ImgData<float> kernel(kernel_size, kernel_size, 1);
    for (int i = 0; i < kernel_size; i++)
        for (int j = 0; j < kernel_size; j++)
            kernel.setPixel(j, i, 1);
    kernel.setPixel(1, 1, -(kernel_size * kernel_size - 1));
    return kernel;
}

template <typename T>
ImgData<T> ImgAlgFilter<T>::_filter(ImgData<float> kernel)
{
    return this->_conv2d(kernel);
}

template <typename T>
ImgData<T> ImgAlgFilter<T>::_filter(ImgData<float> kernel_x, ImgData<float> kernel_y)
{
    ImgAlgConv<T> tmp = this->_conv2d(kernel_x);
    return tmp._conv2d(kernel_y);
}

#endif // IMGALGFILTER_H
