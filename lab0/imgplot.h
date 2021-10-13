#ifndef IMGPLOT_H
#define IMGPLOT_H

#include <QObject>
#include <QWidget>
#include <QLabel>

class ImgPlot : public QWidget
{
    Q_OBJECT
private:
    QLabel* label;
public:
    explicit ImgPlot(QWidget *parent = nullptr);
    void imshow(const QImage& img);
signals:

};


#endif // IMGPLOT_H
