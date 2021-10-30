#include "widget.h"
#include "img.h"
#include "imgplotbox.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{

    img_plot_box_ = new ImgPlotBox(this);

    connect(img_plot_box_, &ImgPlotBox::dragSignal, [&](int dx, int dy)
    {
        this->view_offset_x_ -= dx;
        this->view_offset_y_ -= dy;
        this->_render();
    });

    button_open_ = new QPushButton(this);
    button_open_->setText("打开文件");

    button_save_ = new QPushButton(this);
    button_save_->setText("保存文件");

    connect(button_open_, &QPushButton::clicked, [&]()
            {
                filename_in_ = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home/jana", tr("Raw Image Files (*.raw)"));
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
    int view_width = img_plot_box_->width();
    int view_height = img_plot_box_->height();
    int src_width = img_in_.width();
    int src_height = img_in_.height();

    img_out_=img_in_.crop((-view_width+src_width)/2+view_offset_x_,
                          (-view_height+src_height)/2+view_offset_y_,
                          view_width, view_height);
    img_plot_box_->imshow(img_out_.toQImage());
    this->_reLayout();
}

void Widget::_loadImage()
{
    try
    {
        img_in_.fromRaw(filename_in_);
        this->_render();
        this->_reLayout();
        view_offset_x_=0;
        view_offset_y_=0;
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
    grid_layout_->addWidget(img_plot_box_, 0, 0, 1, 2);
    grid_layout_->addWidget(button_open_, 1, 0, 1, 1);

    grid_layout_->addWidget(button_save_, 1, 1, 1, 1);
    setLayout(grid_layout_);
}

void Widget::resizeEvent(QResizeEvent * event)
{
    this->_reLayout();
    this->_render();
}
