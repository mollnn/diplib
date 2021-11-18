#ifndef ImgData_H
#define ImgData_H

#include <QDebug>
#include <cmath>

#include "xmmintrin.h"
#include "immintrin.h"
#include "ammintrin.h"

template <typename T>
class ImgData
{
protected:
    // 所有子类必须保证 sizeof(data)=width*height，width,height 对用户是只读的
    int width_;
    int height_;
    T range_; // unsigned value in [0, range], signed value in [-range, range] (float)
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

    template <typename R>
    ImgData<R> cast();
    template <typename R>
    ImgData<R> cast(float k, float b);

    // 此函数为平台相关，移植请改写
    void debug();

    // 对所有算子，不检查范围，不做饱和运算，选择确保正确的中间运算类型
    // 饱和运算的责任交给用户
    // uint16 -> float -> ... -> float -> clamped -> uint16

    // Algebra
    ImgData<T> _add(const ImgData<T> &rhs);
    ImgData<T> _subtract(const ImgData<T> &rhs);
    ImgData<T> _amplify(float rhs);
    ImgData<T> _inverse();
    ImgData<T> _transpose();
    ImgData<T> _mirrorX();
    ImgData<T> _mirrorY();
    ImgData<T> _mirrorXY();
    ImgData<T> _multiply(const ImgData<T> &rhs);
    ImgData<T> _clamp(T clamp_min, T clamp_max);

    ImgData<T> operator+(const ImgData<T> &rhs);
    ImgData<T> operator-(const ImgData<T> &rhs);
    ImgData<T> operator*(const ImgData<T> &rhs);
    ImgData<T> operator*(float rhs);
    ImgData<T> operator/(float rhs);

    ImgData<T> &operator+=(const ImgData<T> &rhs);
    ImgData<T> &operator-=(const ImgData<T> &rhs);
    ImgData<T> &operator*=(const ImgData<T> &rhs);
    ImgData<T> &operator*=(float rhs);
    ImgData<T> &operator/=(float rhs);

    // Conv
    ImgData<T> _conv2d(ImgData<float> kernel);
    ImgData<T> _conv2d_Baseline(ImgData<float> kernel);
    ImgData<T> _conv2d_Fast(ImgData<float> kernel);
    ImgData<T> _conv2d_Avx2(ImgData<float> kernel);

    // Copy
    ImgData<T> _copySubImg(int x0, int y0, int target_width, int target_height);
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
template <typename R>
ImgData<R> ImgData<T>::cast()
{
    ImgData<R> result(this->width(), this->height(), this->range());
    auto result_ptr = result.bits();
    auto source_ptr = this->bits();
#pragma omp parallel for
    for (int i = 0; i < this->height_; i++)
    {
        for (int j = 0; j < this->width_; j++)
        {
            result_ptr[i * this->width_ + j] = source_ptr[i * this->width_ + j];
        }
    }
    return result;
}

template <typename T>
template <typename R>
ImgData<R> ImgData<T>::cast(float k, float b)
{
    ImgData<R> result(this->width(), this->height(), this->range());
    auto result_ptr = result.bits();
    auto source_ptr = this->bits();
#pragma omp parallel for
    for (int i = 0; i < this->height_; i++)
    {
        for (int j = 0; j < this->width_; j++)
        {
            result_ptr[i * this->width_ + j] = k * source_ptr[i * this->width_ + j] + b;
        }
    }
    return result;
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

#pragma omp parallel for
    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            result.setPixel(j, i, this->pixel(j, i) + rhs.pixel(j, i));
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_subtract(const ImgData<T> &rhs)
{
    assert(this->width() == rhs.width());
    assert(this->height() == rhs.height());

    ImgData<T> result(this->width(), this->height(), this->range());

#pragma omp parallel for
    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            result.setPixel(j, i, this->pixel(j, i) - rhs.pixel(j, i));
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_amplify(float rhs)
{
    ImgData<T> result(this->width(), this->height(), this->range());

#pragma omp parallel for
    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            result.setPixel(j, i, this->pixel(j, i) * rhs);
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

template <typename T>
ImgData<T> ImgData<T>::_clamp(T clamp_min, T clamp_max)
{
    ImgData<T> result(this->width(), this->height(), this->range_);

#pragma omp parallel for
    for (int i = 0; i < this->height(); i++)
    {
        for (int j = 0; j < this->width(); j++)
        {
            T value = this->pixel(j, i);
            if (value < clamp_min)
                value = clamp_min;
            if (value > clamp_max)
                value = clamp_max;
            result.setPixel(j, i, value);
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::operator+(const ImgData<T> &rhs)
{
    return this->_add(rhs);
}

template <typename T>
ImgData<T> ImgData<T>::operator-(const ImgData<T> &rhs)
{
    return this->_subtract(rhs);
}

template <typename T>
ImgData<T> ImgData<T>::operator*(const ImgData<T> &rhs)
{
    return this->_multiply(rhs);
}

template <typename T>
ImgData<T> ImgData<T>::operator*(float rhs)
{
    return this->_amplify(rhs);
}

template <typename T>
ImgData<T> ImgData<T>::operator/(float rhs)
{
    return this->_amplify(1.0f / rhs);
}

template <typename T>
ImgData<T> &ImgData<T>::operator+=(const ImgData<T> &rhs)
{
    *this = *this + rhs;
    return *this;
}

template <typename T>

ImgData<T> &ImgData<T>::operator-=(const ImgData<T> &rhs)
{
    *this = *this - rhs;
    return *this;
}

template <typename T>

ImgData<T> &ImgData<T>::operator*=(const ImgData<T> &rhs)
{
    *this = *this * rhs;
    return *this;
}

template <typename T>
ImgData<T> &ImgData<T>::operator*=(float rhs)
{
    *this = *this * rhs;
    return *this;
}

template <typename T>
ImgData<T> &ImgData<T>::operator/=(float rhs)
{
    *this = *this / rhs;
    return *this;
}

template <typename T>
ImgData<T> ImgData<T>::_copySubImg(int x0, int y0, int target_width, int target_height)
{
    ImgData<T> result(target_width, target_height, this->range_);
    memset(result.bits(), 0, target_width * target_height * sizeof(T));

    int source_x_begin = x0;
    int source_x_end = x0 + target_width;
    int target_x_begin = 0;
    int target_x_end = target_width;

    if (source_x_begin < 0)
    {
        target_x_begin -= source_x_begin;
        source_x_begin -= source_x_begin;
    }
    if (target_x_begin < 0)
    {
        source_x_begin -= target_x_begin;
        target_x_begin -= target_x_begin;
    }
    if (source_x_end > this->width_)
    {
        int delta = source_x_end - this->width_;
        source_x_end -= delta;
        target_x_end -= delta;
    }
    if (target_x_end > target_width)
    {
        int delta = target_x_end - target_width;
        source_x_end -= delta;
        target_x_end -= delta;
    }
    if (source_x_end < 0 || target_x_end < 0 || source_x_begin >= this->width_ || target_x_begin >= target_width)
    {
        return result;
    }

#pragma omp parallel for
    for (int i = 0; i < target_height; i++)
    {
        int y1 = i + y0;
        if (y1 >= 0 && y1 < this->height_)
        {
            memcpy(result.bits() + i * target_width + target_x_begin,
                   this->data_ + y1 * this->width_ + source_x_begin,
                   (source_x_end - source_x_begin) * sizeof(T));
        }
    }
    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_conv2d(ImgData<float> kernel)
{
    if (kernel.width() * kernel.height() >= 9 || kernel.height() >= 5)
        return this->_conv2d_Avx2(kernel);
    else
        return this->_conv2d_Fast(kernel);
}

template <typename T>
ImgData<T> ImgData<T>::_conv2d_Baseline(ImgData<float> kernel)
{
    ImgData<T> result(this->width_, this->height_, this->range_);

    int kernel_width_half = (kernel.width() - 1) / 2;
    int kernel_height_half = (kernel.height() - 1) / 2;

#pragma omp parallel for
    for (int i = 0; i < result.height(); i++)
    {
        for (int j = 0; j < result.width(); j++)
        {
            float sum = 0;
            for (int u = 0; u < kernel.height(); u++)
            {
                for (int v = 0; v < kernel.width(); v++)
                {
                    sum += this->_pixel(j + kernel_width_half - v, i + kernel_height_half - u) * kernel.pixel(v, u);
                }
            }
            result.setPixel(j, i, sum);
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_conv2d_Fast(ImgData<float> kernel)
{
    // 先生成 padding 后的图像，以避免循环内层出现分支
    ImgData<T> result(this->width_, this->height_, this->range_);

    int kernel_width_half = (kernel.width() - 1) / 2;
    int kernel_height_half = (kernel.height() - 1) / 2;

    int image_padding_width = this->width() + 2 * kernel_width_half;
    int image_padding_height = this->height() + 2 * kernel_height_half;

    int kernel_width = kernel.width();
    int kernel_height = kernel.height();

    int result_width = result.width();
    int result_height = result.height();

    ImgData<T> image_padding = this->_copySubImg(-kernel_width_half, -kernel_height_half, image_padding_width, image_padding_height);

    T *image_padding_ptr = image_padding.bits();
    T *image_result_ptr = result.bits();
    float *kernel_ptr = kernel.bits();

#pragma omp parallel for
    for (int i = 0; i < result_height; i++)
    {
        for (int j = 0; j < result_width; j++)
        {
            T *image_result_px_ptr = image_result_ptr + i * result_width + j;
            float sum = 0;
            for (int u = 0; u < kernel_height; u++)
            {
                for (int v = 0; v < kernel_width; v++)
                {
                    T *image_padding_px_ptr = image_padding_ptr + (i + kernel_height - u - 1) * image_padding_width + j + kernel_width - v - 1;
                    float *kernel_px_ptr = kernel_ptr + u * kernel_width + v;
                    sum += (*kernel_px_ptr) * (*image_padding_px_ptr);
                }
            }
            (*image_result_px_ptr) = sum;
        }
    }

    return result;
}

template <typename T>
ImgData<T> ImgData<T>::_conv2d_Avx2(ImgData<float> kernel)
{
    // 卷积核翻转
    kernel = kernel._mirrorXY();

    ImgData<T> result(this->width_, this->height_, this->range_);

    int kernel_width_half = (kernel.width() - 1) / 2;
    int kernel_height_half = (kernel.height() - 1) / 2;

    int image_padding_width = this->width() + 2 * kernel_width_half;
    int image_padding_height = this->height() + 2 * kernel_height_half;

    int kernel_width = kernel.width();
    int kernel_height = kernel.height();

    int result_width = result.width();
    int result_width_r8 = result_width / 8 * 8;
    int result_height = result.height();

    ImgData<T> image_padding = this->_copySubImg(-kernel_width_half, -kernel_height_half, image_padding_width, image_padding_height);

    T *image_padding_ptr = image_padding.bits();
    T *image_result_ptr = result.bits();
    float *kernel_ptr = kernel.bits();

    // 图像转换为 32 位浮点
    float *image_ps;
    if (sizeof(T) == 4)
    {
        image_ps = reinterpret_cast<float *>(image_padding_ptr);
    }
    else
    {
        image_ps = new float[image_padding_width * image_padding_height];
#pragma omp parallel for
        for (int i = 0; i < image_padding_width * image_padding_height; i++)
        {
            image_ps[i] = image_padding_ptr[i];
        }
    }

    // 卷积核扩展（每像素重复 8 次）
    float *kernel_ps = new float[kernel_width * kernel_height * 8];
    for (int i = 0; i < kernel_width * kernel_height; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            kernel_ps[i * 8 + j] = kernel_ptr[i];
        }
    }

    int *kernel_offsets = new int[kernel_height * kernel_width];
    int *image_offsets = new int[kernel_height * kernel_width];

    for (int u = 0; u < kernel_height; u++)
    {
        for (int v = 0; v < kernel_width; v++)
        {
            kernel_offsets[u * kernel_width + v] = (u * kernel_width + v) * 8;
            image_offsets[u * kernel_width + v] = u * image_padding_width + v;
        }
    }

#pragma omp parallel for
    for (int i = 0; i < result_height; i++)
    {
        // SIMD
        for (int j = 0; j < result_width_r8; j += 8)
        {
            float sum_ps[8];
            // 每次计算 8 个连续像素的结果
            // 卷积仍然通过循环枚举
            __m256 sum_vec = _mm256_setzero_ps();
            float *image_ps_block_ptr = image_ps + i * image_padding_width + j;

            for (int k = 0; k < kernel_height * kernel_width; k++)
            {
                __m256 kernel_vec = _mm256_loadu_ps(kernel_ps + kernel_offsets[k]);
                __m256 image_vec = _mm256_loadu_ps(image_ps_block_ptr + image_offsets[k]);
                sum_vec = _mm256_fmadd_ps(kernel_vec, image_vec, sum_vec);
            }

            if (sizeof(T) == 4)
            {
                _mm256_storeu_ps(reinterpret_cast<float *>(image_result_ptr + i * result_width + j), sum_vec);
            }
            else
            {
                _mm256_storeu_ps(sum_ps, sum_vec);
                // 转回原类型
                for (int r = 0; r < 8; r++)
                {
                    image_result_ptr[i * result_width + j + r] = sum_ps[r];
                }
            }
        }

        // Serial for remaining
        for (int j = result_width_r8; j < result_width; j++)
        {
            T *image_result_px_ptr = image_result_ptr + i * result_width + j;
            float sum = 0;
            for (int u = 0; u < kernel_height; u++)
            {
                for (int v = 0; v < kernel_width; v++)
                {
                    // 卷积核已经翻转过，这里做的是相关运算
                    T *image_padding_px_ptr = image_padding_ptr + (i + u) * image_padding_width + j + v;
                    float *kernel_px_ptr = kernel_ptr + u * kernel_width + v;
                    sum += (*kernel_px_ptr) * (*image_padding_px_ptr);
                }
            }
            (*image_result_px_ptr) = sum;
        }
    }

    if (sizeof(T) != 4)
        delete[] image_ps;

    delete[] kernel_ps;

    delete[] kernel_offsets;
    delete[] image_offsets;

    return result;
}

#endif // IMGDATA_H
