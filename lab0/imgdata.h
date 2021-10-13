#ifndef IMGDATA_H
#define IMGDATA_H

// 请勿在本类中使用任何平台相关类型
#include <QDebug>

template <typename T>
class ImgData
{
protected:
    // 所有子类必须保证 sizeof(data)=width*height，width,height 对用户是只读的
    int width_;
    int height_;
    T range_; // value in [0, range) 简单起见仅考虑整数
    T* data_;
    bool allocate();
    bool free();
public:
    ImgData();
    ImgData(int width_,int height_,T range_=256);
    ImgData(const ImgData& img);
    ImgData operator = (const ImgData& img);
    virtual ~ImgData();
    int width();
    int height();
    void setPixel(int x,int y,T c);
    T pixel(int x,int y);
    void setRange(T range);
    T range();
};


template <typename T>
bool ImgData<T>::allocate()
{
    if(data_)
    {
        free();
    }
    data_=new T[width_*height_];
    if(data_)
    {
        return true;
    }
    return false;
}

template <typename T>
bool ImgData<T>::free()
{
    if(data_)
    {
        delete[] data_;
        return true;
    }
    return false;
}

template <typename T>
ImgData<T>::ImgData(): width_(0), height_(0), range_(256), data_(nullptr)
{

}

template <typename T>
ImgData<T>::ImgData(int width_, int height_, T range_): width_(width_), height_(height_), range_(range_)
{
    allocate();
}

template <typename T>
ImgData<T>::ImgData(const ImgData& img): data_(nullptr)
{
    width_=img.width_;
    height_=img.height_;
    range_=img.range_;
    allocate();
    memcpy(data_,img.data_,width_*height_);
}

template <typename T>
ImgData<T> ImgData<T>::operator=(const ImgData<T>& img)
{
    width_=img.width_;
    height_=img.height_;
    range_=img.range_;
    allocate();
    memcpy(data_,img.data_,width_*height_);
    return *this;
}

template <typename T>
ImgData<T>::~ImgData()
{
    free();
}

template <typename T>
int ImgData<T>::width()
{
    return width_;
}

template <typename T>
int ImgData<T>::height()
{
    return height_;
}

template <typename T>
void ImgData<T>::setPixel(int x, int y, T c)
{
    if(x<0 || x>=width_ || y<0 || y>height_) throw("out of bound.");
    T* pixel=data_+(y*width_+x);
    pixel[0]=c;
}

template <typename T>
T ImgData<T>::pixel(int x, int y)
{
    if(x<0 || x>=width_ || y<0 || y>height_) throw("out of bound.");
    T* pixel=data_+(y*width_+x);
    return pixel[0];
}

template <typename T>
void ImgData<T>::setRange(T range)
{
    range_=range;
}

template <typename T>
T ImgData<T>::range()
{
    return range_;
}


#endif // IMGDATA_H
