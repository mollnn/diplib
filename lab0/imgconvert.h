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
    QImage qimage_grayscale8=qimage.convertToFormat(QImage::Format_Grayscale8);
    memcpy(ImgData<T>::data_,qimage_grayscale8.bits(),ImgData<T>::width_*ImgData<T>::height_);
}

template <typename T>
QImage ImgConvert<T>::toQImage()
{
    return QImage(const_cast<const uchar*>(ImgData<T>::data_),ImgData<T>::width_,ImgData<T>::height_,QImage::Format_Grayscale8);
}


#endif // IMGCONVERT_H
