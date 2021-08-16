#ifndef IMGDATA_H
#define IMGDATA_H

// 约定：变量名小写字母下划线分隔，方法名小写开头驼峰不标注类型信息，类名大写开头，形参与成员变量同名时形参加下划线前缀
// 涉及颜色分量的整型数据用 stdint 类型，其它用普通 int

// 原则：ImgData 是管理图像数据的“核心”实体类，必须与框架无关，因此禁止在该类的声明和实现中使用框架/平台依赖的类型
// 原则：所有子类必须保证 sizeof(data)=width*height，width,height 对用户是只读的

#include <stdint.h>

class ImgData
{
protected:
    int width;
    int height;
    uint8_t* data;
    // 约定：GRAY8，offset=(y*w+x)+0，不作对齐处理
    bool allocate();
    bool free();
public:
    ImgData();
    ImgData(const ImgData& img);
    ImgData operator = (const ImgData& img);
    ~ImgData();

//    void setWidth(int _width);
//    void setHeight(int _height);
    int getWidth();
    int getHeight();
    void setPixel(int x,int y,uint8_t c);
    uint8_t getPixel(int x,int y);
};

#endif // IMGDATA_H
