#ifndef IMGAFFINE_H
#define IMGAFFINE_H

#include "ImgAlgInterpolate.h"
#include "ImgAlgCropFast.h"
#include "ImgAlgAffine.h"
#include "mathheader.h"
#include "TransformMatrix.h"
#include <cmath>

template <typename T>
class ImgFuncTransform : public virtual ImgAlgAffine<T>, public virtual ImgAlgCropFast<T>
{
public:
    ImgFuncTransform() {}
public:
    ImgData<T> transformAffine(TransformMatrix transform, int target_width, int target_height);
    ImgData<T> transformAffine(TransformMatrix transform);

    ImgData<T> transformStd(double translate_x, double translate_y, double rotate_rad, double scale, double anchor_x, double anchor_y, int target_width, int target_height);
    ImgData<T> transformStd(double translate_x, double translate_y, double rotate_rad, double scale, double anchor_x, double anchor_y);
    ImgData<T> transformStd(double translate_x=0, double translate_y=0, double rotate_rad=0, double scale=1);

    ImgData<T> crop(double x0, double y0, int target_width, int target_height, double rotation = 0, double scale = 1);
};

/////////////////////////////////////////////////////


template <typename T>
ImgData<T> ImgFuncTransform<T>::transformAffine(TransformMatrix transform, int target_width, int target_height)
{
    return this->_transformAffine(transform.to_mat3(), target_width, target_height);
}


template <typename T>
ImgData<T> ImgFuncTransform<T>::transformAffine(TransformMatrix transform)
{
    return this->_transformAffine(transform.to_mat3(), this->width_, this->height_);
}

template <typename T>
ImgData<T> ImgFuncTransform<T>::transformStd(double translate_x, double translate_y, double rotate_rad, double scale, double anchor_x, double anchor_y, int target_width, int target_height)
{
    // 顺序： scale, rotate, translate
    TransformMatrix transform;
    transform.translate(-anchor_x, -anchor_y);
    transform.rotate(rotate_rad);
    transform.scale(scale);
    transform.translate(anchor_x + translate_x, anchor_y + translate_y);
    return this->_transformAffine(transform.to_mat3(), target_width, target_height);
}

template <typename T>
ImgData<T> ImgFuncTransform<T>::transformStd(double translate_x, double translate_y, double rotate_rad, double scale, double anchor_x, double anchor_y)
{
    return transformStd(translate_x,translate_y,rotate_rad,scale,anchor_x,anchor_y);
}

template <typename T>
ImgData<T> ImgFuncTransform<T>::transformStd(double translate_x, double translate_y, double rotate_rad, double scale)
{
    return transformStd(translate_x,translate_y,rotate_rad,scale,this->width_/2,this->height_/2);
}

template <typename T>
ImgData<T> ImgFuncTransform<T>::crop(double x0, double y0, int target_width, int target_height, double rotation, double scale)
{
    if(rotation==0 && scale==1)
    {
        return this->_cropFast(x0,y0,target_width,target_height);
    }
    else
    {
        return transformStd(-x0, -y0, rotation, scale, this->width_/2, this->height_/2, target_width, target_height);
    }
}

#endif // IMGAFFINE_H
