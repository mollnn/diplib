#include "imgwidget.h"
#include <QDebug>
#include <QLabel>

ImgWidget::ImgWidget(QWidget *parent) : QWidget(parent)
{
    label=new QLabel(this);
}


void ImgWidget::imshow(ImgConvert* img_convert)
{
    label->setPixmap(QPixmap::fromImage(img_convert->toQImage().copy()));
}
