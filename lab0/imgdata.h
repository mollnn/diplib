#ifndef IMGDATA_H
#define IMGDATA_H

// 请勿在本类中使用任何平台相关类型


#include <stdint.h>
#include <string>
#include <QDebug>

template <typename T>
class ImgData
{
protected:
    // 所有子类必须保证 sizeof(data)=width*height，width,height 对用户是只读的
    int width;
    int height;
    uint8_t* data; // 约定：GRAY8，offset=(y*w+x)+0，不作对齐处理
    bool allocate();
    bool free();
public:
    ImgData();
    ImgData(int width_,int height_);
    ImgData(const ImgData& img);
    ImgData operator = (const ImgData& img);
    virtual ~ImgData();
    int getWidth();
    int getHeight();
    void setPixel(int x,int y,uint8_t c);
    uint8_t getPixel(int x,int y);
};


template <typename T>
bool ImgData<T>::allocate()
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

template <typename T>
bool ImgData<T>::free()
{
    if(data)
    {
        delete[] data;
        return true;
    }
    return false;
}

template <typename T>
ImgData<T>::ImgData(): width(0), height(0), data(nullptr)
{

}

template <typename T>
ImgData<T>::ImgData(int width_, int height_): width(width_), height(height_)
{
    allocate();
}

template <typename T>
ImgData<T>::ImgData(const ImgData& img): data(nullptr)
{
    width=img.width;
    height=img.height;
    allocate();
    memcpy(data,img.data,width*height);
}

template <typename T>
ImgData<T> ImgData<T>::operator=(const ImgData<T>& img)
{
    width=img.width;
    height=img.height;
    allocate();
    memcpy(data,img.data,width*height);
    return *this;
}

template <typename T>
ImgData<T>::~ImgData()
{
    free();
}

template <typename T>
int ImgData<T>::getWidth()
{
    return width;
}

template <typename T>
int ImgData<T>::getHeight()
{
    return height;
}

template <typename T>
void ImgData<T>::setPixel(int x, int y, uint8_t c)
{
    if(x<0 || x>=width || y<0 || y>height) throw("out of bound.");
    uint8_t* pixel=data+(y*width+x);
    pixel[0]=c;
}

template <typename T>
uint8_t ImgData<T>::getPixel(int x, int y)
{
    if(x<0 || x>=width || y<0 || y>height) throw("out of bound.");
    uint8_t* pixel=data+(y*width+x);
    return pixel[0];
}


#endif // IMGDATA_H
