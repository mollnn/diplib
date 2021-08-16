#ifndef IMGWIDGET_H
#define IMGWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include "imgconvert.h"

class ImgWidget : public QWidget
{
    Q_OBJECT
private:
    QImage qimage;
    QLabel* label;
public:
    explicit ImgWidget(QWidget *parent = nullptr);
    void imshow(ImgConvert* img_convert);
signals:

};

#endif // IMGWIDGET_H
