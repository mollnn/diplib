#include "widget.h"
#include "img.h"
#include "imgplot.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{

    img_plot_in_=new ImgPlot(this);
    img_plot_out_=new ImgPlot(this);

    label_anchor_x_=new QLabel(this);
    label_anchor_x_->setText("锚点X/px");
    label_anchor_y_=new QLabel(this);
    label_anchor_y_->setText("锚点Y/px");
    label_prescale_=new QLabel(this);
    label_prescale_->setText("缩放/%");
    label_translate_x_=new QLabel(this);
    label_translate_x_->setText("平移X/px");
    label_translate_y_=new QLabel(this);
    label_translate_y_->setText("平移Y/px");
    label_rotate_=new QLabel(this);
    label_rotate_->setText("旋转/°");

    slider_anchor_x_=new QSlider(Qt::Horizontal,this);
    slider_anchor_y_=new QSlider(Qt::Horizontal,this);
    slider_prescale_=new QSlider(Qt::Horizontal,this);
    slider_translate_x_=new QSlider(Qt::Horizontal,this);
    slider_translate_y_=new QSlider(Qt::Horizontal,this);
    slider_rotate_=new QSlider(Qt::Horizontal,this);

    spinbox_anchor_x_=new QSpinBox(this);
    spinbox_anchor_y_=new QSpinBox(this);
    spinbox_prescale_=new QSpinBox(this);
    spinbox_translate_x_=new QSpinBox(this);
    spinbox_translate_y_=new QSpinBox(this);
    spinbox_rotate_=new QSpinBox(this);

    button_open_=new QPushButton(this);
    button_open_->setText("打开文件");

    button_save_=new QPushButton(this);
    button_save_->setText("保存文件");

    _bindValueEvents(&anchor_x_, slider_anchor_x_, spinbox_anchor_x_);
    _bindValueEvents(&anchor_y_, slider_anchor_y_, spinbox_anchor_y_);
    _bindValueEvents(&scale_precent_, slider_prescale_, spinbox_prescale_);
    _bindValueEvents(&translate_x_, slider_translate_x_, spinbox_translate_x_);
    _bindValueEvents(&translate_y_, slider_translate_y_, spinbox_translate_y_);
    _bindValueEvents(&rotate_degree_, slider_rotate_, spinbox_rotate_);

    connect(button_open_, &QPushButton::clicked, [&](){
        filename_in_ = QFileDialog::getOpenFileName(this,tr("Open Image"), "/home/jana", tr("Image Files (*.bmp *.jpg *.gif *.tif *.png)"));
        this->_loadImage();
    });

    connect(button_save_, &QPushButton::clicked, [&](){
        filename_out_ = QFileDialog::getSaveFileName(this,tr("Save Image"), "/home/jana", tr("Image Files (*.bmp *.jpg *.gif *.tif *.png)"));
        this->_saveImage();
    });


    grid_layout_=new QGridLayout;
    this->_reLayout();

    this->setWindowTitle("DIP Lab 1");
}

Widget::~Widget()
{
}


void Widget::_bindValueEvents(int* value, QSlider* slider, QSpinBox* spinbox)
{
    connect(slider, &QSlider::valueChanged, [=](){
        if(*value==slider->value()) return;
        *value=slider->value();
        spinbox->setValue(*value);
        this->_render();
    });
    connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged), [=](){
        if(*value==spinbox->value()) return;
        *value=spinbox->value();
        slider->setValue(*value);
        this->_render();
    });
}

void Widget::_render()
{
    img_out_=img_in_.transformTRS(translate_x_,translate_y_,rotate_degree_*3.1415926/180,scale_precent_*0.01,anchor_x_,anchor_y_);
    img_plot_in_->imshow(img_in_.toQImage());
    img_plot_out_->imshow(img_out_.toQImage());
}

void Widget::_loadImage()
{
    try
    {
        QImage tmp = QImage(filename_in_);
        if(tmp.sizeInBytes()==0)
        {
            qDebug()<<"fail to load or empty image.";
            return;
        }
        else
        {
            img_in_.fromQImage(tmp);
        }
        img_out_=img_in_;
        translate_x_=0;
        translate_y_=0;
        rotate_degree_=0;
        anchor_x_=img_in_.width()/2;
        anchor_y_=img_out_.height()/2;
        scale_precent_=100;

        slider_anchor_x_->setRange(0, img_in_.width());
        slider_anchor_y_->setRange(0, img_in_.height());
        slider_prescale_->setRange(-1000,1000);
        slider_translate_x_->setRange(-5*img_in_.width(), 5*img_in_.width());
        slider_translate_y_->setRange(-5*img_in_.height(), 5*img_in_.height());
        slider_rotate_->setRange(-180,180);


        spinbox_anchor_x_->setRange(0, img_in_.width());
        spinbox_anchor_y_->setRange(0, img_in_.height());
        spinbox_prescale_->setRange(-1000,1000);
        spinbox_translate_x_->setRange(-5*img_in_.width(), 5*img_in_.width());
        spinbox_translate_y_->setRange(-5*img_in_.height(), 5*img_in_.height());
        spinbox_rotate_->setRange(-180,180);

        slider_anchor_x_->setValue(anchor_x_);
        slider_anchor_y_->setValue(anchor_y_);
        slider_prescale_->setValue(scale_precent_);
        slider_translate_x_->setValue(translate_x_);
        slider_translate_y_->setValue(translate_y_);
        slider_rotate_->setValue(rotate_degree_);

        spinbox_anchor_x_->setValue(anchor_x_);
        spinbox_anchor_y_->setValue(anchor_y_);
        spinbox_prescale_->setValue(scale_precent_);
        spinbox_translate_x_->setValue(translate_x_);
        spinbox_translate_y_->setValue(translate_y_);
        spinbox_rotate_->setValue(rotate_degree_);

        this->_render();
        this->_reLayout();
    }
    catch(...)
    {
        qDebug()<<"error to load image.";
    }
}

void Widget::_saveImage()
{
    try
    {
        img_out_.toQImage().save(filename_out_);
    }
    catch(...)
    {
        qDebug()<<"fail to load image.";
    }
}

void Widget::_reLayout()
{
    grid_layout_->addWidget(img_plot_in_, 0, 0, 1, 3);
    grid_layout_->addWidget(img_plot_out_, 0, 3, 1, 3);
    grid_layout_->addWidget(label_anchor_x_, 1, 0, 1, 1);
    grid_layout_->addWidget(label_anchor_y_, 2, 0, 1, 1);
    grid_layout_->addWidget(label_translate_x_, 3, 0, 1, 1);
    grid_layout_->addWidget(label_translate_y_, 4, 0, 1, 1);
    grid_layout_->addWidget(label_rotate_, 5, 0, 1, 1);
    grid_layout_->addWidget(label_prescale_, 6, 0, 1, 1);
    grid_layout_->addWidget(slider_anchor_x_, 1, 1, 1, 4);
    grid_layout_->addWidget(slider_anchor_y_, 2, 1, 1, 4);
    grid_layout_->addWidget(slider_translate_x_, 3, 1, 1, 4);
    grid_layout_->addWidget(slider_translate_y_, 4, 1, 1, 4);
    grid_layout_->addWidget(slider_rotate_, 5, 1, 1, 4);
    grid_layout_->addWidget(slider_prescale_, 6, 1, 1, 4);
    grid_layout_->addWidget(spinbox_anchor_x_, 1, 5, 1, 1);
    grid_layout_->addWidget(spinbox_anchor_y_, 2, 5, 1, 1);
    grid_layout_->addWidget(spinbox_translate_x_, 3, 5, 1, 1);
    grid_layout_->addWidget(spinbox_translate_y_, 4, 5, 1, 1);
    grid_layout_->addWidget(spinbox_rotate_, 5, 5, 1, 1);
    grid_layout_->addWidget(spinbox_prescale_, 6, 5, 1, 1);

    grid_layout_->addWidget(button_open_, 7, 1, 1, 1);
    grid_layout_->addWidget(button_save_, 7, 4, 1, 1);
    setLayout(grid_layout_);
}
