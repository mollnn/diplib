#ifndef IMGPLOT_H
#define IMGPLOT_H

#include <QObject>
#include <QWidget>
#include <QLabel>

class QImagePlot : public QWidget
{
    Q_OBJECT
private:
    QLabel *label_;

public:
    explicit QImagePlot(QWidget *parent = nullptr);
    void imshow(const QImage &img);
signals:
};

#endif // IMGPLOT_H
