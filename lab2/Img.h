#ifndef IMG_H
#define IMG_H

///////////////////////////////////////////////
/// 类结构
/// 模板参数 T 支持 uint8_t, uint16_t
///////////////////////////////////////////////
///////// 【数据结构层】 拥有数据成员，提供基本方法
/// ImgData                 公共基类，数据结构
/// vec3, mat3, ...         内部辅助结构体，对外需再封装
///////////////////////////////////////////////
///////// 【核心算法层】 基于数据结构，实现核心算法。允许层内调用
/// ImgAlg...               核心算法实现，允许 C 风格传参
/// SIMD 加速在类内完成，SIMT(CUDA) 加速单独编译成动态库
///////////////////////////////////////////////
///////// 【功能逻辑层】 利用核心算法，完成业务需求。禁止层内调用
/// ImgFunc...              功能逻辑，接口参数必须以对象形式传递
/// ImgConvert              与 QImage 互转
///////////////////////////////////////////////
///////// 【交互封装层】 仅封装
/// Img                     “集大成者”
/// AffineTransform, ...    接口参数包装结构，提供少量运算方法
/// QImagePlot              QImage 显示
/// QImagePlotBox           QImage 高级显示
///////////////////////////////////////////////
/// 命名规则示例：Class.fPub(), Class._fPro(), Clas.__fStaticPro
///       Class._fPriv_Spec(), Class.mem_var, fGlobal(), local_var
///////////////////////////////////////////////

// 加速控制标志
#ifndef IMG_ENABLE_CUDA
#define IMG_ENABLE_CUDA     // 注释该行以禁用 CUDA
#endif
#ifndef IMG_ENABLE_AVX2
#define IMG_ENABLE_AVX2 // 注释该行以禁用 AVX2
#endif

#include "ImgConvert.h"
#include "ImgFuncTransform.h"
#include "ImgFuncGrayWindow.h"

template <typename T>
class Img : public virtual ImgConvert<T>, public virtual ImgFuncTransform<T>, public virtual ImgFuncGrayWindow<T>
{
public:
    Img() {}
    Img(const Img &img) : ImgData<T>(img) {}
    Img(Img &&img) : ImgData<T>(img) {}
    Img(const ImgData<T> &img) : ImgData<T>(img) {}
    Img(ImgData<T> &&img) : ImgData<T>(img) {}
    Img(int width, int height, T range) : ImgData<T>(width, height, range) {}
    Img &operator=(const Img &img);
    Img &operator=(Img &&img);
    Img &operator=(const ImgData<T> &img);
    Img &operator=(ImgData<T> &&img);
};

//////////////////////////////////////////////

template <typename T>
Img<T> &Img<T>::operator=(const Img<T> &img)
{
    ImgData<T>::operator=(img);
    return *this;
}

template <typename T>
Img<T> &Img<T>::operator=(Img<T> &&img)
{
    ImgData<T>::operator=(std::move(img));
    return *this;
}

template <typename T>
Img<T> &Img<T>::operator=(const ImgData<T> &img)
{
    ImgData<T>::operator=(img);
    return *this;
}

template <typename T>
Img<T> &Img<T>::operator=(ImgData<T> &&img)
{
    ImgData<T>::operator=(std::move(img));
    return *this;
}

#endif // IMG_H
