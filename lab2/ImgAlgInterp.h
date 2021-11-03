#ifndef IMGALGINTERPOLATE_H
#define IMGALGINTERPOLATE_H

#include "ImgData.h"
#include <cmath>

#include <immintrin.h>
#include <xmmintrin.h>
#include <avx2intrin.h>



template <typename T>
class ImgAlgInterp : public virtual ImgData<T>
{
public:
    ImgAlgInterp() {}
protected:
    T _pxInterpBilinear(float x, float y);
    ImgData<T> _interpBilinear(float* coords, int target_width, int target_height);
    ImgData<T> _interpBilinear_Baseline(float* coords, int target_width, int target_height);
    ImgData<T> _interpBilinear_Avx2(float* coords, int target_width, int target_height);
private:
//    static __m256 __8pxInterpBilinear_Avx2(const float* image, const int image_width, const __m256 xi, const __m256 yi);
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
ImgData<T> ImgAlgInterp<T>::_interpBilinear(float* coords, int target_width, int target_height)
{
    return _interpBilinear_Avx2(coords, target_width, target_height);
}


template <typename T>
ImgData<T> ImgAlgInterp<T>::_interpBilinear_Baseline(float* coords, int target_width, int target_height)
{
    ImgData<T> result( target_width, target_height, this->range_);
    auto target_data_ptr = result.bits();
    for(int i=0;i<target_height;i++)
    {
        for(int j=0;j<target_width;j++)
        {
            int idx = i*target_width+j;
            target_data_ptr[idx] = this->_pxInterpBilinear(coords[2*idx+0], coords[2*idx+1]);
        }
    }
    return result;
}

//template <typename T>
//__m256 ImgAlgInterp<T>::__8pxInterpBilinear_Avx2(const float* image, const int width, const __m256 xi, const __m256 yi)
//{
//    // 计算连续的 8 个像素

//    return sum;
//}

template <typename T>
ImgData<T> ImgAlgInterp<T>::_interpBilinear_Avx2(float* coords, int target_width, int target_height)
{
    ImgData<T> result( target_width, target_height, this->range_);

    auto target_data_ptr = result.bits();
    auto source_data_ptr = this->bits();

    if(source_data_ptr==0) return result;

    // assume sizeof(T)=2
    int source_img_size = this->width_ * this->height_;
    int target_img_size = target_width * target_height;
    int target_img_size_r8 = target_img_size / 8 * 8;

    // Cast image value to float
    float *source_image_ps = new float[source_img_size * 2];
    float *target_image_ps = new float[target_img_size * 2];
    for(int i=0;i<source_img_size;i++)
    {
        source_image_ps[i]=source_data_ptr[i];
    }

    __m256i coords_index_offsets_x = _mm256_set_epi32(14, 12, 10, 8, 6, 4, 2, 0);
    __m256i coords_index_offsets_y = _mm256_set_epi32(15, 13, 11, 9, 7, 5, 3, 1);

    for(int i=0;i<target_img_size_r8;i+=8)
    {
        int flag = 0;
        for(int j=i; j<i+8; j++)
        {
            float x= coords[j*2+0];
            float y=coords[j*2+1];
            if(x>2 && x+2<this->width_ && y>2 && y+2<this->height_) continue;
            flag = 1;
            break;
        }

        if(flag)
        {
            for(int j=i;j<i+8;j++)
            {
                float x= coords[j*2+0];
                float y=coords[j*2+1];
                target_data_ptr[j]=_pxInterpBilinear(x,y);
            }
            continue;
        }
        __m256i coords_index_base = _mm256_set1_epi32(2*i);
        __m256i coords_index_x = _mm256_add_epi32(coords_index_base, coords_index_offsets_x);
        __m256i coords_index_y = _mm256_add_epi32(coords_index_base, coords_index_offsets_y);
        __m256 xi = _mm256_i32gather_ps(coords, coords_index_x, 4);
        __m256 yi = _mm256_i32gather_ps(coords, coords_index_y, 4);
        int width = this->width_;
        float* image = source_image_ps;
        __m256 ans;
        {
            __m256 const1s = _mm256_set1_ps(1);
            __m256 x0 = _mm256_round_ps(xi, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
            __m256 y0 = _mm256_round_ps(yi, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
            __m256 x1 = _mm256_add_ps(x0, const1s);
            __m256 y1 = _mm256_add_ps(y0, const1s);

            __m256i x0_i32 = _mm256_cvtps_epi32(x0);
            __m256i y0_i32 = _mm256_cvtps_epi32(y0);
            __m256i x1_i32 = _mm256_cvtps_epi32(x1);
            __m256i y1_i32 = _mm256_cvtps_epi32(y1);

            __m256i widths = _mm256_set1_epi32(width);
            __m256i y0_start = _mm256_mullo_epi32(widths, y0_i32);
            __m256i y1_start = _mm256_mullo_epi32(widths, y1_i32);

            __m256i idx00 = _mm256_add_epi32(y0_start, x0_i32);
            __m256i idx01 = _mm256_add_epi32(y1_start, x0_i32);
            __m256i idx10 = _mm256_add_epi32(y0_start, x1_i32);
            __m256i idx11 = _mm256_add_epi32(y1_start, x1_i32);

            __m256 val00 = _mm256_i32gather_ps(image, idx00, 4);
            __m256 val01 = _mm256_i32gather_ps(image, idx01, 4);
            __m256 val10 = _mm256_i32gather_ps(image, idx10, 4);
            __m256 val11 = _mm256_i32gather_ps(image, idx11, 4);


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

            __m256 sum0 = _mm256_add_ps(ans00, ans01);
            __m256 sum1 = _mm256_add_ps(ans10, ans11);

            __m256 sum = _mm256_add_ps(sum0, sum1);
            ans = sum;
        }
        _mm256_storeu_ps(target_image_ps + i, ans);
    }

    for(int i=0;i<target_img_size;i++)
    {
        target_data_ptr[i]=target_image_ps[i];
    }

    // Cast image value back to int

    delete[] source_image_ps;
    delete[] target_image_ps;
    return result;
}


#endif // IMGALGINTERPOLATE_H
