#ifndef IMGPLOT_H
#define IMGPLOT_H

#include <QObject>
#include <QWidget>
#include <QLabel>

class ImgPlotBox : public QWidget
{
    Q_OBJECT
private:
    QLabel *label_;
    QPoint drag_last_pos_;
    bool is_mouse_pressed_;
public:
    explicit ImgPlotBox(QWidget *parent = nullptr);
    void imshow(const QImage &img);
    void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void dragSignal(int dx, int dy);
};

#endif // IMGPLOT_H
