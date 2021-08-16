#ifndef IMG_H
#define IMG_H

#include "imgconvert.h"
#include "imgthresholding.h"

class Img: public virtual ImgConvert, public virtual ImgThresholding
{
public:
    Img();
};

#endif // IMG_H
