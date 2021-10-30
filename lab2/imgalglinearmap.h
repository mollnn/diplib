#ifndef IMGALGLINEARMAPPER_H
#define IMGALGLINEARMAPPER_H

#include "imgdata.h"

template <typename T>
class ImgAlgLinearMap: public virtual ImgData<T>
{
public:
    ImgAlgLinearMap(){}
protected:
    template <typename R=T>
    static R pixelLinearMapSimple(T pixel, R target_range, T source_range)
    {
        // For example, 65535 -> 255:
        //  0->0, 255->0, 256->1, 511->1, ..., 65535->255
        return 1ull * pixel * (target_range + 1) / (source_range + 1);
    }

    template <typename R=T>
    static R pixelLinearMapRange(T pixel, R target_min, R target_max, T source_min, T source_max)
    {
        int delta = 1 * pixel - source_min;
        if(delta>source_max - source_min) delta=source_max-source_min;
        if(delta<0) delta=0;
        delta *= target_max - target_min + 1;
        delta /= source_max - source_min + 1;
        return delta + target_min;
    }
public:
    template <typename R=T>
    ImgData<R> linearMapSimple(R target_range, T source_range)
    {
        R type_max = (1ull << (8 * sizeof(R))) - 1;
        ImgData<R> result(this->width_, this->height_, type_max);
        for(int i=0;i<this->height_;i++)
        {
            for(int j=0;j<this->width_;j++)
            {
                result.setPixel(j,i,this->pixelLinearMapSimple(this->pixel(j,i),target_range,source_range));
            }
        }
        return result;
    }

    template <typename R=T>
    ImgData<R> linearMapRange(R target_min, R target_max, T source_min, T source_max)
    {
        R type_max = (1ull << (8 * sizeof(R))) - 1;
        ImgData<R> result(this->width_, this->height_, type_max);
        qDebug()<<"R"<<source_min<<source_max<<target_min<<target_max;
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
