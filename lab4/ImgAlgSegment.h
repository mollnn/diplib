#ifndef IMGALGSEGMENT_H
#define IMGALGSEGMENT_H

#include "ImgData.h"

template <typename T>
class ImgAlgSegment : public virtual ImgData<T>
{
public:
    ImgAlgSegment() {}

    // 这里是分类器和阈值求解算法
    ImgData<T> threshold(T t);
    T getThresholdMaxEntropy(float step = 1);
};

template <typename T>
ImgData<T> ImgAlgSegment<T>::threshold(T t)
{
    ImgData<T> result(this->width(), this->height(), this->range_);

#pragma omp parallel for
    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            if (this->pixel(j, i) < t)
                result.setPixel(j, i, 0);
            else
                result.setPixel(j, i, this->range_);
        }
    }

    return result;
}

template <typename T>
T ImgAlgSegment<T>::getThresholdMaxEntropy(float step)
{
    // 引入 step 主要是为了兼容浮点

    int len = int(this->range_ / (step + 1e-9) + 1);
    std::vector<float> h(len);
    std::vector<float> s(len);
    float one_contribution = 1.0 / this->width() / this->height();

    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            float val = this->pixel(j, i);
            int idx = val / (step + 1e-9);
            h[idx] += one_contribution;
        }
    }

    s[0] = h[0];
    for (int i = 1; i < len; i++)
        s[i] = s[i - 1] + h[i];

    // 修正浮点累积误差
    float sum = s.back();
    for (int i = 0; i < len; i++)
        h[i] /= sum;
    for (int i = 0; i < len; i++)
        s[i] /= sum;

    std::vector<float> psi_list(len);
    for (int i = 0; i < len; i++)
    {
        float psi = 0;
        for (int j = 0; j < i; j++)
        {
            float r = h[j] / (s[i] + 1e-9);
            psi -= r * log(r + 1e-9);
        }
        for (int j = i; j < len; j++)
        {
            if (1 - s[i] < 1e-9)
                continue;
            float r = h[j] / (1 - s[i] + 1e-9);
            psi -= r * log(r + 1e-9);
        }
        psi_list[i] = psi;
    }

    QVector<float> vv;
    for (auto i : s)
        vv.push_back(i);
    qDebug() << vv;

    QVector<float> vec;
    for (auto i : psi_list)
        vec.push_back(i);
    qDebug() << vec;

    return (max_element(psi_list.begin(), psi_list.end()) - psi_list.begin()) * step;
}

#endif // IMGALGSEGMENT_H
