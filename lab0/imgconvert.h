#ifndef IMGCONVERT_H
#define IMGCONVERT_H

#include "imgdata.h"
#include <QImage>

class ImgConvert: public virtual ImgData
{
public:
    ImgConvert();
    void fromQImage(const QImage& qimage);
    QImage toQImage();
};

#endif // IMGCONVERT_H
