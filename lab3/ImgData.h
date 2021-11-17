#ifndef IMGDATA_H
#define IMGDATA_H

#include <QDebug>
#include <cmath>

// TODO: 饱和饱和

template <typename T>
class ImgData
{
protected:
    // 所有子类必须保证 sizeof(data)=width*height，width,height 对用户是只读的
    int width_;
    int height_;
    T range_; // value in [0, range]
    T *data_;
    void _allocate();
    void _free();

public:
    T _pixel(int x, int y, T default_value = 0);

public:
    ImgData();
    ImgData(int width_, int height_, T range_);
    ImgData(const ImgData &img);
    ImgData(ImgData &&img) noexcept;
    ImgData &operator=(const ImgData &img);
    ImgData &operator=(ImgData &&img) noexcept;
    virtual ~ImgData();

    int width() const;
    int height() const;
    void setPixel(int x, int y, T c);
    T pixel(int x, int y) const;
    T pixelClamped(int x, int y) const;
    void setRange(T range);
    T range() const;
    void clear();
    T *bits();

    // 此函数为平台相关，移植请改写
    void debug();

    ImgData<T> _add(const ImgData<T> &rhs);
    ImgData<T> _amplify(float rhs);
    ImgData<T> _inverse();
    ImgData<T> _transpose();
    ImgData<T> _mirrorX();
    ImgData<T> _mirrorY();
    ImgData<T> _mirrorXY();
    ImgData<T> _multiply(const ImgData<T> &rhs);
};

template <typename T>
void ImgData<T>::_allocate()
{
    if (data_)
        _free();
    data_ = new T[width_ * height_];
    if (!data_)
        throw("ImgData allocate failed.");
}

template <typename T>
void ImgData<T>::_free()
{
    delete[] data_;
}

template <typename T>
ImgData<T>::ImgData() : width_(0), height_(0), range_(255), data_(nullptr)
{
}

template <typename T>
ImgData<T>::ImgData(int width_, int height_, T range_) : width_(width_), height_(height_), range_(range_), data_(nullptr)
{
    _allocate();
}

template <typename T>
ImgData<T>::ImgData(const ImgData &img) : width_(img.width_), height_(img.height_), range_(img.range_), data_(nullptr)
{
    _allocate();
    memcpy(data_, img.data_, sizeof(T) * width_ * height_);
}

template <typename T>
ImgData<T>::ImgData(ImgData &&img) noexcept : width_(img.width_), height_(img.height_), range_(img.range_), data_(img.data_)
{
    // Old img has been Stolen
    img.width_ = 0;
    img.height_ = 0;
    img.data_ = nullptr;
}

template <typename T>
ImgData<T> &ImgData<T>::operator=(const ImgData<T> &img)
{
    width_ = img.width_;
    height_ = img.height_;
    range_ = img.range_;
    _allocate();
    memcpy(data_, img.data_, sizeof(T) * width_ * height_);
    return *this;
}

template <typename T>
ImgData<T> &ImgData<T>::operator=(ImgData<T> &&img) noexcept
{
    // Old img has been Stolen
    width_ = img.width_;
    height_ = img.height_;
    range_ = img.range_;
    if (data_)
    {
        _free();
    }
    data_ = img.data_;
    img.width_ = 0;
    img.height_ = 0;
    img.data_ = nullptr;
    return *this;
}

template <typename T>
ImgData<T>::~ImgData()
{
    _free();
}

template <typename T>
int ImgData<T>::width() const
{
    return width_;
}

template <typename T>
int ImgData<T>::height() const
{
    return height_;
}

template <typename T>
void ImgData<T>::setPixel(int x, int y, T c)
{
    if (x < 0 || x >= width_ || y < 0 || y > height_)
        throw("out of bound.");
    data_[y * width_ + x] = c;
}

template <typename T>
T ImgData<T>::pixel(int x, int y) const
{
    if (x < 0 || x >= width_ || y < 0 || y > height_)
        throw("out of bound.");
    return data_[y * width_ + x];
}

template <typename T>
T ImgData<T>::pixelClamped(int x, int y) const
{
    if (x < 0 || x >= width_ || y < 0 || y > height_)
        throw("out of bound.");
    return std::min(this->range_, std::max(T(0), data_[y * width_ + x]));
}

template <typename T>
T ImgData<T>::_pixel(int x, int y, T default_value)
{
    if (x >= 0 && x < this->width_ && y >= 0 && y < this->height_)
        return this->pixel(x, y);
    return default_value;
}

template <typename T>
void ImgData<T>::setRange(T range)
{
    if (range == 0)
        throw("range must be above 0.");
    range_ = range;
}

template <typename T>
T ImgData<T>::range() const
{
    return range_;
}

template <typename T>
T *ImgData<T>::bits()
{
    return data_;
}

template <typename T>
void ImgData<T>::debug()
{
    qDebug() << "ImgData" << this->width() << this->height() << this->range();
    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            qDebug() << j << i << this->pixel(j, i);
        }
    }
    qDebug() << "END";
}

template <typename T>
ImgData<T> ImgData<T>::_add(const ImgData<T> &rhs)
{
    assert(this->width() == rhs.width());
    assert(this->height() == rhs.height());

    ImgData<T> result(this->width(), this->height(), this->range());

    if (std::is_same_v<T, float>)
    {
#pragma omp parallel for
        for (int i = 0; i < this->height(); i++)
        {
            for (int j = 0; j < this->width(); j++)
            {
                result.setPixel(j, i, this->pixel(j, i) + rhs.pixel(j, i));
            }
        }
    }
    else
    {
        // 整数需要饱和运算
        int range_max = (1ll << 8 * sizeof(T)) - 1;
#pragma omp parallel for
        for (int i = 0; i < this->height(); i++)
        {
            for (int j = 0; j < this->width(); j++)
            {
                int64_t value = 0ll + this->pixel(j, i) + rhs.pixel(j, i);
                if (value < 0)
                    value = 0;
                if (value > range_max)
                    value = range_max;
                result.setPixel(j, i, value);
            }
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_amplify(float rhs)
{
    ImgData<T> result(this->width(), this->height(), this->range());

    if (std::is_same_v<T, float>)
    {
#pragma omp parallel for
        for (int i = 0; i < this->height(); i++)
        {
            for (int j = 0; j < this->width(); j++)
            {
                result.setPixel(j, i, this->pixel(j, i) * rhs);
            }
        }
    }
    else
    {
        // 整数需要饱和运算
        int range_max = (1ll << 8 * sizeof(T)) - 1;
#pragma omp parallel for
        for (int i = 0; i < this->height(); i++)
        {
            for (int j = 0; j < this->width(); j++)
            {
                int64_t value = 0ll + this->pixel(j, i) * rhs;
                if (value < 0)
                    value = 0;
                if (value > range_max)
                    value = range_max;
                result.setPixel(j, i, value);
            }
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_inverse()
{
    ImgData<T> result(this->width(), this->height(), this->range());

#pragma omp parallel for
    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            result.setPixel(j, i, this->range() - this->pixel(j, i));
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_transpose()
{
    ImgData<T> result(this->height(), this->width(), this->range());

#pragma omp parallel for
    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            result.setPixel(i, j, this->pixel(j, i));
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_mirrorX()
{
    ImgData<T> result(this->width(), this->height(), this->range());

#pragma omp parallel for
    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            result.setPixel(j, i, this->pixel(this->width() - 1 - j, i));
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_mirrorY()
{
    ImgData<T> result(this->width(), this->height(), this->range());

#pragma omp parallel for
    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            result.setPixel(j, i, this->pixel(j, this->height() - 1 - i));
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_mirrorXY()
{
    ImgData<T> result(this->width(), this->height(), this->range());

#pragma omp parallel for
    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            result.setPixel(j, i, this->pixel(this->width() - 1 - j, this->height() - 1 - i));
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_multiply(const ImgData<T> &rhs)
{
    int n = this->height();
    int m = this->width();
    int r = rhs.width();

    assert(this->width() == rhs.height());

    ImgData<T> result(n, r, this->range());

#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < r; j++)
        {
            for (int k = 0; k < m; k++)
            {
                result.setPixel(j, i, this->pixel(k, i) * rhs.pixel(j, k));
            }
        }
    }

    return result;
}

#endif // IMGDATA_H
