#include "imgthresholding.h"

ImgThresholding::ImgThresholding()
{

}


void ImgThresholding::thresholding(uint8_t threshold)
{
    for(int i=0;i<width*height;i++)
    {
        data[i]=data[i]>=threshold?255:0;
    }
}
