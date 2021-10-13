#ifndef IMG_H
#define IMG_H

#include "imgconvert.h"
#include "imgthresholding.h"

template <typename T>
class Img: public virtual ImgConvert<T>, public virtual ImgThresholding<T>
{
public:
    Img();
};


template <typename T>
Img<T>::Img()
{

}


#endif // IMG_H
