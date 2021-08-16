#include "widget.h"
#include "ui_widget.h"

#include "img.h"
#include "imgwidget.h"

#include <QGridLayout>

#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Read
    Img img;
    img.fromQImage(QImage("in.jpg"));

    // Processing
    Img img2(img);
    img2.thresholding(128);

    // Save
    img2.toQImage().save("out.jpg");

    // Display
    ImgWidget* img_widget=new ImgWidget(this);
    img_widget->imshow(&img2);
    QGridLayout *grid_layout=new QGridLayout;
    grid_layout->addWidget(img_widget);
    setLayout(grid_layout);
}

Widget::~Widget()
{
    delete ui;
}

