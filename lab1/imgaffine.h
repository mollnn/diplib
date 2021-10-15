#ifndef IMGAFFINE_H
#define IMGAFFINE_H

#include "ImgInterpolate.h"
#include "vec3.h"
#include <cmath>

template <typename T>
class ImgAffine: public virtual ImgInterpolate<T>
{
public:
    ImgAffine() {}
    ImgData<T> transformAffine(const mat3& transform_matrix)
    {
        ImgData<T> result(this->width_, this->height_, this->range_);
        for(int i=0;i<result.height();i++)
        {
            for(int j=0;j<result.width();j++)
            {
                vec3 target_pos = {1.0*j,1.0*i,1.0};
                vec3 source_pos = transform_matrix*target_pos;
                result.setPixel(j,i,this->interpolateBilinear(source_pos.x, source_pos.y));
            }
        }
        return result;
    }

    ImgData<T> transformTRS(double translate_x, double translate_y, double rotate_rad, double scale, double anchor_x, double anchor_y)
    {
        // 顺序： scale, rotate, translate,
        translate_x*=-1;
        translate_y*=-1;
        rotate_rad*=-1;
        scale=1.0/(scale+1e-12);
        mat3 transform_matrix = {{1, 0, anchor_x}, {0, 1, anchor_y}, {0,0,1}};
        transform_matrix*=(mat3){{scale, 0, 0}, {0, scale, 0}, {0,0,1}};
        transform_matrix*=(mat3){{cos(rotate_rad), -sin(rotate_rad), 0},{sin(rotate_rad), cos(rotate_rad), 0}, {0,0,1}};
        transform_matrix*=(mat3){{1,0,translate_x-anchor_x},{0,1,translate_y-anchor_y},{0,0,1}};
        return transformAffine(transform_matrix);
    }
};


#endif // IMGAFFINE_H
