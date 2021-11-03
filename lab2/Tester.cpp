#include "Tester.h"
#include "img.h"

#include <QDebug>

Test1::Test1()
{

}

Test1::~Test1()
{

}

void Test1::test_case1()
{
    Img<uint16_t> img(10000, 10000, 4095);
    QBENCHMARK
    {
        img = img.crop(0, 0, img.width(), img.height());
    }
}


void Test1::test_case2()
{
    Img<uint16_t> img(10000, 10000, 4095);
    QBENCHMARK
    {
        auto img_out = img.applyGrayWindow<uint8_t>(234, 345);
    }
}


void Test1::test_case3()
{
    Img<uint16_t> img(10000, 10000, 4095);
    QBENCHMARK
    {
        img = img.crop(0, 0, img.width(), img.height(), 1, 1.5);
    }
}




//void Test1::test_case4()
//{
//    Img<uint16_t> img(1000, 1000, 4095);
//    for(int i=0;i<1000;i++) for(int j=0;j<1000;j++) img.setPixel(j,i,rand()%4096);
//    auto img1 = img.crop(0, 0, img.width(), img.height(), 1, 1.5);
//    auto img2 = img.crop_base(0, 0, img.width(), img.height(), 1, 1.5);
//    double mse = 0;
//    for(int i=0;i<1000;i++) for(int j=0;j<1000;j++) mse+=pow(img1.pixel(j,i)-img2.pixel(j,i),2);
//    mse /= 1e6;
//    qDebug()<<"MSE ="<<mse;
//}
