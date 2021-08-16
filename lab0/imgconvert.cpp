#include "imgconvert.h"

ImgConvert::ImgConvert()
{

}

void ImgConvert::fromQImage(const QImage &qimage)
{
    width=qimage.width();
    height=qimage.height();
    allocate();
    QImage qimage_grayscale8=qimage.convertToFormat(QImage::Format_Grayscale8);
    memcpy(data,qimage_grayscale8.bits(),width*height);
}

QImage ImgConvert::toQImage()
{
    return QImage(const_cast<const uchar*>(data),width,height,QImage::Format_Grayscale8);
}
