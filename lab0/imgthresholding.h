#ifndef IMGTHRESHOLDING_H
#define IMGTHRESHOLDING_H

#include "imgdata.h"

template <typename T>
class ImgThresholding: public virtual ImgData<T>
{
public:
    ImgThresholding();
    void thresholding(T threshold);
};

template <typename T>
ImgThresholding<T>::ImgThresholding()
{

}

template <typename T>
void ImgThresholding<T>::thresholding(T threshold)
{
    for(int i=0;i<this->width_*this->height_;i++)
    {
        this->data_[i]=this->data_[i]>=threshold?this->range_:0;
    }
}


#endif // IMGTHRESHOLDING_H
