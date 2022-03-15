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
    T getThresholdOtsu(float step = 1);
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

    // 以下实现时间复杂度为 O(v^2)，可以优化到 O(v)
#pragma omp parallel for
    for (int i = 0; i < len; i++)
    {
        float psi = 0;
        for (int j = 0; j <= i; j++)
        {
            float r = h[j] / (s[i] + 1e-9);
            psi -= r * log(r + 1e-9);
        }
        for (int j = i + 1; j < len; j++)
        {
            if (1 - s[i] < 1e-9)
                continue;
            float r = h[j] / (1 - s[i] + 1e-9);
            psi -= r * log(r + 1e-9);
        }
        psi_list[i] = psi;
    }

    return (max_element(psi_list.begin(), psi_list.end()) - psi_list.begin()) * step;
}

template <typename T>
T ImgAlgSegment<T>::getThresholdOtsu(float step)
{
    // 引入 step 主要是为了兼容浮点

    int len = int(this->range_ / (step + 1e-9) + 1);
    std::vector<float> h(len);
    std::vector<float> s(len);
    std::vector<float> h1(len);
    std::vector<float> s1(len);
    std::vector<float> h2(len);
    std::vector<float> s2(len);
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

    for (int i = 0; i < len; i++)
    {
        float x = i * step;
        h1[i] = h[i] * x;
        h2[i] = h[i] * x * x;
    }

    s[0] = h[0];
    s1[0] = h1[0];
    s2[0] = h2[0];

    for (int i = 1; i < len; i++)
    {
        s[i] = s[i - 1] + h[i];
        s1[i] = s1[i - 1] + h1[i];
        s2[i] = s2[i - 1] + h2[i];
    }

    // 修正浮点累积误差
    float sum = s.back();
    for (int i = 0; i < len; i++)
    {
        h[i] /= sum;
        s[i] /= sum;
    }

    std::vector<float> psi_list(len);

    // O(n)
    for (int i = 0; i < len; i++)
    {
        float psi = 0;
        float pa0 = s[i];
        float pa1 = s1[i];
        float pa2 = s2[i];
        float pb0 = s.back() - pa0;
        float pb1 = s1.back() - pa1;
        float pb2 = s2.back() - pa2;
        float sx2a = pa2 / (pa0 + 1e-10);
        float sx2b = pb2 / (pb0 + 1e-10);
        float s2xa = pow(pa1 / (pa0 + 1e-10), 2);
        float s2xb = pow(pb1 / (pb0 + 1e-10), 2);
        float sigma_a = sx2a - s2xa;
        float sigma_b = sx2b - s2xb;
        psi = pa0 * sigma_a + pb0 * sigma_b;
        psi_list[i] = psi;
    }

    return (min_element(psi_list.begin(), psi_list.end()) - psi_list.begin()) * step;
}

#endif // IMGALGSEGMENT_H
