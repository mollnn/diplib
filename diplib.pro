QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# TO ENABLE OPENMP, CONFIG HERE
QMAKE_CXXFLAGS += -fopenmp
QMAKE_LIBDIR += .
LIBS += -fopenmp

# TO ENABLE INTEL SIMD, CONFIG HERE
QMAKE_CXXFLAGS += -msse -msse2 -msse3 -mavx -mavx2 -mfma

# TO ENABLE CUDA, CONFIG HERE
#LIBS +=     -L. -lImgAlg_Cuda -lcudart\
#     -L"C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.0/lib/x64/"
#INCLUDEPATH += "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.0/include"
#CUDA_DIR = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.0"
#INCLUDEPATH += $$CUDA_DIR/include
#QMAKE_LIBDIR += $$CUDA_DIR/lib/x64/
#QMAKE_LIBDIR += .

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ImgUtilTransformMatrix.cpp \
    ImgUtil_mat3.cpp \
    ImgUtil_vec3.cpp \
    QImagePlot.cpp \
    QImagePlotBox.cpp \
    Tester.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    ImgAlgFilter.h \
    ImgAlgMapping.h \
    ImgAlgSegment.h \
    ImgAlgTransform.h \
    ImgData.h \
    ImgFuncBlurSharpen.h \
    ImgFuncGeoTransform.h \
    ImgFuncGrayWindow.h \
    ImgConvert.h \
    Img.h \
    ImgFuncSegment.h \
    ImgUtilTransformMatrix.h \
    ImgUtil_mat3.h \
    ImgUtil_mathheader.h \
    ImgUtil_vec3.h \
    QImagePlot.h \
    QImagePlotBox.h \
    Tester.h \
    widget.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
