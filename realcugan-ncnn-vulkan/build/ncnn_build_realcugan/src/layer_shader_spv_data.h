// Layer Shader Spv Data header
//
// This file is auto-generated by cmake, don't edit it.

#include "layer/vulkan/shader/convolution.comp.hex.h"
#include "layer/vulkan/shader/convolution_1x1s1d1.comp.hex.h"
#include "layer/vulkan/shader/convolution_3x3s1d1_winograd23_transform_input.comp.hex.h"
#include "layer/vulkan/shader/convolution_3x3s1d1_winograd23_transform_output.comp.hex.h"
#include "layer/vulkan/shader/convolution_3x3s1d1_winograd43_transform_input.comp.hex.h"
#include "layer/vulkan/shader/convolution_3x3s1d1_winograd43_transform_output.comp.hex.h"
#include "layer/vulkan/shader/convolution_3x3s1d1_winograd_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack1to4.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack1to4_1x1s1d1.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack1to4_3x3s1d1_winograd_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack1to4_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack1to8.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack1to8_1x1s1d1.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack1to8_3x3s1d1_winograd_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack1to8_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_1x1s1d1.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_1x1s1d1_khr_cm_16_16_16.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_1x1s1d1_khr_cm_16_8_8.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_1x1s1d1_nv_cm_16_16_16.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_1x1s1d1_nv_cm_16_8_8.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_3x3s1d1_winograd23_transform_input.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_3x3s1d1_winograd23_transform_output.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_3x3s1d1_winograd43_transform_input.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_3x3s1d1_winograd43_transform_output.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_3x3s1d1_winograd_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_3x3s1d1_winograd_gemm_khr_cm_16_16_16.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_3x3s1d1_winograd_gemm_khr_cm_16_8_8.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_3x3s1d1_winograd_gemm_nv_cm_16_16_16.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_3x3s1d1_winograd_gemm_nv_cm_16_8_8.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_gemm_khr_cm_16_16_16.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_gemm_khr_cm_16_8_8.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_gemm_nv_cm_16_16_16.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4_gemm_nv_cm_16_8_8.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4to1.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4to1_1x1s1d1.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4to1_3x3s1d1_winograd_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4to1_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4to8.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4to8_1x1s1d1.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4to8_3x3s1d1_winograd_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack4to8_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8_1x1s1d1.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8_3x3s1d1_winograd23_transform_input.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8_3x3s1d1_winograd23_transform_output.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8_3x3s1d1_winograd43_transform_input.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8_3x3s1d1_winograd43_transform_output.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8_3x3s1d1_winograd_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8to1.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8to1_1x1s1d1.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8to1_3x3s1d1_winograd_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8to1_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8to4.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8to4_1x1s1d1.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8to4_3x3s1d1_winograd_gemm.comp.hex.h"
#include "layer/vulkan/shader/convolution_pack8to4_gemm.comp.hex.h"
#include "layer/vulkan/shader/crop.comp.hex.h"
#include "layer/vulkan/shader/crop_pack1to4.comp.hex.h"
#include "layer/vulkan/shader/crop_pack1to8.comp.hex.h"
#include "layer/vulkan/shader/crop_pack4.comp.hex.h"
#include "layer/vulkan/shader/crop_pack4to1.comp.hex.h"
#include "layer/vulkan/shader/crop_pack4to8.comp.hex.h"
#include "layer/vulkan/shader/crop_pack8.comp.hex.h"
#include "layer/vulkan/shader/crop_pack8to1.comp.hex.h"
#include "layer/vulkan/shader/crop_pack8to4.comp.hex.h"
#include "layer/vulkan/shader/deconvolution.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_col2im.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_gemm.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack1to4.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack1to4_gemm.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack1to8.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack1to8_gemm.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack4.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack4_col2im.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack4_gemm.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack4_gemm_khr_cm_16_16_16.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack4_gemm_khr_cm_16_8_8.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack4_gemm_nv_cm_16_16_16.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack4_gemm_nv_cm_16_8_8.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack4to1.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack4to1_gemm.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack4to8.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack4to8_gemm.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack8.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack8_col2im.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack8_gemm.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack8to1.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack8to1_gemm.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack8to4.comp.hex.h"
#include "layer/vulkan/shader/deconvolution_pack8to4_gemm.comp.hex.h"
#include "layer/vulkan/shader/eltwise.comp.hex.h"
#include "layer/vulkan/shader/eltwise_pack4.comp.hex.h"
#include "layer/vulkan/shader/eltwise_pack8.comp.hex.h"
#include "layer/vulkan/shader/flatten.comp.hex.h"
#include "layer/vulkan/shader/flatten_pack1to4.comp.hex.h"
#include "layer/vulkan/shader/flatten_pack1to8.comp.hex.h"
#include "layer/vulkan/shader/flatten_pack4.comp.hex.h"
#include "layer/vulkan/shader/flatten_pack4to8.comp.hex.h"
#include "layer/vulkan/shader/flatten_pack8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_gemm.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_gemm_wp1to4.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_gemm_wp1to8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_gemm_wp4.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_gemm_wp4to1.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_gemm_wp4to8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_gemm_wp8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_gemm_wp8to1.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_gemm_wp8to4.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_pack1to4.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_pack1to8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_pack4.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_pack4to1.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_pack4to8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_pack8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_pack8to1.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_pack8to4.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_reduce_sum8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_reduce_sum8_pack4.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_reduce_sum8_pack8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_sum8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_sum8_pack1to4.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_sum8_pack1to8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_sum8_pack4.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_sum8_pack4to1.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_sum8_pack4to8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_sum8_pack8.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_sum8_pack8to1.comp.hex.h"
#include "layer/vulkan/shader/innerproduct_sum8_pack8to4.comp.hex.h"
#include "layer/vulkan/shader/pooling.comp.hex.h"
#include "layer/vulkan/shader/pooling_adaptive.comp.hex.h"
#include "layer/vulkan/shader/pooling_adaptive_pack4.comp.hex.h"
#include "layer/vulkan/shader/pooling_adaptive_pack8.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_max.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_max_first.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_max_first_pack4.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_max_first_pack8.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_max_last.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_max_last_pack4.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_max_last_pack8.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_max_pack4.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_max_pack8.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_sum.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_sum_first.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_sum_first_pack4.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_sum_first_pack8.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_sum_last.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_sum_last_pack4.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_sum_last_pack8.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_sum_pack4.comp.hex.h"
#include "layer/vulkan/shader/pooling_global_reduce_sum_pack8.comp.hex.h"
#include "layer/vulkan/shader/pooling_pack4.comp.hex.h"
#include "layer/vulkan/shader/pooling_pack8.comp.hex.h"
#include "layer/vulkan/shader/relu.comp.hex.h"
#include "layer/vulkan/shader/relu_pack4.comp.hex.h"
#include "layer/vulkan/shader/relu_pack8.comp.hex.h"
#include "layer/vulkan/shader/scale.comp.hex.h"
#include "layer/vulkan/shader/scale_pack4.comp.hex.h"
#include "layer/vulkan/shader/scale_pack8.comp.hex.h"
#include "layer/vulkan/shader/sigmoid.comp.hex.h"
#include "layer/vulkan/shader/sigmoid_pack4.comp.hex.h"
#include "layer/vulkan/shader/sigmoid_pack8.comp.hex.h"
#include "layer/vulkan/shader/binaryop.comp.hex.h"
#include "layer/vulkan/shader/binaryop_broadcast.comp.hex.h"
#include "layer/vulkan/shader/binaryop_broadcast_pack1to4.comp.hex.h"
#include "layer/vulkan/shader/binaryop_broadcast_pack1to8.comp.hex.h"
#include "layer/vulkan/shader/binaryop_broadcast_pack4.comp.hex.h"
#include "layer/vulkan/shader/binaryop_broadcast_pack8.comp.hex.h"
#include "layer/vulkan/shader/binaryop_pack4.comp.hex.h"
#include "layer/vulkan/shader/binaryop_pack8.comp.hex.h"
#include "layer/vulkan/shader/padding.comp.hex.h"
#include "layer/vulkan/shader/padding_3d.comp.hex.h"
#include "layer/vulkan/shader/padding_3d_pack4.comp.hex.h"
#include "layer/vulkan/shader/padding_3d_pack8.comp.hex.h"
#include "layer/vulkan/shader/padding_pack1to4.comp.hex.h"
#include "layer/vulkan/shader/padding_pack1to8.comp.hex.h"
#include "layer/vulkan/shader/padding_pack4.comp.hex.h"
#include "layer/vulkan/shader/padding_pack4to1.comp.hex.h"
#include "layer/vulkan/shader/padding_pack4to8.comp.hex.h"
#include "layer/vulkan/shader/padding_pack8.comp.hex.h"
#include "layer/vulkan/shader/padding_pack8to1.comp.hex.h"
#include "layer/vulkan/shader/padding_pack8to4.comp.hex.h"
#include "layer/vulkan/shader/interp.comp.hex.h"
#include "layer/vulkan/shader/interp_bicubic.comp.hex.h"
#include "layer/vulkan/shader/interp_bicubic_coeffs.comp.hex.h"
#include "layer/vulkan/shader/interp_bicubic_pack4.comp.hex.h"
#include "layer/vulkan/shader/interp_bicubic_pack8.comp.hex.h"
#include "layer/vulkan/shader/interp_pack4.comp.hex.h"
#include "layer/vulkan/shader/interp_pack8.comp.hex.h"
#include "layer/vulkan/shader/packing.comp.hex.h"
#include "layer/vulkan/shader/packing_fp16_to_fp32.comp.hex.h"
#include "layer/vulkan/shader/packing_fp32_to_fp16.comp.hex.h"
#include "layer/vulkan/shader/packing_pack1to4.comp.hex.h"
#include "layer/vulkan/shader/packing_pack1to4_fp16_to_fp32.comp.hex.h"
#include "layer/vulkan/shader/packing_pack1to4_fp32_to_fp16.comp.hex.h"
#include "layer/vulkan/shader/packing_pack1to8.comp.hex.h"
#include "layer/vulkan/shader/packing_pack1to8_fp16_to_fp32.comp.hex.h"
#include "layer/vulkan/shader/packing_pack1to8_fp32_to_fp16.comp.hex.h"
#include "layer/vulkan/shader/packing_pack4.comp.hex.h"
#include "layer/vulkan/shader/packing_pack4_fp16_to_fp32.comp.hex.h"
#include "layer/vulkan/shader/packing_pack4_fp32_to_fp16.comp.hex.h"
#include "layer/vulkan/shader/packing_pack4to1.comp.hex.h"
#include "layer/vulkan/shader/packing_pack4to1_fp16_to_fp32.comp.hex.h"
#include "layer/vulkan/shader/packing_pack4to1_fp32_to_fp16.comp.hex.h"
#include "layer/vulkan/shader/packing_pack4to8.comp.hex.h"
#include "layer/vulkan/shader/packing_pack4to8_fp16_to_fp32.comp.hex.h"
#include "layer/vulkan/shader/packing_pack4to8_fp32_to_fp16.comp.hex.h"
#include "layer/vulkan/shader/packing_pack8.comp.hex.h"
#include "layer/vulkan/shader/packing_pack8_fp16_to_fp32.comp.hex.h"
#include "layer/vulkan/shader/packing_pack8_fp32_to_fp16.comp.hex.h"
#include "layer/vulkan/shader/packing_pack8to1.comp.hex.h"
#include "layer/vulkan/shader/packing_pack8to1_fp16_to_fp32.comp.hex.h"
#include "layer/vulkan/shader/packing_pack8to1_fp32_to_fp16.comp.hex.h"
#include "layer/vulkan/shader/packing_pack8to4.comp.hex.h"
#include "layer/vulkan/shader/packing_pack8to4_fp16_to_fp32.comp.hex.h"
#include "layer/vulkan/shader/packing_pack8to4_fp32_to_fp16.comp.hex.h"
#include "layer/vulkan/shader/cast_fp16_to_fp32.comp.hex.h"
#include "layer/vulkan/shader/cast_fp16_to_fp32_pack4.comp.hex.h"
#include "layer/vulkan/shader/cast_fp16_to_fp32_pack8.comp.hex.h"
#include "layer/vulkan/shader/cast_fp32_to_fp16.comp.hex.h"
#include "layer/vulkan/shader/cast_fp32_to_fp16_pack4.comp.hex.h"
#include "layer/vulkan/shader/cast_fp32_to_fp16_pack8.comp.hex.h"
#include "layer/vulkan/shader/pixelshuffle.comp.hex.h"
#include "layer/vulkan/shader/pixelshuffle_pack4.comp.hex.h"
#include "layer/vulkan/shader/pixelshuffle_pack4to1.comp.hex.h"
#include "layer/vulkan/shader/pixelshuffle_pack8.comp.hex.h"
#include "layer/vulkan/shader/pixelshuffle_pack8to1.comp.hex.h"
#include "layer/vulkan/shader/pixelshuffle_pack8to4.comp.hex.h"
#include "layer/vulkan/shader/erf.comp.hex.h"
#include "layer/vulkan/shader/erf_pack4.comp.hex.h"
#include "layer/vulkan/shader/erf_pack8.comp.hex.h"
#include "layer/vulkan/shader/celu.comp.hex.h"
#include "layer/vulkan/shader/celu_pack4.comp.hex.h"
#include "layer/vulkan/shader/celu_pack8.comp.hex.h"
#include "layer/vulkan/shader/convert_ycbcr.comp.hex.h"
#include "layer/vulkan/shader/vulkan_activation.comp.hex.h"
