#ifndef TESTER_H
#define TESTER_H

#include <QtTest>
#include <QDebug>
#include "Img.h"

// 自动测试程序
// _valid：正确性验证
// _benchmark：性能测试

class Test1 : public QObject
{
    Q_OBJECT

public:
    Test1() {}
    ~Test1() {}

private slots:
//    void test_alg_conv2d_fast_valid()
//    {
//        Img<uint16_t> img(100, 100, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 100, rand() % 100, rand() % 256);
//        Img<uint16_t> img_conv2d = img._conv2d_Baseline(img.__getLaplacianKernel2D(3));
//        Img<uint16_t> img_conv2d_fast = img._conv2d_Fast(img.__getLaplacianKernel2D(3));
//        for (int i = 0; i < 100; i++)
//            for (int j = 0; j < 100; j++)
//                QVERIFY(abs(img_conv2d_fast.pixel(i, j) - img_conv2d.pixel(i, j)) < 3);
//    }

//    void test_alg_conv2d_avx2_valid()
//    {
//        Img<uint16_t> img(100, 100, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 100, rand() % 100, rand() % 256);
//        Img<uint16_t> img_conv2d = img._conv2d_Baseline(img.__getLaplacianKernel2D(3));
//        Img<uint16_t> img_conv2d_avx2 = img._conv2d_Avx2(img.__getLaplacianKernel2D(3));
//        for (int i = 0; i < 100; i++)
//            for (int j = 0; j < 100; j++)
//                QVERIFY(abs(img_conv2d_avx2.pixel(i, j) - img_conv2d.pixel(i, j)) < 3);
//    }

//    void test_alg_conv2d_cuda_valid()
//    {
//        Img<float> img(100, 100, 255);
//        for (int i = 0; i < 100; i++)
//            for (int j = 0; j < 100; j++)
//                img.setPixel(i, j, 0);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 100, rand() % 100, rand() % 256);
//        Img<float> img_conv2d = img._conv2d_Baseline(img.__getLaplacianKernel2D(3));
//        Img<float> img_conv2d_cuda = img._conv2d_Cuda(img.__getLaplacianKernel2D(3));
//        for (int i = 3; i < 96; i++)
//            for (int j = 3; j < 96; j++)
//            {
//                QVERIFY(abs(img_conv2d_cuda.pixel(i, j) - img_conv2d.pixel(i, j)) < 3);
//            }
//    }
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    void test_alg_conv2d_baseline_3x3_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Baseline(img.__getLaplacianKernel2D(3));
//        }
//    }

//    void test_alg_conv2d_fast_3x3_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Fast(img.__getLaplacianKernel2D(3));
//        }
//    }

//    void test_alg_conv2d_avx2_3x3_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Avx2(img.__getLaplacianKernel2D(3));
//        }
//    }

//    void test_alg_conv2d_cuda_3x3_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Cuda(img.__getLaplacianKernel2D(3));
//        }
//    }

//    void test_alg_conv2d_fast_7x7_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Fast(img.__getLaplacianKernel2D(7));
//        }
//    }

//    void test_alg_conv2d_avx2_7x7_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Avx2(img.__getLaplacianKernel2D(7));
//        }
//    }

//    void test_alg_conv2d_cuda_7x7_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Cuda(img.__getLaplacianKernel2D(7));
//        }
//    }

//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    void test_alg_conv2d_baseline_1x7_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Baseline(img.__getGaussianKernel1Dy(7, 0.1));
//        }
//    }

//    void test_alg_conv2d_fast_1x7_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Fast(img.__getGaussianKernel1Dy(7, 0.1));
//        }
//    }

//    void test_alg_conv2d_avx2_1x7_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Avx2(img.__getGaussianKernel1Dy(7, 0.1));
//        }
//    }

//    void test_alg_conv2d_cuda_1x7_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Cuda(img.__getGaussianKernel1Dy(7, 0.1));
//        }
//    }

//    void test_alg_conv2d_fast_1x21_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Fast(img.__getGaussianKernel1Dy(21, 0.1));
//        }
//    }

//    void test_alg_conv2d_avx2_1x21_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Avx2(img.__getGaussianKernel1Dy(21, 0.1));
//        }
//    }

//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    void test_alg_conv2d_baseline_7x1_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Baseline(img.__getGaussianKernel1Dx(7, 0.1));
//        }
//    }

//    void test_alg_conv2d_fast_7x1_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Fast(img.__getGaussianKernel1Dx(7, 0.1));
//        }
//    }

//    void test_alg_conv2d_avx2_7x1_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Avx2(img.__getGaussianKernel1Dx(7, 0.1));
//        }
//    }

//    void test_alg_conv2d_cuda_7x1_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Cuda(img.__getGaussianKernel1Dx(7, 0.1));
//        }
//    }

//    void test_alg_conv2d_fast_21x1_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Fast(img.__getGaussianKernel1Dx(21, 0.1));
//        }
//    }

//    void test_alg_conv2d_avx2_21x1_benchmark()
//    {
//        Img<uint16_t> img(10000, 10000, 255);
//        for (int i = 0; i < 1000; i++)
//            img.setPixel(rand() % 10000, rand() % 10000, rand() % 256);
//        QBENCHMARK
//        {
//            Img<uint16_t> img_conv2d = img._conv2d_Avx2(img.__getGaussianKernel1Dx(21, 0.1));
//        }
//    }
};

#endif // TESTER_H
