#ifndef IMGALGAFFINE_H
#define IMGALGAFFINE_H

#include "ImgAlgInterp.h"
#include "ImgAlgCopy.h"
#include "mathheader.h"
#include <cmath>


template <typename T>
class ImgAlgAffine : public virtual ImgAlgInterp<T>
{
public:
    ImgAlgAffine() {}

protected:
    ImgData<T> _transformAffine(const mat3 &transform_matrix, int target_width, int target_height);
    ImgData<T> _transformAffine_Baseline(const mat3 &transform_matrix, int target_width, int target_height);
    ImgData<T> _transformAffine_Fast(const mat3 &transform_matrix, int target_width, int target_height);
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
ImgData<T> ImgAlgAffine<T>::_transformAffine_Avx2(const mat3 &transform_matrix, int target_width, int target_height)
{
    mat3 transform_matrix_inverse = transform_matrix.inverse();

    float t11 = transform_matrix_inverse[0][0];
    float t12 = transform_matrix_inverse[0][1];
    float t13 = transform_matrix_inverse[0][2];

    float t21 = transform_matrix_inverse[1][0];
    float t22 = transform_matrix_inverse[1][1];
    float t23 = transform_matrix_inverse[1][2];

    float *coords = new float[target_width * target_height * 2];

#pragma omp parallel for
    for (int i = 0; i < target_height; i++)
    {
        for (int j = 0; j < target_width; j++)
        {
            float src_x = t11 * j + t12 * i + t13;
            float src_y = t21 * j + t22 * i + t23;
            coords[(i*target_width + j) * 2 + 0] = src_x;
            coords[(i*target_width + j) * 2 + 1] = src_y;
        }
    }
    auto result = this->_interpBilinear_Avx2(coords, target_width, target_height);

    delete[] coords;
    return result;
}



template <typename T>
ImgData<T> ImgAlgAffine<T>::_transformAffine(const mat3 &transform_matrix, int target_width, int target_height)
{
    return _transformAffine_Avx2(transform_matrix, target_width, target_height);
}

#endif // IMGALGAFFINE_H
