#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <cstdint>
#include "img.h"
#include "imgplot.h"
#include <QGridLayout>
#include <QDebug>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <QPushButton>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

    void _bindValueEvents(int* value, QSlider* slider, QSpinBox* spinbox);
    void _render();
    void _loadImage();
    void _saveImage();
    void _reLayout();
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private:
    QString filename_in_;
    QString filename_out_;
    Img<uint8_t> img_in_;
    Img<uint8_t> img_out_;
    int anchor_x_;
    int anchor_y_;
    int scale_precent_;
    int translate_x_;
    int translate_y_;
    int rotate_degree_;
    ImgPlot* img_plot_in_;
    ImgPlot* img_plot_out_;
    QLabel* label_anchor_x_;
    QLabel* label_anchor_y_;
    QLabel* label_prescale_;
    QLabel* label_translate_x_;
    QLabel* label_translate_y_;
    QLabel* label_rotate_;

    QSlider* slider_anchor_x_;
    QSlider* slider_anchor_y_;
    QSlider* slider_prescale_;
    QSlider* slider_translate_x_;
    QSlider* slider_translate_y_;
    QSlider* slider_rotate_;

    QSpinBox* spinbox_anchor_x_;
    QSpinBox* spinbox_anchor_y_;
    QSpinBox* spinbox_prescale_;
    QSpinBox* spinbox_translate_x_;
    QSpinBox* spinbox_translate_y_;
    QSpinBox* spinbox_rotate_;

    QPushButton* button_open_;
    QPushButton* button_save_;


    QGridLayout *grid_layout_;
};
#endif // WIDGET_H
