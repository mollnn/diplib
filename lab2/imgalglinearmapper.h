#ifndef IMGALGLINEARMAPPER_H
#define IMGALGLINEARMAPPER_H

#include "imgdata.h"

template <typename T>
class ImgAlgLinearMapper: public virtual ImgData<T>
{
public:
    ImgAlgLinearMapper(){}
protected:
    static T pixelLinearMapSimple(T pixel, T target_range, T source_range)
    {
        // For example, 65535 -> 255:
        //  0->0, 255->0, 256->1, 511->1, ..., 65535->255
        return 1ull * pixel * (target_range + 1) / (source_range + 1);
    }

    static T pixelLinearMapRange(T pixel, T target_min, T target_max, T source_min, T source_max)
    {
        int delta = 1 * pixel - source_min;
        if(delta>source_max - source_min) delta=source_max-source_min;
        if(delta<0) delta=0;
        delta *= target_max - target_min + 1;
        delta /= source_max - source_min + 1;
        return delta + target_min;
    }
public:
    ImgData<T> linearMapSimple(T target_range, T source_range)
    {
        ImgData<T> result(this->width_, this->height_, target_range);
        for(int i=0;i<this->height_;i++)
        {
            for(int j=0;j<this->width_;j++)
            {
                result.setPixel(j,i,this->pixelLinearMapSimple(this->pixel(j,i),target_range,source_range));
            }
        }
        return result;
    }

    ImgData<T> linearMapRange(T target_min, T target_max, T source_min, T source_max)
    {
        ImgData<T> result(this->width_, this->height_, target_max);
        for(int i=0;i<this->height_;i++)
        {
            for(int j=0;j<this->width_;j++)
            {
                result.setPixel(j,i,this->pixelLinearMapRange(this->pixel(j,i),target_min,target_max,source_min,source_max));
            }
        }
        return result;
    }
};

#endif // IMGALGLINEARMAPPER_H
