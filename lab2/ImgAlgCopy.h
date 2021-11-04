#ifndef IMGALGCROPFAST_H
#define IMGALGCROPFAST_H

#include "ImgData.h"
#include <cmath>
#include <QDebug>

template <typename T>
class ImgAlgCopy: public virtual ImgData<T>
{
public:
    ImgAlgCopy(){}
protected:
    ImgData<T> _copySubImg(int x0, int y0, int target_width, int target_height);
};

template <typename T>
ImgData<T> ImgAlgCopy<T>::_copySubImg(int x0, int y0, int target_width, int target_height)
{
    ImgData<T> result(target_width, target_height, this->range_);
    memset(result.bits(), 0, target_width*target_height*sizeof(T));

    int source_x_begin = x0;
    int source_x_end = x0 + target_width;
    int target_x_begin = 0;
    int target_x_end = target_width;

    if(source_x_begin<0)
    {
        target_x_begin-=source_x_begin;
        source_x_begin-=source_x_begin;
    }
    if(target_x_begin<0)
    {
        source_x_begin-=target_x_begin;
        target_x_begin-=target_x_begin;
    }
    if(source_x_end>this->width_)
    {
        int delta=source_x_end-this->width_;
        source_x_end-=delta;
        target_x_end-=delta;
    }
    if(target_x_end>target_width)
    {
        int delta=target_x_end-target_width;
        source_x_end-=delta;
        target_x_end-=delta;
    }
    if(source_x_end<0||target_x_end<0||source_x_begin>=this->width_||target_x_begin>=target_width)
    {
        return result;
    }

#pragma omp parallel for
    for(int i=0;i<target_height;i++)
    {
        int y1=i+y0;
        if(y1>=0 && y1<this->height_)
        {
            memcpy(result.bits()+i*target_width+target_x_begin,
                   this->data_+y1*this->width_+source_x_begin,
                   (source_x_end-source_x_begin)*sizeof(T));
        }
    }
    return result;
}

#endif // IMGALGCROPFAST_H
