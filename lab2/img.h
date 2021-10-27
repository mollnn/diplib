#ifndef IMG_H
#define IMG_H

/////////////////////////////////
/// 类结构
/// 模板参数 T 支持 uint8_t, uint16_t
/// ImgData         公共基类，数据结构（唯一拥有数据成员的类）
/// ImgConvert      与 QImage 互转（唯一的平台相关类）
/// Img...          处理算法（不负责数据管理）
/// Img             虚继承以上各类，提供给用户
/// ImgPlot         QImage 显示辅助类（与以上各类无继承关系）
/// 命名规则示例：Class.funcPublic(), Class._funcPrivate(), Class.member_variable, funcGlobal(), local_variable
/////////////////////////////////

#include "imgconvert.h"
#include "imgaffine.h"

template <typename T>
class Img : public virtual ImgConvert<T>, public virtual ImgAffine<T>
{
public:
    Img() {}
    Img(const Img &img) : ImgData<T>(img) {}
    Img(Img &&img) : ImgData<T>(img) {}
    Img(const ImgData<T> &img) : ImgData<T>(img) {}
    Img(ImgData<T> &&img) : ImgData<T>(img) {}
    Img &operator=(const Img &img)
    {
        ImgData<T>::operator=(img);
        return *this;
    }
    Img &operator=(Img &&img)
    {
        ImgData<T>::operator=(std::move(img));
        return *this;
    }
    Img &operator=(const ImgData<T> &img)
    {
        ImgData<T>::operator=(img);
        return *this;
    }
    Img &operator=(ImgData<T> &&img)
    {
        ImgData<T>::operator=(std::move(img));
        return *this;
    }
};

#endif // IMG_H