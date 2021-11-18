#include <cuda_runtime.h>
#include <stdio.h>
#include <stdint.h>

#define DLL_EXP
#include "ImgAlg_Cuda.h"

/////////////////////////////////////////////////////
// ImgAlgInterp
/////////////////////////////////////////////////////

template <typename T>
__device__ T ___device_ImgAlgInterp_pxInterpBilinear_cuda_(float x, float y, T *src_ptr_d, int src_width, int src_height, T default_value)
{
    int x0 = x;
    int x1 = x0 + 1;
    int y0 = y;
    int y1 = y0 + 1;

    bool valid_x0 = 0 <= x0 && x0 < src_width;
    bool valid_x1 = 0 <= x1 && x1 < src_width;
    bool valid_y0 = 0 <= y0 && y0 < src_height;
    bool valid_y1 = 0 <= y1 && y1 < src_height;

    int idx00 = y0 * src_width + x0;
    int idx01 = y1 * src_width + x0;
    int idx10 = y0 * src_width + x1;
    int idx11 = y1 * src_width + x1;

    T val00 = valid_x0 && valid_y0 ? src_ptr_d[idx00] : default_value;
    T val01 = valid_x0 && valid_y1 ? src_ptr_d[idx01] : default_value;
    T val10 = valid_x1 && valid_y0 ? src_ptr_d[idx10] : default_value;
    T val11 = valid_x1 && valid_y1 ? src_ptr_d[idx11] : default_value;

    float v = ((x1 - x) * val00 + (x - x0) * val10) * (y1 - y) +
              ((x1 - x) * val01 + (x - x0) * val11) * (y - y0);

    return v;
}

template <typename T>
__global__ void __kernel_ImgAlgInterp_interpBilinear_cuda(T *dest_ptr_d, T *src_ptr_d, float *x_coords_d, float *y_coords_d, int dest_width, int dest_height, int src_width, int src_height, T default_value)
{
    int dest_x = blockDim.x * blockIdx.x + threadIdx.x;
    int dest_y = blockDim.y * blockIdx.y + threadIdx.y;

    if (dest_x >= dest_width || dest_y >= dest_height)
        return;

    int dest_idx = dest_y * dest_width + dest_x;

    float x = x_coords_d[dest_idx];
    float y = y_coords_d[dest_idx];

    dest_ptr_d[dest_y * dest_width + dest_x] = ___device_ImgAlgInterp_pxInterpBilinear_cuda_(x, y, src_ptr_d, src_width, src_height, default_value);
}

template <typename T>
void __ImgAlgInterp_interpBilinear_cuda_(T *dest_ptr, T *src_ptr, float *x_coords, float *y_coords, int dest_width, int dest_height, int src_width, int src_height, T default_value)
{
    int src_size = src_width * src_height;
    int dest_size = dest_width * dest_height;

    T *dest_ptr_d;
    T *src_ptr_d;
    float *x_coords_d;
    float *y_coords_d;

    cudaMalloc(&dest_ptr_d, dest_size * sizeof(T));
    cudaMalloc(&src_ptr_d, src_size * sizeof(T));
    cudaMalloc(&x_coords_d, dest_size * sizeof(float));
    cudaMalloc(&y_coords_d, dest_size * sizeof(float));

    cudaMemcpy(src_ptr_d, src_ptr, src_size * sizeof(T), cudaMemcpyHostToDevice);
    cudaMemcpy(x_coords_d, x_coords, dest_size * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(y_coords_d, y_coords, dest_size * sizeof(float), cudaMemcpyHostToDevice);

    int block_width = 16;
    int block_height = 16;

    int grid_width = (dest_width + block_width - 1) / block_width;
    int grid_height = (dest_height + block_height - 1) / block_height;

    __kernel_ImgAlgInterp_interpBilinear_cuda<T><<<dim3(grid_width, grid_height), dim3(block_width, block_height)>>>(dest_ptr_d, src_ptr_d, x_coords_d, y_coords_d, dest_width, dest_height, src_width, src_height, default_value);
    cudaDeviceSynchronize();

    cudaMemcpy(dest_ptr, dest_ptr_d, dest_size * sizeof(T), cudaMemcpyDeviceToHost);

    cudaFree(dest_ptr_d);
    cudaFree(src_ptr_d);
    cudaFree(x_coords_d);
    cudaFree(y_coords_d);
}

////////////////////////////////////////
// ImgAlgAffine
////////////////////////////////////////

__device__ void ___device_ImgAlgAffine_pxAffineTransformCoordEvaluate_cuda(float *x_coords_dest_d, float *y_coords_dest_d, int coords_idx, float *mat_d, float x, float y)
{
    float x1 = mat_d[0] * x + mat_d[1] * y + mat_d[2];
    float y1 = mat_d[3] * x + mat_d[4] * y + mat_d[5];
    x_coords_dest_d[coords_idx] = x1;
    y_coords_dest_d[coords_idx] = y1;
}

template <typename T>
__global__ void __kernel_ImgAlgAffine_affineTransform_cuda(T *dest_ptr_d, T *src_ptr_d, float *mat_d, int dest_width, int dest_height, int src_width, int src_height, T default_value)
{
    int dest_x = blockDim.x * blockIdx.x + threadIdx.x;
    int dest_y = blockDim.y * blockIdx.y + threadIdx.y;
    float src_x;
    float src_y;
    ___device_ImgAlgAffine_pxAffineTransformCoordEvaluate_cuda(&src_x, &src_y, 0, mat_d, dest_x, dest_y);
    // dest_ptr_d[dest_y * dest_width + dest_x] = ___device_ImgAlgInterp_pxInterpBilinear_cuda_<T>(src_x, src_y, src_ptr_d, src_width, src_height, default_value);
    dest_ptr_d[dest_y * dest_width + dest_x] = 127;
    dest_ptr_d[0] = 127;
    dest_ptr_d[1] = 127;
    dest_ptr_d[2] = 127;
}

template <typename T>
void __ImgAlgAffine_affineTransform_cuda_(T *dest_ptr, T *src_ptr, float *mat, int dest_width, int dest_height, int src_width, int src_height, T default_value)
{
    mat[8] = 111;
    int src_size = src_width * src_height;
    int dest_size = dest_width * dest_height;

    T *dest_ptr_d;
    T *src_ptr_d;
    float *mat_d;

    cudaMalloc(&dest_ptr_d, dest_size * sizeof(T));
    cudaMalloc(&src_ptr_d, src_size * sizeof(T));
    cudaMalloc(&mat_d, 9 * sizeof(float));

    cudaMemcpy(src_ptr_d, src_ptr, src_size * sizeof(T), cudaMemcpyHostToDevice);
    cudaMemcpy(mat_d, mat, 9 * sizeof(float), cudaMemcpyHostToDevice);

    int block_width = 16;
    int block_height = 16;

    int grid_width = (dest_width + block_width - 1) / block_width;
    int grid_height = (dest_height + block_height - 1) / block_height;

    __kernel_ImgAlgAffine_affineTransform_cuda<T><<<dim3(grid_width, grid_height), dim3(block_width, block_height)>>>(dest_ptr_d, src_ptr_d, mat_d, dest_width, dest_height, src_width, src_height, default_value);
    cudaDeviceSynchronize();

    cudaMemcpy(dest_ptr, dest_ptr_d, dest_size * sizeof(T), cudaMemcpyDeviceToHost);

    cudaFree(dest_ptr_d);
    cudaFree(src_ptr_d);
    cudaFree(mat_d);
}

template <typename T>
__global__ void __kernel_Img_conv2d_cuda(T *dest_ptr_d, T *src_ptr_d, float *kernel_d, int dest_width, int dest_height, int src_width, int src_height, int kernel_width, int kernel_height, T default_value)
{
    int dest_x = blockDim.x * blockIdx.x + threadIdx.x;
    int dest_y = blockDim.y * blockIdx.y + threadIdx.y;

    if (!(dest_x >= 0 && dest_y >= 0 && dest_x < dest_width && dest_y < dest_height))
        return;

    int kernel_width_half = kernel_width / 2;
    int kernel_height_half = kernel_height / 2;

    float local_sum = 0;

    for (int i = -kernel_height_half; i <= kernel_height_half; i++)
    {
        for (int j = -kernel_width_half; j <= kernel_width_half; j++)
        {
            int kernel_x = j + kernel_width_half;
            int kernel_y = i + kernel_height_half;
            int src_x = dest_x - j;
            int src_y = dest_y - i;

            float kernel_value = kernel_d[kernel_y * kernel_width + kernel_x];

            // 分支导致了线程束分化（但是开销比 padding 小）
            if (src_x >= 0 && src_y >= 0 && src_x < src_width && src_y < src_height)
            {
                local_sum += src_ptr_d[src_y * src_width + src_x] * kernel_value;
            }
            else
            {
                local_sum += default_value * kernel_value;
            }
        }
    }

    dest_ptr_d[dest_y * dest_width + dest_x] = local_sum;
}

template <typename T>
void __Img_conv2d_cuda_(T *dest_ptr, T *src_ptr, float *kernel, int dest_width, int dest_height, int src_width, int src_height, int kernel_width, int kernel_height, T default_value)
{
    int src_size = src_width * src_height;
    int dest_size = dest_width * dest_height;
    int kernel_size = kernel_width * kernel_height;

    T *dest_ptr_d;
    T *src_ptr_d;
    float *kernel_d;

    cudaMalloc(&dest_ptr_d, dest_size * sizeof(T));
    cudaMalloc(&src_ptr_d, src_size * sizeof(T));
    cudaMalloc(&kernel_d, kernel_size * sizeof(float));

    cudaMemcpy(src_ptr_d, src_ptr, src_size * sizeof(T), cudaMemcpyHostToDevice);
    cudaMemcpy(kernel_d, kernel, kernel_size * sizeof(float), cudaMemcpyHostToDevice);

    int block_width = 16;
    int block_height = 16;

    int grid_width = (dest_width + block_width - 1) / block_width;
    int grid_height = (dest_height + block_height - 1) / block_height;

    __kernel_Img_conv2d_cuda<T><<<dim3(grid_width, grid_height), dim3(block_width, block_height)>>>(dest_ptr_d, src_ptr_d, kernel_d, dest_width, dest_height, src_width, src_height, kernel_width, kernel_height, default_value);
    cudaDeviceSynchronize();

    cudaMemcpy(dest_ptr, dest_ptr_d, dest_size * sizeof(T), cudaMemcpyDeviceToHost);

    cudaFree(dest_ptr_d);
    cudaFree(src_ptr_d);
    cudaFree(kernel_d);
}

//////////////////////////////////////////
//// Specifications (for export)
//// MinGW 与 MSVC 的 C++ ABI 不兼容，故采用 C 风格导出
//////////////////////////////////////////

extern "C" void __ImgAlgInterp_interpBilinear_cuda_epu8(uint8_t *dest_ptr, uint8_t *src_ptr, float *x_coords, float *y_coords, int dest_width, int dest_height, int src_width, int src_height, uint8_t default_value)
{
    __ImgAlgInterp_interpBilinear_cuda_<uint8_t>(dest_ptr, src_ptr, x_coords, y_coords, dest_width, dest_height, src_width, src_height, default_value);
}

extern "C" void __ImgAlgInterp_interpBilinear_cuda_epu16(uint16_t *dest_ptr, uint16_t *src_ptr, float *x_coords, float *y_coords, int dest_width, int dest_height, int src_width, int src_height, uint16_t default_value)
{
    __ImgAlgInterp_interpBilinear_cuda_<uint16_t>(dest_ptr, src_ptr, x_coords, y_coords, dest_width, dest_height, src_width, src_height, default_value);
}

extern "C" void __ImgAlgInterp_interpBilinear_cuda_ps(float *dest_ptr, float *src_ptr, float *x_coords, float *y_coords, int dest_width, int dest_height, int src_width, int src_height, float default_value)
{
    __ImgAlgInterp_interpBilinear_cuda_<float>(dest_ptr, src_ptr, x_coords, y_coords, dest_width, dest_height, src_width, src_height, default_value);
}

extern "C" void __ImgAlgAffine_affineTransform_cuda_epu8(uint8_t *dest_ptr, uint8_t *src_ptr, float *mat, int dest_width, int dest_height, int src_width, int src_height, uint8_t default_value)
{
    __ImgAlgAffine_affineTransform_cuda_<uint8_t>(dest_ptr, src_ptr, mat, dest_width, dest_height, src_width, src_height, default_value);
}

extern "C" void __ImgAlgAffine_affineTransform_cuda_epu16(uint16_t *dest_ptr, uint16_t *src_ptr, float *mat, int dest_width, int dest_height, int src_width, int src_height, uint16_t default_value)
{
    __ImgAlgAffine_affineTransform_cuda_<uint16_t>(dest_ptr, src_ptr, mat, dest_width, dest_height, src_width, src_height, default_value);
}

extern "C" void __ImgAlgAffine_affineTransform_cuda_ps(float *dest_ptr, float *src_ptr, float *mat, int dest_width, int dest_height, int src_width, int src_height, float default_value)
{
    __ImgAlgAffine_affineTransform_cuda_<float>(dest_ptr, src_ptr, mat, dest_width, dest_height, src_width, src_height, default_value);
}

extern "C" void __Img_conv2d_cuda_epu8(uint8_t *dest_ptr, uint8_t *src_ptr, float *kernel, int dest_width, int dest_height, int src_width, int src_height, int kernel_width, int kernel_height, uint8_t default_value)
{
    __Img_conv2d_cuda_<uint8_t>(dest_ptr, src_ptr, kernel, dest_width, dest_height, src_width, src_height, kernel_width, kernel_height, default_value);
}

extern "C" void __Img_conv2d_cuda_epu16(uint16_t *dest_ptr, uint16_t *src_ptr, float *kernel, int dest_width, int dest_height, int src_width, int src_height, int kernel_width, int kernel_height, uint16_t default_value)
{
    __Img_conv2d_cuda_<uint16_t>(dest_ptr, src_ptr, kernel, dest_width, dest_height, src_width, src_height, kernel_width, kernel_height, default_value);
}

extern "C" void __Img_conv2d_cuda_ps(float *dest_ptr, float *src_ptr, float *kernel, int dest_width, int dest_height, int src_width, int src_height, int kernel_width, int kernel_height, float default_value)
{
    __Img_conv2d_cuda_<float>(dest_ptr, src_ptr, kernel, dest_width, dest_height, src_width, src_height, kernel_width, kernel_height, default_value);
}
