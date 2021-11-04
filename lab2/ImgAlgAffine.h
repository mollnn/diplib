#ifndef IMGALGAFFINE_H
#define IMGALGAFFINE_H

#include "ImgAlgInterp.h"
#include "ImgAlgCopy.h"
#include "mathheader.h"
#include <cmath>

#include <immintrin.h>
#include <xmmintrin.h>
#include <avx2intrin.h>

template <typename T>
class ImgAlgAffine : public virtual ImgAlgInterp<T>
{
public:
    ImgAlgAffine() {}

protected:
    ImgData<T> _transformAffine(const mat3 &transform_matrix, int target_width, int target_height);
    ImgData<T> _transformAffine_Baseline(const mat3 &transform_matrix, int target_width, int target_height);
    ImgData<T> _transformAffine_Fast(const mat3 &transform_matrix, int target_width, int target_height);
    ImgData<T> _transformAffine_Avx2_sep(const mat3 &transform_matrix, int target_width, int target_height);
    ImgData<T> _transformAffine_Avx2(const mat3 &transform_matrix, int target_width, int target_height);
};


//////////////////////////////////////////////


template <typename T>
ImgData<T> ImgAlgAffine<T>::_transformAffine_Baseline(const mat3 &transform_matrix, int target_width, int target_height)
{
    mat3 transform_matrix_inverse = transform_matrix.inverse();
    ImgData<T> result(target_width, target_height, this->range_);
#pragma omp parallel for
    for (int i = 0; i < result.height(); i++)
    {
#pragma omp parallel for
        for (int j = 0; j < result.width(); j++)
        {
            vec3 target_pos = {1.0f * j, 1.0f * i, 1.0};
            vec3 source_pos = transform_matrix_inverse * target_pos;
            result.setPixel(j, i, this->_pxInterpBilinear(source_pos.x, source_pos.y));
        }
    }
    return result;
}

template <typename T>
ImgData<T> ImgAlgAffine<T>::_transformAffine_Fast(const mat3 &transform_matrix, int target_width, int target_height)
{
    mat3 transform_matrix_inverse = transform_matrix.inverse();

    float t11 = transform_matrix_inverse[0][0];
    float t12 = transform_matrix_inverse[0][1];
    float t13 = transform_matrix_inverse[0][2];

    float t21 = transform_matrix_inverse[1][0];
    float t22 = transform_matrix_inverse[1][1];
    float t23 = transform_matrix_inverse[1][2];

    ImgData<T> result(target_width, target_height, this->range_);

#pragma omp parallel for
    for (int i = 0; i < result.height(); i++)
    {
#pragma omp parallel for
        for (int j = 0; j < result.width(); j++)
        {
            float src_x = t11 * j + t12 * i + t13;
            float src_y = t21 * j + t22 * i + t23;
            result.setPixel(j, i, this->_pxInterpBilinear(src_x, src_y));
        }
    }
    return result;
}

template <typename T>
ImgData<T> ImgAlgAffine<T>::_transformAffine_Avx2_sep(const mat3 &transform_matrix, int target_width, int target_height)
{
    mat3 transform_matrix_inverse = transform_matrix.inverse();

    float t11 = transform_matrix_inverse[0][0];
    float t12 = transform_matrix_inverse[0][1];
    float t13 = transform_matrix_inverse[0][2];

    float t21 = transform_matrix_inverse[1][0];
    float t22 = transform_matrix_inverse[1][1];
    float t23 = transform_matrix_inverse[1][2];

    float *x_coords = new float[target_width * target_height];
    float *y_coords = new float[target_width * target_height];

    int target_width_r8 = target_width / 8 * 8;

    __m256 x_weight = _mm256_set1_ps(t11);
    __m256 y_weight = _mm256_set1_ps(t21);

    __m256i deltas = _mm256_set1_epi32(8);

#pragma omp parallel for
    for (int i = 0; i < target_height; i++)
    {
        float src_x0 = t12 * i + t13;
        float src_y0 = t22 * i + t23;

        __m256 x_bias = _mm256_set1_ps(src_x0);
        __m256 y_bias = _mm256_set1_ps(src_y0);

        float* x_coords_lineptr = x_coords + target_width * i;
        float* y_coords_lineptr = y_coords + target_width * i;

        __m256i cols = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);

        for (int j = 0; j < target_width_r8; j+=8)
        {
            __m256 cols_ps = _mm256_cvtepi32_ps(cols);
            __m256 x_prod = _mm256_mul_ps(x_weight, cols_ps);
            __m256 x_coord = _mm256_add_ps(x_prod, x_bias);
            __m256 y_prod = _mm256_mul_ps(y_weight, cols_ps);
            __m256 y_coord = _mm256_add_ps(y_prod, y_bias);
            _mm256_storeu_ps (x_coords_lineptr + j, x_coord);
            _mm256_storeu_ps (y_coords_lineptr + j, y_coord);
            cols=_mm256_add_epi32(cols, deltas);
        }
        for(int j=target_width_r8;j<target_width;j++)
        {
            float src_x = t11 * j + src_x0;
            float src_y = t21 * j + src_y0;
            x_coords[i*target_width + j] = src_x;
            y_coords[i*target_width + j] = src_y;
        }
    }

    auto result = this->_interpBilinear_Avx2(x_coords, y_coords, target_width, target_height);

    delete[] x_coords;
    delete[] y_coords;
    return result;
}

template <typename T>
ImgData<T> ImgAlgAffine<T>::_transformAffine_Avx2(const mat3 &transform_matrix, int target_width, int target_height)
{
    ImgData<T> result(target_width, target_height, this->range_);

    mat3 transform_matrix_inverse = transform_matrix.inverse();

    float t11 = transform_matrix_inverse[0][0];
    float t12 = transform_matrix_inverse[0][1];
    float t13 = transform_matrix_inverse[0][2];

    float t21 = transform_matrix_inverse[1][0];
    float t22 = transform_matrix_inverse[1][1];
    float t23 = transform_matrix_inverse[1][2];

    int target_width_r8 = target_width / 8 * 8;
    int source_img_size = this->width_ * this->height_;
    int target_img_size = target_width * target_height;

    __m256 x_weight = _mm256_set1_ps(t11);
    __m256 y_weight = _mm256_set1_ps(t21);

    __m256i idx_default_value = _mm256_set1_epi32(source_img_size);

    __m256i bound_x_max = _mm256_set1_epi32(this->width_ - 1);
    __m256i bound_y_max = _mm256_set1_epi32(this->height_ - 1);

    __m256i deltas = _mm256_set1_epi32(8);

    auto target_data_ptr = result.bits();
    auto source_data_ptr = this->bits();

    if(source_data_ptr==0)
    {
        memset(target_data_ptr, 0, target_width * target_height * sizeof(T));
        return result;
    }

    // Cast image value to float (stupid but easy)
    float *source_image_ps = new float[source_img_size + 1];  // 末尾附加元素表示默认值
    float *target_image_ps = new float[target_img_size];

    int width = this->width_;
    __m256i widths = _mm256_set1_epi32(width);

#pragma omp parallel for
    for(int i=0;i<source_img_size;i++)
    {
        source_image_ps[i]=source_data_ptr[i];
    }
    source_image_ps[source_img_size] = 0; // 默认值可自定义

#pragma omp parallel for
    for (int i = 0; i < target_height; i++)
    {
        float src_x0 = t12 * i + t13;
        float src_y0 = t22 * i + t23;

        __m256 x_bias = _mm256_set1_ps(src_x0);
        __m256 y_bias = _mm256_set1_ps(src_y0);

        __m256i cols = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);

        for (int j = 0; j < target_width_r8; j+=8)
        {
            __m256 cols_ps = _mm256_cvtepi32_ps(cols);
            __m256 x_prod = _mm256_mul_ps(x_weight, cols_ps);
            __m256 x_coord = _mm256_add_ps(x_prod, x_bias);
            __m256 y_prod = _mm256_mul_ps(y_weight, cols_ps);
            __m256 y_coord = _mm256_add_ps(y_prod, y_bias);
            __m256 ans = ImgAlgInterp<T>::__8pxInterpBilinear_Avx2_ps(source_image_ps, x_coord, y_coord, widths, bound_x_max, bound_y_max, idx_default_value);
            _mm256_storeu_ps(target_image_ps + i * target_width + j, ans);

            cols=_mm256_add_epi32(cols, deltas);
        }

        for(int j=target_width_r8;j<target_width;j++)
        {
            float src_x = t11 * j + src_x0;
            float src_y = t21 * j + src_y0;
            target_image_ps[i*target_width + j]=this->_pxInterpBilinear(src_x, src_y);
        }
    }

#pragma omp parallel for
    for(int i=0;i<target_img_size;i++)
    {
        target_data_ptr[i]=target_image_ps[i];
    }

    // Cast image value back to int

    delete[] source_image_ps;
    delete[] target_image_ps;
    return result;
}

template <typename T>
ImgData<T> ImgAlgAffine<T>::_transformAffine(const mat3 &transform_matrix, int target_width, int target_height)
{
    return _transformAffine_Avx2(transform_matrix, target_width, target_height);
}

#endif // IMGALGAFFINE_H
