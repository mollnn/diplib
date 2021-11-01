#include "widget.h"
#include "img.h"
#include "qimageplotbox.h"
#include <QKeyEvent>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{

    img_plot_box_ = new QImagePlotBox(this);

    connect(img_plot_box_, &QImagePlotBox::dragSignal, [&](int dx, int dy)
    {
        this->view_offset_x_ -= dx;
        this->view_offset_y_ -= dy;
        this->_render();
    });

    connect(img_plot_box_, &QImagePlotBox::scrollSignal, [&](double delta)
    {
        if(is_ctrl_pressed_)
            this->view_rotation_ += 0.0005*delta;
        else this->view_scale_ *= 1 + 0.0005*delta;
        this->_render();
    });

    is_ctrl_pressed_=false;

    label_gray_window_breadth_ = new QLabel(this);
    label_gray_window_position_ = new QLabel(this);
    label_tip_ = new QLabel(this);

    label_tip_->setText("鼠标拖动图像以旋转，滚动滚轮以缩放，按下Ctrl滚动滚轮以旋转");

    slider_gray_window_breadth_ = new QSlider(Qt::Horizontal, this);
    slider_gray_window_position_ = new QSlider(Qt::Horizontal, this);

    spinbox_gray_window_breadth_ = new QSpinBox(this);
    spinbox_gray_window_position_ = new QSpinBox(this);

    label_gray_window_breadth_ -> setText("窗宽");
    label_gray_window_position_->setText("窗位");

    slider_gray_window_breadth_->setRange(0,0);
    slider_gray_window_position_->setRange(0,0);
    spinbox_gray_window_breadth_->setRange(0,0);
    spinbox_gray_window_position_->setRange(0,0);

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

    _bindValueEvents(&gray_window_breadth_, slider_gray_window_breadth_, spinbox_gray_window_breadth_);
    _bindValueEvents(&gray_window_position_, slider_gray_window_position_, spinbox_gray_window_position_);

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
        if (*value == spinbox->value() && *value == slider->value())
            return;
        *value = slider->value();
        spinbox->setValue(*value);
        this->_render();
    });
    connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged), [=]()
    {
        if (*value == spinbox->value() && *value == slider->value())
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

    Img<uint16_t> img_tmp = img_in_.crop((-view_width+src_width)/2+view_offset_x_,
                                         (-view_height+src_height)/2+view_offset_y_,
                                         view_width, view_height, view_rotation_, view_scale_);

    img_out_=img_tmp.applyGrayWindow<uint8_t>(gray_window_breadth_, gray_window_position_);
    img_plot_box_->imshow(img_out_.toQImage());
    this->_reLayout();
}

void Widget::_loadImage()
{
    try
    {
        img_in_.fromRaw(filename_in_);
        view_offset_x_=0;
        view_offset_y_=0;
        view_scale_=1;
        view_rotation_=0;

        slider_gray_window_breadth_->setRange(0,4096);
        spinbox_gray_window_breadth_->setRange(0,4096);
        slider_gray_window_position_->setRange(0,4095);
        spinbox_gray_window_position_->setRange(0,4095);

        gray_window_breadth_=4096;
        gray_window_position_=2048;

        slider_gray_window_breadth_->setValue(gray_window_breadth_);
        slider_gray_window_position_->setValue(gray_window_position_);

        spinbox_gray_window_breadth_->setValue(gray_window_breadth_);
        spinbox_gray_window_position_->setValue(gray_window_position_);

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
    grid_layout_->addWidget(img_plot_box_, 0, 0, 1, 7);
    grid_layout_->addWidget(label_gray_window_breadth_, 1, 0, 1, 1);
    grid_layout_->addWidget(slider_gray_window_breadth_, 1, 1, 1, 5);
    grid_layout_->addWidget(spinbox_gray_window_breadth_, 1, 6, 1, 1);
    grid_layout_->addWidget(label_gray_window_position_, 2, 0, 1, 1);
    grid_layout_->addWidget(slider_gray_window_position_, 2, 1, 1, 5);
    grid_layout_->addWidget(spinbox_gray_window_position_, 2, 6, 1, 1);
    grid_layout_->addWidget(button_open_, 3, 2, 1, 1);

    grid_layout_->addWidget(button_save_, 3, 4, 1, 1);
    grid_layout_->addWidget(label_tip_, 4, 0, 1, 7);
    setLayout(grid_layout_);
}

void Widget::resizeEvent(QResizeEvent * event)
{
    this->_reLayout();
    this->_render();
}


void Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Control)
    {
        is_ctrl_pressed_=true;
    }
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Control)
    {
        is_ctrl_pressed_=false;
    }
}
