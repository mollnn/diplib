#include "qimageplotbox.h"
#include <QDebug>
#include <QLabel>
#include <QMouseEvent>

QImagePlotBox::QImagePlotBox(QWidget *parent) : QWidget(parent)
{

    label_ = new QLabel(this);

    QSizePolicy size_policy = label_->sizePolicy();
    size_policy.setVerticalPolicy(QSizePolicy::Expanding);
    size_policy.setHorizontalPolicy(QSizePolicy::Expanding);
    label_->setSizePolicy(size_policy);

    size_policy = this->sizePolicy();
    size_policy.setVerticalPolicy(QSizePolicy::Expanding);
    size_policy.setHorizontalPolicy(QSizePolicy::Expanding);
    this->setSizePolicy(size_policy);

    this->is_mouse_pressed_=false;
}

void QImagePlotBox::imshow(const QImage &img)
{
    label_->setPixmap(QPixmap::fromImage(img.copy()));
}

void QImagePlotBox::resizeEvent(QResizeEvent * event)
{
    label_->setFixedSize(this->size());
    event->accept();
}

void QImagePlotBox::mousePressEvent(QMouseEvent *event)
{
    this->drag_last_pos_=event->pos();
    this->is_mouse_pressed_=true;
    event->accept();
}

void QImagePlotBox::mouseMoveEvent(QMouseEvent *event)
{
    if(this->is_mouse_pressed_)
    {
        auto current_pos = event->pos();
        emit dragSignal(current_pos.x()-drag_last_pos_.x(), current_pos.y()-drag_last_pos_.y());
        this->drag_last_pos_=current_pos;
    }
    event->accept();
}

void QImagePlotBox::mouseReleaseEvent(QMouseEvent *event)
{
    if(this->is_mouse_pressed_)
    {
        auto current_pos = event->pos();
        emit dragSignal(current_pos.x()-drag_last_pos_.x(), current_pos.y()-drag_last_pos_.y());
        this->drag_last_pos_=current_pos;
        this->is_mouse_pressed_=false;
    }
    event->accept();
}

void QImagePlotBox::wheelEvent(QWheelEvent *event)
{
    double delta = event->delta();
    emit scrollSignal(delta);
    event->accept();
}
