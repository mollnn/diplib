#include "QImagePlot.h"
#include <QDebug>
#include <QLabel>

QImagePlot::QImagePlot(QWidget *parent) : QWidget(parent)
{
    label_ = new QLabel(this);
}

void QImagePlot::imshow(const QImage &img)
{
    label_->setFixedSize(img.size());
    label_->resize(img.size());
    label_->setPixmap(QPixmap::fromImage(img.copy()));
    this->setFixedSize(img.size());
    this->resize(img.size());
}
