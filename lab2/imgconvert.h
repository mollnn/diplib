#ifndef IMGCONVERT_H
#define IMGCONVERT_H

#include "imgdata.h"
#include <QImage>

template <typename T>
class ImgConvert : public virtual ImgData<T>
{
public:
    ImgConvert();
    void fromQImage(const QImage &qimage);
    QImage toQImage();
};

template <typename T>
ImgConvert<T>::ImgConvert()
{
}

template <typename T>
void ImgConvert<T>::fromQImage(const QImage &qimage)
{
    this->width_ = qimage.width();
    this->height_ = qimage.height();
    this->_allocate();

    T type_max = (1ull << (8 * sizeof(T))) - 1;
    T range_max = this->range_;

    if (type_max == range_max)
    {
        // 注意 QImage 存储时相当于将 width 上取整为 4 的倍数，因此需要逐行复制
        if (sizeof(T) == 1)
        {
            // type_max 与 range_max 相等且目标为标准 8bit 可转为 GRAY8 直接拷贝
            QImage tmp_qimage = qimage.convertToFormat(QImage::Format_Grayscale8);
            for (int i = 0; i < this->height_; i++)
                memcpy(this->data_ + i * this->width_, tmp_qimage.bits() + i * tmp_qimage.bytesPerLine(), sizeof(T) * this->width_);
        }
        else if (sizeof(T) == 2)
        {
            // type_max 与 range_max 相等且目标为标准 16bit 可转为 GRAY16 直接拷贝（>=Qt 5.13）
            QImage tmp_qimage = qimage.convertToFormat(QImage::Format_Grayscale16);
            for (int i = 0; i < this->height_; i++)
                memcpy(this->data_ + i * this->width_, tmp_qimage.bits() + i * tmp_qimage.bytesPerLine(), sizeof(T) * this->width_);
        }
        else
            throw("Unsupported ImgData Pixel Type!");
    }
    else
    {
        // 动态范围不匹配，只能牺牲性能，从 GRAY16 枚举转换
        QImage tmp_qimage = qimage.convertToFormat(QImage::Format_Grayscale16);
        T *qimage_data = reinterpret_cast<T *>(tmp_qimage.bits());
        for (int i = 0; i < this->height_; i++)
            for (int j = 0; j < this->width_; j++)
            {
                // 灰度线性变换（小心溢出）
                this->data_[i * this->width_ + j] = 1ull * qimage_data[i * ((this->width_ + 3) / 4 * 4) + j] * (range_max + 0) / (type_max + 0);
            }
    }
}

template <typename T>
QImage ImgConvert<T>::toQImage()
{
    T type_max = (1ull << (8 * sizeof(T))) - 1;
    T range_max = this->range_;

    if (type_max == range_max)
    {
        // 注意 QImage 存储时相当于将 width 上取整为 4 的倍数，因此需要逐行复制
        if (sizeof(T) == 1)
        {
            // type_max 与 range_max 相等且目标为标准 8bit 可转为 GRAY8 直接拷贝
            T *tmp_data = new T[((this->width_ + 3) / 4 * 4) * this->height_];
            for (int i = 0; i < this->height_; i++)
                memcpy(tmp_data + i * ((this->width_ + 3) / 4 * 4), this->data_ + i * this->width_, sizeof(T) * this->width_);
            return QImage(
                reinterpret_cast<uchar *>(tmp_data), this->width_, this->height_, QImage::Format_Grayscale8, [](void *ptr)
                { delete[](uchar *) ptr; },
                tmp_data);
        }
        else if (sizeof(T) == 2)
        {
            // type_max 与 range_max 相等且目标为标准 16bit 可转为 GRAY16 直接拷贝
            T *tmp_data = new T[((this->width_ + 3) / 4 * 4) * this->height_];
            for (int i = 0; i < this->height_; i++)
                memcpy(tmp_data + i * ((this->width_ + 3) / 4 * 4), this->data_ + i * this->width_, sizeof(T) * this->width_);
            return QImage(
                reinterpret_cast<uchar *>(tmp_data), this->width_, this->height_, QImage::Format_Grayscale16, [](void *ptr)
                { delete[](uchar *) ptr; },
                tmp_data);
        }
        else
            throw("Unsupported ImgData Pixel Type!");
    }
    else
    {
        // 灰度线性变换（小心溢出），从 GRAY16 转换
        T *tmp_data = new T[((this->width_ + 3) / 4 * 4) * this->height_];
        if (tmp_data == nullptr)
        {
            throw("to QImage failed.");
        }
        for (int i = 0; i < this->height_; i++)
            for (int j = 0; j < this->width_; j++)
            {
                tmp_data[i * ((this->width_ + 3) / 4 * 4) + j] = 1ull * this->data_[i * this->width_ + j] * (type_max + 0) / (range_max + 0);
            }
        QImage result(
            reinterpret_cast<uchar *>(tmp_data), this->width_, this->height_, QImage::Format_Grayscale16, [](void *ptr)
            { delete[](uchar *) ptr; },
            tmp_data);
        return result;
    }
}

#endif // IMGCONVERT_H
