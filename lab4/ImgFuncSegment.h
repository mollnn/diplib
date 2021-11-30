#ifndef IMGFUNCSEGMENT_H
#define IMGFUNCSEGMENT_H

#include <ImgAlgSegment.h>

template <typename T>
class ImgFuncSegment : public virtual ImgAlgSegment<T>
{
public:
    ImgFuncSegment();
    ImgFuncSegment(const ImgData<T> &img);
    ImgFuncSegment(ImgData<T> &&img);

    ImgData<T> otsuThresholding();
};


template <typename T>
ImgFuncSegment<T>::ImgFuncSegment()
{
}

template <typename T>
ImgFuncSegment<T>::ImgFuncSegment(const ImgData<T> &img) : ImgData<T>(img)
{
}

template <typename T>
ImgFuncSegment<T>::ImgFuncSegment(ImgData<T> &&img) : ImgData<T>(img)
{
}


template <typename T>
ImgData<T> ImgFuncSegment<T>::otsuThresholding()
{
    T t = this->getThresholdOtsu();
    return this->threshold(t);
}

#endif // IMGFUNCSEGMENT_H
