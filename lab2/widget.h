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
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

    void _bindValueEvents(int *value, QSlider *slider, QSpinBox *spinbox);
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
    Img<uint16_t> img_in_;
    Img<uint16_t> img_out_;
    int anchor_x_;
    int anchor_y_;
    int scale_precent_;
    int translate_x_;
    int translate_y_;
    int rotate_degree_;
    ImgPlot *img_plot_in_;
    ImgPlot *img_plot_out_;

    QPushButton *button_open_;
    QPushButton *button_save_;

    QGridLayout *grid_layout_;
};
#endif // WIDGET_H
