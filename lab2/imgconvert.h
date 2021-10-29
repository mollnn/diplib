#ifndef IMGCONVERT_H
#define IMGCONVERT_H

#include "imgdata.h"
#include "imglinearmapper.h"
#include <QImage>

template <typename T>
class ImgConvert : public virtual ImgData<T>, public virtual ImgLinearMapper<T>
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
        if (sizeof(T) <= 2)
        {
            QImage tmp_qimage = qimage.convertToFormat(sizeof(T) == 1 ? QImage::Format_Grayscale8 : QImage::Format_Grayscale16);
            for (int i = 0; i < this->height_; i++)
                memcpy(this->data_ + i * this->width_, tmp_qimage.bits() + i * tmp_qimage.bytesPerLine(), sizeof(T) * this->width_);
        }
        else
            throw("Unsupported ImgData Pixel Type!");
    }
    else
    {
        QImage tmp_qimage = qimage.convertToFormat(QImage::Format_Grayscale16);
        T *qimage_data = reinterpret_cast<T *>(tmp_qimage.bits());
        for (int i = 0; i < this->height_; i++)
        {
            for (int j = 0; j < this->width_; j++)
            {
                this->data_[i * this->width_ + j] =  ImgLinearMapper<T>::pixelLinearMapSimple(qimage_data[i * ((this->width_ + 3) / 4 * 4) + j], range_max, type_max);
            }
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
        if (sizeof(T) <= 2)
        {
            T *tmp_data = new T[((this->width_ + 3) / 4 * 4) * this->height_];
            for (int i = 0; i < this->height_; i++)
                  memcpy(tmp_data + i * this->width_, this->data_ + i * this->width_, sizeof(T) * this->width_);
            return QImage(
                reinterpret_cast<uchar *>(tmp_data), this->width_, this->height_, sizeof(T) == 1 ? QImage::Format_Grayscale8 : QImage::Format_Grayscale16, [](void *ptr)
                { delete[](uchar *) ptr; },
                tmp_data);
        }
        else
            throw("Unsupported ImgData Pixel Type!");
    }
    else
    {
        T *tmp_data = new T[((this->width_ + 3) / 4 * 4) * this->height_];
        for (int i = 0; i < this->height_; i++)
        {
            for (int j = 0; j < this->width_; j++)
            {
                tmp_data[i * this->width_ + j] = ImgLinearMapper<T>::pixelLinearMapSimple(this->data_[i * this->width_ + j], type_max, range_max);
            }
        }
        QImage result(
            reinterpret_cast<uchar *>(tmp_data), this->width_, this->height_, QImage::Format_Grayscale16, [](void *ptr)
            { delete[](uchar *) ptr; },
            tmp_data);
        return result;
    }
}

#endif // IMGCONVERT_H
