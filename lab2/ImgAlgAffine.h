#ifndef IMGALGAFFINE_H
#define IMGALGAFFINE_H

#include "ImgAlgInterpolate.h"
#include "ImgAlgCropFast.h"
#include "mathheader.h"
#include <cmath>

template <typename T>
class ImgAlgAffine : public virtual ImgAlgInterpolate<T>
{
public:
    ImgAlgAffine() {}

protected:
    ImgData<T> _transformAffine(const mat3 &transform_matrix, int target_width, int target_height);
};


//////////////////////////////////////////////


template <typename T>
ImgData<T> ImgAlgAffine<T>::_transformAffine(const mat3 &transform_matrix, int target_width, int target_height)
{
    mat3 transform_matrix_inverse = transform_matrix.inverse();
    ImgData<T> result(target_width, target_height, this->range_);
#pragma omp parallel for
    for (int i = 0; i < result.height(); i++)
    {
        for (int j = 0; j < result.width(); j++)
        {
            vec3 target_pos = {1.0 * j, 1.0 * i, 1.0};
            vec3 source_pos = transform_matrix_inverse * target_pos;
            result.setPixel(j, i, this->_interpolateBilinear(source_pos.x, source_pos.y));
        }
    }
    return result;
}

#endif // IMGALGAFFINE_H
