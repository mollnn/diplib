#ifndef _IMGALG_CUDA_H_
#define _IMGALG_CUDA_H_

#include <stdio.h>
#include <stdint.h>

#ifdef DLL_EXP
#include <cuda_runtime.h>
#define DECL_DLL __declspec(dllexport)
#else
#define DECL_DLL __declspec(dllimport)
#endif

// Stupid verision, where ~90% time used in malloc and memcpy. It's not a good idea to pass precalculated coords as args.
extern "C" void DECL_DLL __ImgAlgInterp_interpBilinear_cuda_epu8(uint8_t *dest_ptr, uint8_t *src_ptr, float *x_coords, float *y_coords, int dest_width, int dest_height, int src_width, int src_height, uint8_t default_value);
extern "C" void DECL_DLL __ImgAlgInterp_interpBilinear_cuda_epu16(uint16_t *dest_ptr, uint16_t *src_ptr, float *x_coords, float *y_coords, int dest_width, int dest_height, int src_width, int src_height, uint16_t default_value);
extern "C" void DECL_DLL __ImgAlgInterp_interpBilinear_cuda_ps(float *dest_ptr, float *src_ptr, float *x_coords, float *y_coords, int dest_width, int dest_height, int src_width, int src_height, float default_value);

extern "C" void DECL_DLL __ImgAlgAffine_affineTransform_cuda_epu8(uint8_t *dest_ptr, uint8_t *src_ptr, float *mat, int dest_width, int dest_height, int src_width, int src_height, uint8_t default_value);
extern "C" void DECL_DLL __ImgAlgAffine_affineTransform_cuda_epu16(uint16_t *dest_ptr, uint16_t *src_ptr, float *mat, int dest_width, int dest_height, int src_width, int src_height, uint16_t default_value);
extern "C" void DECL_DLL __ImgAlgAffine_affineTransform_cuda_ps(float *dest_ptr, float *src_ptr, float *mat, int dest_width, int dest_height, int src_width, int src_height, float default_value);

extern "C" void DECL_DLL __Img_conv2d_cuda_epu8(uint8_t *dest_ptr, uint8_t *src_ptr, float *kernel, int dest_width, int dest_height, int src_width, int src_height, int kernel_width, int kernel_height, uint8_t default_value);
extern "C" void DECL_DLL __Img_conv2d_cuda_epu16(uint16_t *dest_ptr, uint16_t *src_ptr, float *kernel, int dest_width, int dest_height, int src_width, int src_height, int kernel_width, int kernel_height, uint16_t default_value);
extern "C" void DECL_DLL __Img_conv2d_cuda_ps(float *dest_ptr, float *src_ptr, float *kernel, int dest_width, int dest_height, int src_width, int src_height, int kernel_width, int kernel_height, float default_value);

#endif