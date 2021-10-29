#include "widget.h"
#include "img.h"
#include "imgplot.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{

    img_plot_in_ = new ImgPlot(this);
    img_plot_out_ = new ImgPlot(this);

    button_open_ = new QPushButton(this);
    button_open_->setText("打开文件");

    button_save_ = new QPushButton(this);
    button_save_->setText("保存文件");

    connect(button_open_, &QPushButton::clicked, [&]()
            {
                filename_in_ = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home/jana", tr("Image Files (*.bmp *.jpg *.gif *.tif *.png)"));
                this->_loadImage();
            });

    connect(button_save_, &QPushButton::clicked, [&]()
            {
                filename_out_ = QFileDialog::getSaveFileName(this, tr("Save Image"), "/home/jana", tr("Image Files (*.bmp *.jpg *.gif *.tif *.png)"));
                this->_saveImage();
            });

    grid_layout_ = new QGridLayout;
    this->_reLayout();

    this->setWindowTitle("DIP Lab 2");
}

Widget::~Widget()
{
}

void Widget::_bindValueEvents(int *value, QSlider *slider, QSpinBox *spinbox)
{
    connect(slider, &QSlider::valueChanged, [=]()
            {
                if (*value == slider->value())
                    return;
                *value = slider->value();
                spinbox->setValue(*value);
                this->_render();
            });
    connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged), [=]()
            {
                if (*value == spinbox->value())
                    return;
                *value = spinbox->value();
                slider->setValue(*value);
                this->_render();
            });
}

void Widget::_render()
{
    img_plot_in_->imshow(img_in_.toQImage());
    img_plot_out_->imshow(img_out_.toQImage());
    this->_reLayout();
}

void Widget::_loadImage()
{
    try
    {
        QImage tmp = QImage(filename_in_);
        if (tmp.sizeInBytes() == 0)
        {
            qDebug() << "fail to load or empty image.";
            return;
        }
        else
        {
            img_in_.setRange(65535);
            img_in_.fromQImage(tmp);
        }
        img_out_ = img_in_;
        img_out_ = img_out_.linearMapSimple(65535, 255);

        this->_render();
        this->_reLayout();
    }
    catch (...)
    {
        qDebug() << "error to load image.";
    }
}

void Widget::_saveImage()
{
    try
    {
        img_out_.toQImage().save(filename_out_);
    }
    catch (...)
    {
        qDebug() << "fail to load image.";
    }
}

void Widget::_reLayout()
{
    grid_layout_->addWidget(img_plot_in_, 0, 0, 1, 3);
    grid_layout_->addWidget(img_plot_out_, 0, 3, 1, 3);
    grid_layout_->addWidget(button_open_, 7, 1, 1, 1);
    grid_layout_->addWidget(button_save_, 7, 4, 1, 1);
    setLayout(grid_layout_);
}
