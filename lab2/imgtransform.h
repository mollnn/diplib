#ifndef IMGAFFINE_H
#define IMGAFFINE_H

#include "imgalginterpolate.h"
#include "imgalgcropfast.h"
#include "mathheader.h"
#include <cmath>

template <typename T>
class ImgTransform : public virtual ImgAlgInterpolate<T>, public virtual ImgAlgCropFast<T>
{
public:
    ImgTransform() {}
    ImgData<T> transformAffine(const mat3 &transform_matrix, int target_width, int target_height)
    {
        ImgData<T> result(target_width, target_height, this->range_);
        for (int i = 0; i < result.height(); i++)
        {
            for (int j = 0; j < result.width(); j++)
            {
                vec3 target_pos = {1.0 * j, 1.0 * i, 1.0};
                vec3 source_pos = transform_matrix * target_pos;
                result.setPixel(j, i, this->interpolateBilinear(source_pos.x, source_pos.y));
            }
        }
        return result;
    }

    ImgData<T> transformTRS(double translate_x, double translate_y, double rotate_rad, double scale, double anchor_x, double anchor_y, int target_width, int target_height)
    {
        // 顺序： scale, rotate, translate,
        translate_x *= -1;
        translate_y *= -1;
        rotate_rad *= -1;
        scale = 1.0 / (scale + 1e-12);
        mat3 transform_matrix = {{1, 0, anchor_x}, {0, 1, anchor_y}, {0, 0, 1}};
        transform_matrix *= (mat3){{scale, 0, 0}, {0, scale, 0}, {0, 0, 1}};
        transform_matrix *= (mat3){{cos(rotate_rad), -sin(rotate_rad), 0}, {sin(rotate_rad), cos(rotate_rad), 0}, {0, 0, 1}};
        transform_matrix *= (mat3){{1, 0, translate_x - anchor_x}, {0, 1, translate_y - anchor_y}, {0, 0, 1}};
        return transformAffine(transform_matrix, target_width, target_height);
    }

    ImgData<T> crop(double x0, double y0, int target_width, int target_height, double rotation = 0, double scale = 1)
    {
        return transformTRS(-x0, -y0, rotation, scale, this->width_/2, this->height_/2, target_width, target_height);
    }

    ImgData<T> cropFast(int x0, int y0, int target_width, int target_height)
    {
        return this->_cropFast(x0,y0,target_width,target_height);
    }

};

#endif // IMGAFFINE_H
