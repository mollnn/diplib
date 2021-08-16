#ifndef IMGTHRESHOLDING_H
#define IMGTHRESHOLDING_H

#include "imgdata.h"

class ImgThresholding: public virtual ImgData
{
public:
    ImgThresholding();
    void thresholding(uint8_t threshold);
};

#endif // IMGTHRESHOLDING_H
