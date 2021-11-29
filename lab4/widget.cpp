#include "widget.h"
#include "Img.h"
#include "QImagePlotBox.h"
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
                if (is_ctrl_pressed_)
                    this->view_rotation_ += 0.0005 * delta;
                else
                    this->view_scale_ *= 1 + 0.0005 * delta;
                this->_render();
            });

    is_ctrl_pressed_ = false;

    label_smooth_sigma_ = new QLabel(this);

    label_tip_ = new QLabel(this);

    label_tip_->setText("按住鼠标左键拖拽图像以平移，滚动鼠标滚轮以缩放，按住Ctrl滚动滚轮以旋转");

    slider_smooth_sigma_ = new QSlider(Qt::Horizontal, this);

    spinbox_smooth_sigma_ = new QSpinBox(this);

    label_smooth_sigma_->setText("高斯平滑 σ / 0.001");

    this->smooth_sigma_ = 10;

    spinbox_smooth_sigma_->setRange(1, 200);

    button_open_ = new QPushButton(this);
    button_open_->setText("打开文件");

    filename_in_ = "D:/seed.bmp";

    connect(button_open_, &QPushButton::clicked, [&]()
            {
                filename_in_ = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home/jana", tr("BMP Image Files (*.bmp)"));
                this->_loadImage();
            });

    _bindValueEvents(&smooth_sigma_, slider_smooth_sigma_, spinbox_smooth_sigma_);

    grid_layout_ = new QGridLayout;

    this->_reLayout();

    this->_loadImage();

    this->setWindowTitle("DIP Lab 3");
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

    // 就应用而言，可以缓存中间结果
    // 这里为了测试处理流程的整体性能，每次重新执行整个计算流程

    Img<float> img_in_ps = img_in_.cast<float>();
    Img<float> img_feature = img_in_ps.backgroundCorrection(smooth_sigma_ * 0.001, false);
    float t = img_feature.getThresholdMaxEntropy();
    qDebug() << t;
    img_out_ = img_feature.threshold(t).cast<uint8_t>();
    img_render_ = img_out_.crop((-view_width + src_width) / 2 + view_offset_x_,
                                (-view_height + src_height) / 2 + view_offset_y_,
                                view_width, view_height, view_rotation_, view_scale_);

    // img_out_ = img_usm.cast<uint8_t>(255.0 / 4096, 0);

    img_plot_box_->imshow(img_render_.toQImage());
    this->_reLayout();
}

void Widget::_loadImage()
{
    try
    {
        img_in_.fromQImage(QImage(filename_in_));
        view_offset_x_ = 0;
        view_offset_y_ = 0;
        view_scale_ = 1;
        view_rotation_ = 0;

        smooth_sigma_ = 10;

        slider_smooth_sigma_->setValue(smooth_sigma_);
        spinbox_smooth_sigma_->setValue(smooth_sigma_);

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

    grid_layout_->addWidget(label_smooth_sigma_, 5, 0, 1, 1);
    grid_layout_->addWidget(slider_smooth_sigma_, 5, 1, 1, 5);
    grid_layout_->addWidget(spinbox_smooth_sigma_, 5, 6, 1, 1);

    grid_layout_->addWidget(button_open_, 6, 3, 1, 1);
    grid_layout_->addWidget(label_tip_, 7, 0, 1, 7);

    setLayout(grid_layout_);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    this->_reLayout();
    this->_render();
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        is_ctrl_pressed_ = true;
    }
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        is_ctrl_pressed_ = false;
    }
}
