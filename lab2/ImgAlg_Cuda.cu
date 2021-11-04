#include <cuda_runtime.h>
#include <stdio.h>
#include <stdint.h>

template <typename T>
__global__ void __kernel_ImgAlgInterp_interpBilinear_cuda(T *dest_ptr_d, T *src_ptr_d, float *coords_d, int dest_width, int dest_height, int src_width, int src_height, T default_value)
{
    int dest_x = blockDim.x * blockIdx.x + threadIdx.x;
    int dest_y = blockDim.y * blockIdx.y + threadIdx.y;

    if (dest_x >= dest_width || dest_y >= dest_height)
        return;

    int dest_idx = dest_y * dest_width + dest_x;

    float x = coords_d[2 * dest_idx + 0];
    float y = coords_d[2 * dest_idx + 1];

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

    dest_ptr_d[dest_y * dest_width + dest_x] = v;
}

template <typename T>
void  __ImgAlgInterp_interpBilinear_cuda_(T *dest_ptr, T *src_ptr, float *coords, int dest_width, int dest_height, int src_width, int src_height, T default_value)
{
    int src_size = src_width * src_height;
    int dest_size = dest_width * dest_height;

    T *dest_ptr_d;
    T *src_ptr_d;
    float *coords_d;

    cudaMalloc(&dest_ptr_d, dest_size * sizeof(T));
    cudaMalloc(&src_ptr_d, src_size * sizeof(T));
    cudaMalloc(&coords_d, dest_size * 2 * sizeof(float));

    cudaMemcpy(src_ptr_d, src_ptr, src_size * sizeof(T), cudaMemcpyHostToDevice);
    cudaMemcpy(coords_d, coords, dest_size * 2 * sizeof(float), cudaMemcpyHostToDevice);

    int block_width = 8;
    int block_height = 8;

    int grid_width = (dest_width + block_width - 1) / block_width;
    int grid_height = (dest_height + block_height - 1) / block_height;

    __kernel_ImgAlgInterp_interpBilinear_cuda<T><<<dim3(grid_width, grid_height), dim3(block_width, block_height)>>>(dest_ptr_d, src_ptr_d, coords_d, dest_width, dest_height, src_width, src_height, default_value);
    cudaDeviceSynchronize();

    cudaMemcpy(dest_ptr, dest_ptr_d, dest_size * sizeof(T), cudaMemcpyDeviceToHost);

    cudaFree(dest_ptr_d);
    cudaFree(src_ptr_d);
    cudaFree(coords_d);
}

// template
// void  __declspec(dllexport) __ImgAlgInterp_interpBilinear_cuda <uint8_t>(uint8_t *dest_ptr, uint8_t *src_ptr, float *coords, int dest_width, int dest_height, int src_width, int src_height, uint8_t default_value);

// template
// void  __declspec(dllexport) __ImgAlgInterp_interpBilinear_cuda <uint16_t>(T *dest_ptr, T *src_ptr, float *coords, int dest_width, int dest_height, int src_width, int src_height, T default_value);

// template
// void  __declspec(dllexport) __ImgAlgInterp_interpBilinear_cuda <float>(T *dest_ptr, T *src_ptr, float *coords, int dest_width, int dest_height, int src_width, int src_height, T default_value);


extern "C" void __declspec(dllexport) __ImgAlgInterp_interpBilinear_cuda_epi8(uint8_t *dest_ptr, uint8_t *src_ptr, float *coords, int dest_width, int dest_height, int src_width, int src_height, uint8_t default_value)
{
    __ImgAlgInterp_interpBilinear_cuda_<uint8_t>(dest_ptr, src_ptr, coords, dest_width,dest_height,src_width,src_height,default_value);
}

extern "C" void __declspec(dllexport) __ImgAlgInterp_interpBilinear_cuda_epi16(uint16_t *dest_ptr, uint16_t *src_ptr, float *coords, int dest_width, int dest_height, int src_width, int src_height, uint16_t default_value)
{
    __ImgAlgInterp_interpBilinear_cuda_<uint16_t>(dest_ptr, src_ptr, coords, dest_width,dest_height,src_width,src_height,default_value);
}

extern "C" void __declspec(dllexport) __ImgAlgInterp_interpBilinear_cuda_ps(float *dest_ptr, float *src_ptr, float *coords, int dest_width, int dest_height, int src_width, int src_height, float default_value)
{
    __ImgAlgInterp_interpBilinear_cuda_<float>(dest_ptr, src_ptr, coords, dest_width,dest_height,src_width,src_height,default_value);
}

extern "C" void __declspec(dllexport) test()
{
    printf("ok");
}

// int main()
// {
//     int w = 10000;
//     int h = 10000;
//     uint16_t *a = (uint16_t *)malloc(w * h * sizeof(uint16_t));
//     uint16_t *b = (uint16_t *)malloc(w * h * sizeof(uint16_t));
//     float *c = (float *)malloc(w * h * 2 * sizeof(float));
//     for (int i = 0; i < w * h; i++)
//     {
//         c[i * 2 + 0] = i * i * 1e-4;
//         c[i * 2 + 1] = i * i * 1e-4;
//     }
//     __ImgAlgInterp_interpBilinear_cuda<uint16_t>(b, a, c, w, h, w, h, 0);
// }