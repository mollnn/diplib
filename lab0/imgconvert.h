#ifndef IMGCONVERT_H
#define IMGCONVERT_H

#include "imgdata.h"
#include <QImage>

template <typename T>
class ImgConvert: public virtual ImgData<T>
{
public:
    ImgConvert();
    void fromQImage(const QImage& qimage);
    QImage toQImage(bool flag_shared = 1); // flag_shared=1 允许 QImage 直接共享 this->data_，若需要灰度线性变换时该参数无效
};


template <typename T>
ImgConvert<T>::ImgConvert()
{

}

template <typename T>
void ImgConvert<T>::fromQImage(const QImage &qimage)
{
    ImgData<T>::width_=qimage.width();
    ImgData<T>::height_=qimage.height();
    ImgData<T>::_allocate();

    T type_max = (1ull << (8*sizeof(T))) - 1;
    T range_max = this->range_;

    if(type_max == range_max)
    {
        if(sizeof(T)==1)
        {
            // type_max 与 range_max 相等且目标为标准 8bit 可转为 GRAY8 直接拷贝
            QImage tmp_qimage=qimage.convertToFormat(QImage::Format_Grayscale8);
            memcpy(this->data_,tmp_qimage.bits(),sizeof(T)*this->width_*this->height_);
        }
        else if(sizeof(T)==2)
        {
            // type_max 与 range_max 相等且目标为标准 16bit 可转为 GRAY16 直接拷贝（>=Qt 5.13）
            QImage tmp_qimage=qimage.convertToFormat(QImage::Format_Grayscale16);
            memcpy(this->data_,tmp_qimage.bits(),sizeof(T)*this->width_*this->height_);
        }
        else throw("Unsupported ImgData Pixel Type!");
    }
    else
    {
        // 动态范围不匹配，只能牺牲性能，从 GRAY16 枚举转换
        QImage tmp_qimage=qimage.convertToFormat(QImage::Format_Grayscale16);
        T* qimage_data = reinterpret_cast<T*>(tmp_qimage.bits());
        for(int i=0;i<this->height_*this->width_;i++)
        {
            // 灰度线性变换（小心溢出）
            this->data_[i]=1ull*qimage_data[i]*(range_max+0)/(type_max+0);
        }
    }
}

template <typename T>
QImage ImgConvert<T>::toQImage(bool flag_shared)
{
    T type_max = (1ull << (8*sizeof(T))) - 1;
    T range_max = this->range_;

    if(type_max==range_max)
    {
        if(sizeof(T)==1)
        {
            // type_max 与 range_max 相等且目标为标准 8bit 可转为 GRAY8 直接拷贝
            if(!flag_shared)
            {
                T* tmp_data = new T[sizeof(T)*this->width_*this->height_];
                memcpy(tmp_data, this->data_, sizeof(T)*this->width_*this->height_);
                return QImage(reinterpret_cast<uchar*>(this->data_),this->width_,this->height_,QImage::Format_Grayscale8,[](void* ptr){delete[] (uchar*)ptr;},tmp_data);
            }
            else
            {
                return QImage(reinterpret_cast<uchar*>(this->data_),this->width_,this->height_,QImage::Format_Grayscale8);
            }
        }
        else if(sizeof(T)==2)
        {
            // type_max 与 range_max 相等且目标为标准 16bit 可转为 GRAY16 直接拷贝
            if(!flag_shared)
            {
                T* tmp_data = new T[sizeof(T)*this->width_*this->height_];
                memcpy(tmp_data, this->data_, sizeof(T)*this->width_*this->height_);
                return QImage(reinterpret_cast<uchar*>(this->data_),this->width_,this->height_,QImage::Format_Grayscale16,[](void* ptr){delete[] (uchar*)ptr;},tmp_data);
            }
            else
            {
                return QImage(reinterpret_cast<uchar*>(this->data_),this->width_,this->height_,QImage::Format_Grayscale16);
            }
        }
        else throw("Unsupported ImgData Pixel Type!");
    }
    else
    {
        // 灰度线性变换（小心溢出），从 GRAY16 转换
        T* tmp_data = new T[this->height_ * this->width_];
        if(tmp_data == nullptr)
        {
            throw("to QImage failed.");
        }
        for(int i=0;i<this->height_*this->width_;i++)
        {
            tmp_data[i] = 1ull*this->data_[i]*(type_max+0)/(range_max+0);
        }
        QImage result(reinterpret_cast<uchar*>(tmp_data),this->width_,this->height_,QImage::Format_Grayscale16,[](void* ptr){delete[] (uchar*)ptr;},tmp_data);
        return result;
    }
}


#endif // IMGCONVERT_H
