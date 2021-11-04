#include <bits/stdc++.h>
#include <immintrin.h>
#include <xmmintrin.h>

using namespace std;

__m256 __8pxInterpBilinear_Avx2(const float *image, const int width, const __m256 xi, const __m256 yi)
{
    // 计算连续的 8 个像素
    __m256 const1s = _mm256_set1_ps(1);
    __m256 x0 = _mm256_round_ps(xi, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
    __m256 y0 = _mm256_round_ps(yi, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
    __m256 x1 = _mm256_add_ps(x0, const1s);
    __m256 y1 = _mm256_add_ps(y0, const1s);

    cout << _mm256_cvtss_f32(xi) << ",";
    cout << _mm256_cvtss_f32(yi) << endl;
    cout << _mm256_cvtss_f32(x0) << ",";
    cout << _mm256_cvtss_f32(y0) << endl;

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
    return sum;
}

signed main()
{
    float img[30] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    __m256 xi = _mm256_set_ps(1, 2, 3, 4, 5, 6, 7, 8);
    __m256 yi = _mm256_set_ps(1, 1, 1, 1, 1, 1, 1, 1);
    int width = 10;
    __m256 ans = __8pxInterpBilinear_Avx2(img, width, xi, yi);
    cout << _mm256_cvtss_f32(ans) << endl;
}