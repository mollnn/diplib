#ifndef IMGALGCONV_H
#define IMGALGCONV_H

#include "ImgData.h"
#include "ImgAlgCopy.h"

#include <immintrin.h>
#include <xmmintrin.h>
#include <avx2intrin.h>

#include <QTime>

template <typename T>
class ImgAlgConv : public virtual ImgAlgCopy<T>
{
public:
    ImgAlgConv();
    ImgAlgConv(const ImgData<T> &img);
    ImgAlgConv(ImgData<T> &&img);
    ImgData<T> _conv2d(ImgData<float> kernel);
    ImgData<T> _conv2d_Baseline(ImgData<float> kernel);
    ImgData<T> _conv2d_Fast(ImgData<float> kernel);
    ImgData<T> _conv2d_Avx2(ImgData<float> kernel);
protected:
};

template <typename T>
ImgAlgConv<T>::ImgAlgConv() {}

template <typename T>
ImgAlgConv<T>::ImgAlgConv(const ImgData<T> &img) : ImgData<T>(img) {}

template <typename T>
ImgAlgConv<T>::ImgAlgConv(ImgData<T> &&img) : ImgData<T>(img) {}

template <typename T>
ImgData<T> ImgAlgConv<T>::_conv2d(ImgData<float> kernel)
{
    if (kernel.width() * kernel.height() >= 9 || kernel.height() >= 5)
        return this->_conv2d_Avx2(kernel);
    else
        return this->_conv2d_Fast(kernel);
}

template <typename T>
ImgData<T> ImgAlgConv<T>::_conv2d_Baseline(ImgData<float> kernel)
{
    ImgData<T> result(this->width_, this->height_, this->range_);

    int kernel_width_half = (kernel.width() - 1) / 2;
    int kernel_height_half = (kernel.height() - 1) / 2;

#pragma omp parallel for
    for (int i = 0; i < result.height(); i++)
    {
        for (int j = 0; j < result.width(); j++)
        {
            float sum = 0;
            for (int u = 0; u < kernel.height(); u++)
            {
                for (int v = 0; v < kernel.width(); v++)
                {
                    sum += this->_pixel(j + kernel_width_half - v, i + kernel_height_half - u) * kernel.pixel(v, u);
                }
            }
            result.setPixel(j, i, sum);
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgAlgConv<T>::_conv2d_Fast(ImgData<float> kernel)
{
    // 先生成 padding 后的图像，以避免循环内层出现分支
    ImgData<T> result(this->width_, this->height_, this->range_);

    int kernel_width_half = (kernel.width() - 1) / 2;
    int kernel_height_half = (kernel.height() - 1) / 2;

    int image_padding_width = this->width() + 2 * kernel_width_half;
    int image_padding_height = this->height() + 2 * kernel_height_half;

    int kernel_width = kernel.width();
    int kernel_height = kernel.height();

    int result_width = result.width();
    int result_height = result.height();

    ImgData<T> image_padding = this->_copySubImg(-kernel_width_half, -kernel_height_half, image_padding_width, image_padding_height);

    T *image_padding_ptr = image_padding.bits();
    T *image_result_ptr = result.bits();
    float *kernel_ptr = kernel.bits();

#pragma omp parallel for
    for (int i = 0; i < result_height; i++)
    {
        for (int j = 0; j < result_width; j++)
        {
            T *image_result_px_ptr = image_result_ptr + i * result_width + j;
            float sum = 0;
            for (int u = 0; u < kernel_height; u++)
            {
                for (int v = 0; v < kernel_width; v++)
                {
                    T *image_padding_px_ptr = image_padding_ptr + (i + kernel_height - u - 1) * image_padding_width + j + kernel_width - v - 1;
                    float *kernel_px_ptr = kernel_ptr + u * kernel_width + v;
                    sum += (*kernel_px_ptr) * (*image_padding_px_ptr);
                }
            }
            (*image_result_px_ptr) = sum;
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgAlgConv<T>::_conv2d_Avx2(ImgData<float> kernel)
{
    // 卷积核翻转
    kernel = kernel._mirrorXY();

    ImgData<T> result(this->width_, this->height_, this->range_);

    int kernel_width_half = (kernel.width() - 1) / 2;
    int kernel_height_half = (kernel.height() - 1) / 2;

    int image_padding_width = this->width() + 2 * kernel_width_half;
    int image_padding_height = this->height() + 2 * kernel_height_half;

    int kernel_width = kernel.width();
    int kernel_height = kernel.height();

    int result_width = result.width();
    int result_width_r8 = result_width / 8 * 8;
    int result_height = result.height();

    ImgData<T> image_padding = this->_copySubImg(-kernel_width_half, -kernel_height_half, image_padding_width, image_padding_height);

    T *image_padding_ptr = image_padding.bits();
    T *image_result_ptr = result.bits();
    float *kernel_ptr = kernel.bits();

    // 图像转换为 32 位浮点
    float *image_ps;
    if (sizeof(T) == 4)
    {
        image_ps = reinterpret_cast<float *>(image_padding_ptr);
    }
    else
    {
        image_ps = new float[image_padding_width * image_padding_height];
#pragma omp parallel for
        for (int i = 0; i < image_padding_width * image_padding_height; i++)
        {
            image_ps[i] = image_padding_ptr[i];
        }
    }

    // 卷积核扩展（每像素重复 8 次）
    float *kernel_ps = new float[kernel_width * kernel_height * 8];
    for (int i = 0; i < kernel_width * kernel_height; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            kernel_ps[i * 8 + j] = kernel_ptr[i];
        }
    }

    int *kernel_offsets = new int[kernel_height * kernel_width];
    int *image_offsets = new int[kernel_height * kernel_width];

    for (int u = 0; u < kernel_height; u++)
    {
        for (int v = 0; v < kernel_width; v++)
        {
            kernel_offsets[u * kernel_width + v] = (u * kernel_width + v) * 8;
            image_offsets[u * kernel_width + v] = u * image_padding_width + v;
        }
    }

#pragma omp parallel for
    for (int i = 0; i < result_height; i++)
    {
        // SIMD
        for (int j = 0; j < result_width_r8; j += 8)
        {
            float sum_ps[8];
            // 每次计算 8 个连续像素的结果
            // 卷积仍然通过循环枚举
            __m256 sum_vec = _mm256_setzero_ps();
            float *image_ps_block_ptr = image_ps + i * image_padding_width + j;

            for (int k = 0; k < kernel_height * kernel_width; k++)
            {
                __m256 kernel_vec = _mm256_loadu_ps(kernel_ps + kernel_offsets[k]);
                __m256 image_vec = _mm256_loadu_ps(image_ps_block_ptr + image_offsets[k]);
                sum_vec = _mm256_fmadd_ps(kernel_vec, image_vec, sum_vec);
            }

            if (sizeof(T) == 4)
            {
                _mm256_storeu_ps(reinterpret_cast<float *>(image_result_ptr + i * result_width + j), sum_vec);
            }
            else
            {
                _mm256_storeu_ps(sum_ps, sum_vec);
                // 转回原类型
                for (int r = 0; r < 8; r++)
                {
                    image_result_ptr[i * result_width + j + r] = sum_ps[r];
                }
            }
        }

        // Serial for remaining
        for (int j = result_width_r8; j < result_width; j++)
        {
            T *image_result_px_ptr = image_result_ptr + i * result_width + j;
            float sum = 0;
            for (int u = 0; u < kernel_height; u++)
            {
                for (int v = 0; v < kernel_width; v++)
                {
                    // 卷积核已经翻转过，这里做的是相关运算
                    T *image_padding_px_ptr = image_padding_ptr + (i + u) * image_padding_width + j + v;
                    float *kernel_px_ptr = kernel_ptr + u * kernel_width + v;
                    sum += (*kernel_px_ptr) * (*image_padding_px_ptr);
                }
            }
            (*image_result_px_ptr) = sum;
        }
    }

    if (sizeof(T) != 4)
        delete[] image_ps;

    delete[] kernel_ps;

    delete[] kernel_offsets;
    delete[] image_offsets;

    return result;
}

#endif // IMGALGCONV_H
