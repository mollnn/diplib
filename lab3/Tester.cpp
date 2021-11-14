#include "Tester.h"
#include "Img.h"

#include <QDebug>

//    Average time usage (ms) Per 1 Mega Pixels
//    Release Profile
//    Intel(R) Core i5-11300H @ 3.10GHz (4 cores 320KB/5MB/8MB)
//    Mem 3200MHz
//    NVIDIA GeForce MX450 (14 SM × 64 SP/SM, 80GBps, 2.2 TFlops)

//    Crop(Copy) Serial		0.94
//    Crop(Copy) OMP		0.61

//    GrayW Serial          4.47
//    GrayW OMP             1.15
//    GrayW OMP+LUT         0.93

//    Affine Serial         24.68
//    Affine OpenMP         6.25
//    + matmul expansion	4.87
//    Affine AVX2           3.10
//    + avx2 -> avx512		...
//    + float->int          ...
//    Affine CUDA (no texm)	2.49
