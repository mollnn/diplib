#include "imgdata.h"
#include <string>
#include <QDebug>

bool ImgData::allocate()
{
    if(data)
    {
        free();
    }
    data=new uint8_t[width*height];
    if(data)
    {
        return true;
    }
    return false;
}

bool ImgData::free()
{
    if(data)
    {
        delete[] data;
        return true;
    }
    return false;
}

ImgData::ImgData(): width(0), height(0), data(nullptr)
{

}

ImgData::ImgData(const ImgData& img): data(nullptr)
{
    width=img.width;
    height=img.height;
    allocate();
    memcpy(data,img.data,width*height);
}


ImgData ImgData::operator=(const ImgData& img)
{
    width=img.width;
    height=img.height;
    allocate();
    memcpy(data,img.data,width*height);
    return *this;
}

ImgData::~ImgData()
{
    free();
}

//void ImgData::setWidth(int _width)
//{
//    width=_width;
//}

//void ImgData::setHeight(int _height)
//{
//    height=_height;
//}

int ImgData::getWidth()
{
    return width;
}

int ImgData::getHeight()
{
    return height;
}

void ImgData::setPixel(int x, int y, uint8_t c)
{
    if(x<0 || x>=width || y<0 || y>height) throw("out of bound.");
    uint8_t* pixel=data+(y*width+x);
    pixel[0]=c;
}

uint8_t ImgData::getPixel(int x, int y)
{
    if(x<0 || x>=width || y<0 || y>height) throw("out of bound.");
    uint8_t* pixel=data+(y*width+x);
    return pixel[0];
}
