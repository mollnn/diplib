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
    QImage toQImage();
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
    ImgData<T>::allocate();

    T type_max = (1ull << (8*sizeof(T))) - 1;
    T range_max = this->range_;

    if(type_max == range_max && sizeof(T)==1)
    {
        // type_max 与 range_max 相等且目标为标准 8bit 可转为 GRAY8 直接拷贝
        QImage qimage_grayscale8=qimage.convertToFormat(QImage::Format_Grayscale8);
        memcpy(ImgData<T>::data_,qimage_grayscale8.bits(),this->width_*this->height_);
    }
    else
    {
        // Qt 没有提供对 16bit 灰度图像支持，只能牺牲性能，从 RGBX64 枚举转换
        QImage qimage_rgbx64=qimage.convertToFormat(QImage::Format_RGBX64);
        T* qimage_data = reinterpret_cast<T*>(qimage_rgbx64.bits());
        for(int i=0;i<this->height_*this->width_;i++)
        {
            // 这里假设读入图像是灰度，直接提取任意颜色通道
            if(type_max==range_max)
            {
                // 直接拷贝
                this->data_[i]=qimage_data[(i<<2)+1];
            }
            else
            {
                // 位深度不相等，作灰度线性变换（小心溢出）
                this->data_[i]=1ull*qimage_data[(i<<2)+1]*(range_max+1)/(type_max+1);
            }
        }
    }
}

template <typename T>
QImage ImgConvert<T>::toQImage()
{
    T type_max = (1ull << (8*sizeof(T))) - 1;
    T range_max = this->range_;

    if(type_max==range_max && sizeof(T)==1)
    {
        // type_max 与 range_max 相等且目标为标准 8bit 可转为 GRAY8 直接拷贝
        return QImage(reinterpret_cast<const uint8_t*>(ImgData<T>::data_),ImgData<T>::width_,ImgData<T>::height_,QImage::Format_Grayscale8);
    }
    else
    {
        // Qt 没有提供对 16bit 灰度图像支持，从 RGBX64 转换

            // 只能牺牲性能，作灰度线性变换（小心溢出）
            T* tmp_data = new T[this->height_ * this->width_ * 4];
            if(tmp_data == nullptr)
            {
                throw("to QImage failed.");
            }
            for(int i=0;i<this->height_*this->width_;i++)
            {
                tmp_data[i*4+0] = 1ull*this->data_[i]*(type_max+1)/(range_max+1);
                tmp_data[i*4+1] = 1ull*this->data_[i]*(type_max+1)/(range_max+1);
                tmp_data[i*4+2] = 1ull*this->data_[i]*(type_max+1)/(range_max+1);
                tmp_data[i*4+3] = 1ull*this->data_[i]*(type_max+1)/(range_max+1);
            }
            QImage result(reinterpret_cast<const uchar*>(tmp_data),ImgData<T>::width_,ImgData<T>::height_,QImage::Format_RGBX64);
            result.bits();      // 强制深拷贝，因为临时空间即将释放
            result = result.copy();
            delete[] tmp_data;
            return result;
    }
}


#endif // IMGCONVERT_H
