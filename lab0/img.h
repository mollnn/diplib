#ifndef IMG_H
#define IMG_H

/////////////////////////////////
/// 类结构
/// 模板参数 T 支持 uint8_t, uint16_t, uint32_t 等无符号整数类型
/// ImgData         公共基类，数据结构
/// ImgConvert      与 QImage 互转
/// Img...          处理算法
/// Img             虚继承以上各类，提供给用户
/// ImgPlot         QImage 显示辅助类（与以上各类无继承关系）
/// 代码风格基本遵循 google-styleguide.googlecode.com
/////////////////////////////////

#include "imgconvert.h"
#include "imgthresholding.h"

template <typename T>
class Img: public virtual ImgConvert<T>, public virtual ImgThresholding<T>
{
public:
    Img();
};


template <typename T>
Img<T>::Img()
{

}


#endif // IMG_H
