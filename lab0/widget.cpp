#include "widget.h"
#include "ui_widget.h"

#include "img.h"
#include "imgplot.h"

#include <QGridLayout>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    Img<uint8_t> img_in;
    img_in.fromQImage(QImage("in.jpg"));

    Img<uint8_t> img_out(img_in);
    img_out.thresholding(128);

    img_out.toQImage().save("out.jpg");

    ImgPlot* img_plot_in=new ImgPlot(this);
    img_plot_in->imshow(img_in.toQImage());

    ImgPlot* img_plot_out=new ImgPlot(this);
    img_plot_out->imshow(img_out.toQImage());

    QGridLayout *grid_layout=new QGridLayout;
    grid_layout->addWidget(img_plot_in, 0, 0);
    grid_layout->addWidget(img_plot_out, 0, 1);
    setLayout(grid_layout);

    this->setWindowTitle("DIP Lab 0");
}

Widget::~Widget()
{
    delete ui;
}

