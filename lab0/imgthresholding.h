#ifndef IMGTHRESHOLDING_H
#define IMGTHRESHOLDING_H

#include "imgdata.h"

template <typename T>
class ImgThresholding: public virtual ImgData<T>
{
public:
    ImgThresholding();
    void thresholding(uint8_t threshold);
};

template <typename T>
ImgThresholding<T>::ImgThresholding()
{

}

template <typename T>
void ImgThresholding<T>::thresholding(uint8_t threshold)
{
    for(int i=0;i<ImgData<T>::width*ImgData<T>::height;i++)
    {
        ImgData<T>::data[i]=ImgData<T>::data[i]>=threshold?255:0;
    }
}


#endif // IMGTHRESHOLDING_H
