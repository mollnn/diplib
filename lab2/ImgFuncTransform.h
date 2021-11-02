#ifndef IMGAFFINE_H
#define IMGAFFINE_H

#include "ImgAlgInterp.h"
#include "ImgAlgCopy.h"
#include "ImgAlgAffine.h"
#include "mathheader.h"
#include "TransformMatrix.h"
#include <cmath>

template <typename T>
class ImgFuncTransform : public virtual ImgAlgAffine<T>, public virtual ImgAlgCopy<T>
{
public:
    ImgFuncTransform() {}
public:
    ImgData<T> transformAffine(TransformMatrix transform, int target_width, int target_height);
    ImgData<T> transformAffine(TransformMatrix transform);

    ImgData<T> transformStd(float translate_x, float translate_y, float rotate_rad, float scale, float anchor_x, float anchor_y, int target_width, int target_height);
    ImgData<T> transformStd(float translate_x, float translate_y, float rotate_rad, float scale, float anchor_x, float anchor_y);
    ImgData<T> transformStd(float translate_x=0, float translate_y=0, float rotate_rad=0, float scale=1);

    ImgData<T> translate(float translate_x=0, float translate_y=0);
    ImgData<T> rotate(float rotate_rad, float anchor_x, float anchor_y);
    ImgData<T> rotate(float rotate_rad=0);
    ImgData<T> scale(float scale, float anchor_x, float anchor_y);
    ImgData<T> scale(float scale=1);

    ImgData<T> crop(float x0, float y0, int target_width, int target_height, float rotation = 0, float scale = 1);
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
ImgData<T> ImgFuncTransform<T>::transformStd(float translate_x, float translate_y, float rotate_rad, float scale, float anchor_x, float anchor_y, int target_width, int target_height)
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
ImgData<T> ImgFuncTransform<T>::transformStd(float translate_x, float translate_y, float rotate_rad, float scale, float anchor_x, float anchor_y)
{
    return transformStd(translate_x,translate_y,rotate_rad,scale,anchor_x,anchor_y);
}

template <typename T>
ImgData<T> ImgFuncTransform<T>::transformStd(float translate_x, float translate_y, float rotate_rad, float scale)
{
    return transformStd(translate_x,translate_y,rotate_rad,scale,this->width_*0.5f, this->height_*0.5f);
}

template <typename T>
ImgData<T> ImgFuncTransform<T>::translate(float translate_x, float translate_y)
{
    TransformMatrix transform;
    transform.translate(translate_x, translate_y);
    return this->_transformAffine(transform.to_mat3(), this->width_, this->height_);
}

template <typename T>
ImgData<T> ImgFuncTransform<T>::rotate(float rotate_rad, float anchor_x, float anchor_y)
{
    TransformMatrix transform;
    transform.translate(-anchor_x, -anchor_y);
    transform.rotate(rotate_rad);
    transform.translate(anchor_x, anchor_y);
    return this->_transformAffine(transform.to_mat3(), this->width_, this->height_);
}


template <typename T>
ImgData<T> ImgFuncTransform<T>::rotate(float rotate_rad)
{
    return this->rotate(rotate_rad, this->width_*0.5f, this->height_*0.5f);
}


template <typename T>
ImgData<T> ImgFuncTransform<T>::scale(float scale, float anchor_x, float anchor_y)
{
    TransformMatrix transform;
    transform.translate(-anchor_x, -anchor_y);
    transform.scale(scale);
    transform.translate(anchor_x, anchor_y);
    return this->_transformAffine(transform.to_mat3(), this->width_, this->height_);
}

template <typename T>
ImgData<T> ImgFuncTransform<T>::scale(float scale)
{
    return this->scale(scale, this->width_*0.5f, this->height_*0.5f);
}

template <typename T>
ImgData<T> ImgFuncTransform<T>::crop(float x0, float y0, int target_width, int target_height, float rotation, float scale)
{
    if(rotation==0 && scale==1)
    {
        return this->_cropFast(x0,y0,target_width,target_height);
    }
    else
    {
        return transformStd(-x0, -y0, rotation, scale, this->width_*0.5f, this->height_*0.5f, target_width, target_height);
    }
}

#endif // IMGAFFINE_H
