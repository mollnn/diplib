#include "Tester.h"
#include "img.h"

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
        img = img.crop(0, 0, img.width(), img.height(), 0.1, 1.1);
    }
}
