#ifndef QImagePlotBox_H
#define QImagePlotBox_H

#include <QObject>
#include <QWidget>
#include <QLabel>

class QImagePlotBox : public QWidget
{
    Q_OBJECT
private:
    QLabel *label_;
    QPoint drag_last_pos_;
    bool is_mouse_pressed_;

public:
    explicit QImagePlotBox(QWidget *parent = nullptr);
    void imshow(const QImage &img);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
signals:
    void dragSignal(int dx, int dy);
    void scrollSignal(double val);
};

#endif // IMGPLOT_H
