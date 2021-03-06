#ifndef ImgConvert_H
#define ImgConvert_H

#include "ImgData.h"
#include "ImgAlgMapping.h"
#include <QImage>
#include <QFile>
#include <QDataStream>

template <typename T>
class ImgConvert : public virtual ImgData<T>, public virtual ImgAlgMapping<T>
{
public:
    ImgConvert();
    void fromQImage(const QImage &qimage);
    QImage toQImage();
    void fromRaw(const QString &filename);
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
    this->range_ = type_max;

    if (std::is_same_v<T, uint8_t>)
    {
        QImage tmp_qimage = qimage.convertToFormat(QImage::Format_Grayscale8);
        for (int i = 0; i < this->height_; i++)
            memcpy(this->data_ + i * this->width_, tmp_qimage.bits() + i * tmp_qimage.bytesPerLine(), sizeof(T) * this->width_);
    }
    else if (std::is_same_v<T, uint16_t>)
    {
        QImage tmp_qimage = qimage.convertToFormat(QImage::Format_Grayscale16);
        for (int i = 0; i < this->height_; i++)
            memcpy(this->data_ + i * this->width_, tmp_qimage.bits() + i * tmp_qimage.bytesPerLine(), sizeof(T) * this->width_);
    }
    else
    {
        throw("Unsupported ImgData Pixel Type!");
    }
}

template <typename T>
QImage ImgConvert<T>::toQImage()
{
    T type_max = (1ull << (8 * sizeof(T))) - 1;
    T range_max = this->range_;

    if (type_max == range_max)
    {
        if (std::is_same_v<T, uint8_t>)
        {
            T *tmp_data = new T[((this->width_ + 3) / 4 * 4) * this->height_];
            for (int i = 0; i < this->height_; i++)
                memcpy(tmp_data + i * ((this->width_ + 3) / 4 * 4), this->data_ + i * this->width_, sizeof(T) * this->width_);
            return QImage(
                reinterpret_cast<uchar *>(tmp_data), this->width_, this->height_, QImage::Format_Grayscale8, [](void *ptr)
                { delete[](uchar *) ptr; },
                tmp_data);
        }
        else if (std::is_same_v<T, uint16_t>)
        {
            T *tmp_data = new T[((this->width_ + 1) / 2 * 2) * this->height_];
            for (int i = 0; i < this->height_; i++)
                memcpy(tmp_data + i * ((this->width_ + 1) / 2 * 2), this->data_ + i * this->width_, sizeof(T) * this->width_);
            return QImage(
                reinterpret_cast<uchar *>(tmp_data), this->width_, this->height_, QImage::Format_Grayscale16, [](void *ptr)
                { delete[](uchar *) ptr; },
                tmp_data);
        }
        else
        {
            throw("Unsupported ImgData Pixel Type!");
        }
    }
    else
    {
        // ????????????????????????????????????
        T *tmp_data = new T[((this->width_ + 1) / 2 * 2) * this->height_];
        for (int i = 0; i < this->height_; i++)
        {
            for (int j = 0; j < this->width_; j++)
            {

                tmp_data[i * ((this->width_ + 1) / 2 * 2) + j] = 1.0f * (this->data_[i * this->width_ + j]) * type_max / range_max;
            }
        }
        QImage result(
            reinterpret_cast<uchar *>(tmp_data), this->width_, this->height_, QImage::Format_Grayscale16, [](void *ptr)
            { delete[](uchar *) ptr; },
            tmp_data);
        return result;
    }
}

template <typename T>
void ImgConvert<T>::fromRaw(const QString &filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    this->range_ = 4095;

    quint32 width;
    quint32 height;
    in.readRawData(reinterpret_cast<char *>(&width), 4);
    in.readRawData(reinterpret_cast<char *>(&height), 4);
    this->width_ = width;
    this->height_ = height;

    this->_allocate();

    in.readRawData(reinterpret_cast<char *>(this->data_), width * height * 2);

    for (int i = 0; i < width * height; i++)
    {
        this->data_[i] &= 0x0fff;
    }
}

#endif // IMGCONVERT_H
