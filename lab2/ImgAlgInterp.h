#ifndef IMGALGINTERPOLATE_H
#define IMGALGINTERPOLATE_H

#include "ImgData.h"
#include <cmath>

#include <immintrin.h>
#include <xmmintrin.h>
#include <avx2intrin.h>

// Using ImgAlg_Cuda.dll (from *.cu)
extern "C" void __declspec(dllimport) __ImgAlgInterp_interpBilinear_cuda_epi8
(uint8_t *dest_ptr, uint8_t *src_ptr, float* x_coords, float* y_coords,  int dest_width, int dest_height, int src_width, int src_height, uint8_t default_value);

extern "C" void __declspec(dllimport) __ImgAlgInterp_interpBilinear_cuda_epi16
(uint16_t *dest_ptr, uint16_t *src_ptr, float* x_coords, float* y_coords,  int dest_width, int dest_height, int src_width, int src_height, uint16_t default_value);

extern "C" void __declspec(dllimport) __ImgAlgInterp_interpBilinear_cuda_ps
(float *dest_ptr, float *src_ptr, float* x_coords, float* y_coords,  int dest_width, int dest_height, int src_width, int src_height, float default_value);


template <typename T>
class ImgAlgInterp : public virtual ImgData<T>
{
public:
    ImgAlgInterp() {}
protected:
    T _pxInterpBilinear(float x, float y);

    inline static __m256 __8pxInterpBilinear_Avx2_ps(float* source_image_ps, __m256 xi, __m256 yi, __m256i widths, __m256i bound_x_max, __m256i bound_y_max, __m256i idx_default_value);

    ImgData<T> _interpBilinear(float* x_coords, float* y_coords,  int target_width, int target_height);
    ImgData<T> _interpBilinear_Baseline(float* x_coords, float* y_coords,  int target_width, int target_height);
    ImgData<T> _interpBilinear_Avx2(float* x_coords, float* y_coords, int target_width, int target_height);
    ImgData<T> _interpBilinear_Cuda(float* x_coords, float* y_coords,  int target_width, int target_height);
private:
    void _interpBilinear_Cuda_C(T *dest_ptr, T *src_ptr, float* x_coords, float* y_coords,  int dest_width, int dest_height, int src_width, int src_height, T default_value);
};

///////////////////////////////////////////////

template <typename T>
T ImgAlgInterp<T>::_pxInterpBilinear(float x, float y)
{
    int x0 = floor(x);
    int x1 = x0 + 1;
    int y0 = floor(y);
    int y1 = y0 + 1;
    return ((x1 - x) * this->_pixel(x0, y0) + (x - x0) * this->_pixel(x1, y0)) * (y1 - y) +
            ((x1 - x) * this->_pixel(x0, y1) + (x - x0) * this->_pixel(x1, y1)) * (y - y0);
}


template <typename T>
ImgData<T> ImgAlgInterp<T>::_interpBilinear(float* x_coords, float* y_coords, int target_width, int target_height)
{
    return _interpBilinear_Cuda(x_coords, y_coords, target_width, target_height);
}


template <typename T>
ImgData<T> ImgAlgInterp<T>::_interpBilinear_Baseline(float* x_coords, float* y_coords, int target_width, int target_height)
{
    ImgData<T> result( target_width, target_height, this->range_);
    auto target_data_ptr = result.bits();
    for(int i=0;i<target_height;i++)
    {
        for(int j=0;j<target_width;j++)
        {
            int idx = i*target_width+j;
            target_data_ptr[idx] = this->_pxInterpBilinear(x_coords[idx], y_coords[idx]);
        }
    }
    return result;
}


template <typename T>
inline __m256 ImgAlgInterp<T>::__8pxInterpBilinear_Avx2_ps(float* source_image_ps, __m256 xi, __m256 yi, __m256i widths, __m256i bound_x_max, __m256i bound_y_max, __m256i idx_default_value)
{
    static __m256i bound_x_min = _mm256_setzero_si256();
    static __m256i bound_y_min = _mm256_setzero_si256();
    static __m256 const1s = _mm256_set1_ps(1);

    __m256 x0 = _mm256_round_ps(xi, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
    __m256 y0 = _mm256_round_ps(yi, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
    __m256 x1 = _mm256_add_ps(x0, const1s);
    __m256 y1 = _mm256_add_ps(y0, const1s);

    __m256i x0_i32 = _mm256_cvtps_epi32(x0);
    __m256i y0_i32 = _mm256_cvtps_epi32(y0);
    __m256i x1_i32 = _mm256_cvtps_epi32(x1);
    __m256i y1_i32 = _mm256_cvtps_epi32(y1);

    __m256i y0_start = _mm256_mullo_epi32(widths, y0_i32);
    __m256i y1_start = _mm256_mullo_epi32(widths, y1_i32);

    __m256i src_idx00 = _mm256_add_epi32(y0_start, x0_i32);
    __m256i src_idx01 = _mm256_add_epi32(y1_start, x0_i32);
    __m256i src_idx10 = _mm256_add_epi32(y0_start, x1_i32);
    __m256i src_idx11 = _mm256_add_epi32(y1_start, x1_i32);

    __m256i boundcheck_x0_min = _mm256_cmpgt_epi32(bound_x_min, x0_i32);
    __m256i boundcheck_x0_max = _mm256_cmpgt_epi32(x0_i32, bound_x_max);
    __m256i boundcheck_x1_min = _mm256_cmpgt_epi32(bound_x_min, x1_i32);
    __m256i boundcheck_x1_max = _mm256_cmpgt_epi32(x1_i32, bound_x_max);
    __m256i boundcheck_y0_min = _mm256_cmpgt_epi32(bound_y_min, y0_i32);
    __m256i boundcheck_y0_max = _mm256_cmpgt_epi32(y0_i32, bound_y_max);
    __m256i boundcheck_y1_min = _mm256_cmpgt_epi32(bound_y_min, y1_i32);
    __m256i boundcheck_y1_max = _mm256_cmpgt_epi32(y1_i32, bound_y_max);

    __m256i boundcheck_x0 = _mm256_or_si256(boundcheck_x0_min, boundcheck_x0_max);
    __m256i boundcheck_x1 = _mm256_or_si256(boundcheck_x1_min, boundcheck_x1_max);
    __m256i boundcheck_y0 = _mm256_or_si256(boundcheck_y0_min, boundcheck_y0_max);
    __m256i boundcheck_y1 = _mm256_or_si256(boundcheck_y1_min, boundcheck_y1_max);

    __m256i boundcheck00 = _mm256_or_si256(boundcheck_x0, boundcheck_y0);
    __m256i boundcheck01 = _mm256_or_si256(boundcheck_x0, boundcheck_y1);
    __m256i boundcheck10 = _mm256_or_si256(boundcheck_x1, boundcheck_y0);
    __m256i boundcheck11 = _mm256_or_si256(boundcheck_x1, boundcheck_y1);

    __m256i idx00 = _mm256_blendv_epi8(src_idx00, idx_default_value, boundcheck00);
    __m256i idx01 = _mm256_blendv_epi8(src_idx01, idx_default_value, boundcheck01);
    __m256i idx10 = _mm256_blendv_epi8(src_idx10, idx_default_value, boundcheck10);
    __m256i idx11 = _mm256_blendv_epi8(src_idx11, idx_default_value, boundcheck11);

    __m256 val00 = _mm256_i32gather_ps(source_image_ps, idx00, 4);
    __m256 val01 = _mm256_i32gather_ps(source_image_ps, idx01, 4);
    __m256 val10 = _mm256_i32gather_ps(source_image_ps, idx10, 4);
    __m256 val11 = _mm256_i32gather_ps(source_image_ps, idx11, 4);

    __m256 dx0 = _mm256_sub_ps(xi, x0);
    __m256 dy0 = _mm256_sub_ps(yi, y0);
    __m256 dx1 = _mm256_sub_ps(x1, xi);
    __m256 dy1 = _mm256_sub_ps(y1, yi);

    __m256 coef00 = _mm256_mul_ps(dx1, dy1);
    __m256 coef01 = _mm256_mul_ps(dx1, dy0);
    __m256 coef10 = _mm256_mul_ps(dx0, dy1);
    __m256 coef11 = _mm256_mul_ps(dx0, dy0);

    __m256 ans00 = _mm256_mul_ps(coef00, val00);
    __m256 ans01 = _mm256_mul_ps(coef01, val01);
    __m256 ans10 = _mm256_mul_ps(coef10, val10);
    __m256 ans11 = _mm256_mul_ps(coef11, val11);

    __m256 ans0 = _mm256_add_ps(ans00, ans01);
    __m256 ans1 = _mm256_add_ps(ans10, ans11);

    __m256 ans = _mm256_add_ps(ans0, ans1);

    return ans;
}


template <typename T>
ImgData<T> ImgAlgInterp<T>::_interpBilinear_Avx2(float* x_coords, float* y_coords, int target_width, int target_height)
{
    ImgData<T> result( target_width, target_height, this->range_);

    auto target_data_ptr = result.bits();
    auto source_data_ptr = this->bits();

    if(source_data_ptr==0)
    {
        memset(target_data_ptr, 0, target_width * target_height * sizeof(T));
        return result;
    }

    int source_img_size = this->width_ * this->height_;
    int target_img_size = target_width * target_height;
    int target_img_size_r8 = target_img_size / 8 * 8;

    // Cast image value to float (stupid but easy)
    float *source_image_ps = new float[source_img_size + 1];  // 末尾附加元素表示默认值
    float *target_image_ps = new float[target_img_size];


#pragma omp parallel for
    for(int i=0;i<source_img_size;i++)
    {
        source_image_ps[i]=source_data_ptr[i];
    }
    source_image_ps[source_img_size] = 0; // 默认值可自定义

    __m256i bound_x_max = _mm256_set1_epi32(this->width_ - 1);
    __m256i bound_y_max = _mm256_set1_epi32(this->height_ - 1);

    __m256i idx_default_value = _mm256_set1_epi32(source_img_size);

    int width = this->width_;

    __m256i widths = _mm256_set1_epi32(width);

#pragma omp parallel for
    for(int i=0;i<target_img_size_r8;i+=8)
    {
        __m256 xi = _mm256_loadu_ps(x_coords + i);
        __m256 yi = _mm256_loadu_ps(y_coords + i);

        __m256 ans = __8pxInterpBilinear_Avx2_ps(source_image_ps, xi, yi, widths, bound_x_max, bound_y_max, idx_default_value);

        _mm256_storeu_ps(target_image_ps + i, ans);
    }

    for(int i=target_img_size_r8;i<target_img_size;i++)
    {
        float x=x_coords[i];
        float y=y_coords[i];
        target_image_ps[i] = _pxInterpBilinear(x,y);
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
ImgData<T> ImgAlgInterp<T>::_interpBilinear_Cuda(float* x_coords, float* y_coords, int target_width, int target_height)
{
    ImgData<T> result( target_width, target_height, this->range_);

    auto target_data_ptr = result.bits();
    auto source_data_ptr = this->bits();

    _interpBilinear_Cuda_C(target_data_ptr, source_data_ptr, x_coords, y_coords, target_width, target_height,
                           this->width_, this->height_, 0);

    return result;
}

template <typename T>
void ImgAlgInterp<T>::_interpBilinear_Cuda_C(T *dest_ptr, T *src_ptr, float* x_coords, float* y_coords,  int dest_width, int dest_height, int src_width, int src_height, T default_value)
{
    // Since difference of ABI between MSVC(nvcc only support on Windows) and MinGW, we cannot dllexport & dllimport a Cpp style function, let alone template
    // Here's a very stupid substitution -_-b
    if(sizeof(T)==1)
    {
        __ImgAlgInterp_interpBilinear_cuda_epi8(reinterpret_cast<uint8_t*>(dest_ptr), reinterpret_cast<uint8_t*>(src_ptr),
                                                x_coords, y_coords, dest_width,dest_height,src_width,src_height,default_value);
    }
    else if(sizeof(T)==2)
    {
        __ImgAlgInterp_interpBilinear_cuda_epi16(reinterpret_cast<uint16_t*>(dest_ptr), reinterpret_cast<uint16_t*>(src_ptr),
                                                 x_coords, y_coords, dest_width,dest_height,src_width,src_height,default_value);
    }
    else
    {
        throw("Unsupported data type.");
    }
}

#endif // IMGALGINTERPOLATE_H
