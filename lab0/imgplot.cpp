#include "imgplot.h"
#include <QDebug>
#include <QLabel>

ImgPlot::ImgPlot(QWidget *parent) : QWidget(parent)
{
    label=new QLabel(this);
}

void ImgPlot::imshow(const QImage& img)
{
    label->setPixmap(QPixmap::fromImage(img.copy()));
}
