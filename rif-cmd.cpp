#include "RadeonImageFilter/include/RadeonImageFilters.h"
#include "RadeonImageFilter/include/RadeonImageFilters_version.h"

#define STB_IMAGE_IMPLEMENTATION
#include "RadeonImageFilter/samples/ImageTools/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "RadeonImageFilter/samples/ImageTools/stb_image_write.h"

#include "RadeonImageFilter/samples/ImageTools/ImageTools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <malloc.h>

// g++ rif-cmd.cpp -L./RadeonImageFilter/Ubuntu18/Dynamic/ -L./RadeonImageFilter/Ubuntu18/Static/ -lRadeonImageFilters -o rif-cmd

#if defined(WIN32) || defined(_WIN32)
   #define BACKEND_TYPE RIF_BACKEND_API_DIRECTX12
#elif __APPLE__
   #define BACKEND_TYPE RIF_BACKEND_API_METAL
#else
   #define BACKEND_TYPE RIF_BACKEND_API_OPENCL
#endif

rif_image_filter set_param(rif_context       context,
                           const rif_char    *filter_name,
                           rif_image_filter  filter,
                           rif_bool          use_default,
                           rif_image_desc    *output_desc)
{
   int i;
// Rotating Filters
   if (!strcmp("flip_h", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/horizontal_flip.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_FLIP_HOR,
                                  &filter);

   } else if (!strcmp("flip_v", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/vertical_flip.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_FLIP_VERT,
                                  &filter);

   } else if (!strcmp("rotate", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/rotation.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_ROTATE,
                                  &filter);

      output_desc->image_width  = output_desc->image_height;
      output_desc->image_height = output_desc->image_width;

// Blurring and Resampling Filters
   } else if (!strcmp("ai_upscale", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ai_upscale.html
      /*
       *    not work with Polaris11(gfx803) + MIOpen v2.0.5
       */
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_AI_UPSCALE,
                                  &filter);

      rif_char ret_model_path[128] = "./RadeonImageFilter/models";

      rifImageFilterSetParameterString(filter, "modelPath", ret_model_path);

      if (!use_default) {
         rif_uint ret_param;
            //    #define RIF_AI_UPSCALE_MODE_GOOD_2X 0x1u
            //    #define RIF_AI_UPSCALE_MODE_BEST_2X 0x2u
            //    #define RIF_AI_UPSCALE_MODE_FAST_2X 0x3u
         printf("Select upscale mode : \n"
                " [1: Good (Default)] \n [2: Best] \n [3: Fast] \n");
            scanf("%1u%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1u(filter, "mode", ret_param);
         
         printf("Path to model files (Default: ./RadeonImageFilter/models) \n"
                " (enter \".\" if use default path) : ");
            scanf("%127s%*[^\n]", &ret_model_path);

         if (strcmp(".", ret_model_path))
            rifImageFilterSetParameterString(filter, "modelPath", ret_model_path);
      }

   } else if (!strcmp("gaussian_blur", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/gaussian_blurring.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_GAUSSIAN_BLUR,
                                  &filter);

      if (!use_default) {
         rif_uint    ret_param[2];
         rif_float   ret_sigma;

         printf("The radius of the region that is used for blurring [Default: 1] [1, 50] : ");
            scanf("%u%*[^\n]", &ret_param[0]);
         printf("Parameter of the decrease of the Gaussian function [Default: 1] : ");
            scanf("%f%*[^\n]", &ret_sigma);
         printf("Enables use of local memory [Default: false] [0 (false), 1 (true)] : ");
            scanf("%u%*[^\n]", &ret_param[1]);

         rifImageFilterSetParameter1u(filter, "radius",  ret_param[0]);
         rifImageFilterSetParameter1u(filter, "tiling",  ret_param[1]);

         rifImageFilterSetParameter1f(filter, "sigma",   ret_sigma);
      }

   } else if (!strcmp("motion_blur", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/motion_blur.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_MOTION_BLUR,
                                  &filter);
      /*
       *    RIF 1.6.2 workaround
       *    https://github.com/GPUOpen-LibrariesAndSDKs/RadeonImageFilter/issues/8
       */
         rifImageFilterSetParameter1u(filter, "radius", 4u);
         rifImageFilterSetParameter1u(filter, "radius", 5u);

      if (!use_default) {
         rif_uint    ret_radius;
         rif_float   ret_param[2];
   
         printf("Radius [Default: 5] [0, 50] : ");
            scanf("%2u%*[^\n]", &ret_radius);
         rifImageFilterSetParameter1u(filter, "radius", ret_radius);
   
         printf("Motion direction [Default: (1, 1)] : ");
            scanf("%f, %f%*[^\n]", &ret_param[0], &ret_param[1]);
         rifImageFilterSetParameter2f(filter, "direction", ret_param[0], ret_param[1]);
      }

   } else if (!strcmp("resample", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/resampling.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_RESAMPLE,
                                  &filter);
      
         rif_float ret_scale = 2.0;

      if (!use_default) {
         rif_uint ret_operator;

         printf("Interpolation operator : \n"
                "[0]: Nearest,                     [1]: Bilinear\n"
                "[2]: Bicubic,                     [3]: Lanczos\n"
                );
            scanf("%u%*[^\n]", &ret_operator);

         rifImageFilterSetParameter1u(filter, "interpOperator", ret_operator);

         printf("Scale {n}x [Default: 2] : ");
            scanf("%f%*[^\n]", &ret_scale);
      }

      output_desc->image_width  = (rif_uint)(output_desc->image_width  * ret_scale);
      output_desc->image_height = (rif_uint)(output_desc->image_height * ret_scale);

/*
      status = rifContextCreateImage(context, &output_desc, nullptr, &outputImage);
         if (status != RIF_SUCCESS) return NULL;
*/
      rifImageFilterSetParameter2u(filter, "outSize",
                                   output_desc->image_width,
                                   output_desc->image_height);


   } else if (!strcmp("dynamic_resample", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/resampling.html
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/info_setting_types/rif_image_filter_interpolation_operator.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_RESAMPLE_DYNAMIC,
                                  &filter);
      
         rif_float   ret_sharp = 1.0,  ret_scale = 2.0;
         rif_uint    ret_operator = 0, offset = 4;

//         rifImageFilterSetParameter1u(filter, "interpOperator", RIF_IMAGE_INTERPOLATION_LANCZOS4);
//         rifImageFilterSetParameter1f(filter, "sharpness", ret_sharp);

      if (!use_default) {
//         rif_uint ret_operator, offset = 4;

         printf("Interpolation operator : \n"
                "[0]: Lanczos4,                    [1]: Lanczos6\n"
                "[2]: Lanczos12,                   [3]: Lanczos3\n"
                "[4]: Kaiser,                      [5]: Blackman\n"
                "[6]: Gauss,                       [7]: Box\n"
                "[8]: Tent,                        [9]: Bell\n"
                "[10]: B-spline,                   [11]: Quadratic Interpolation\n"
                "[12]: Quadratic approximation,    [13]: Quadratic mix\n"
                "[14]: Mitchell,                   [15]: Catmull\n"
                );
            scanf("%u%*[^\n]", &ret_operator);

         rifImageFilterSetParameter1u(filter, "interpOperator", ret_operator + offset);

         printf("Scale {n}x [Default: 2] : ");
            scanf("%f%*[^\n]", &ret_scale);

         printf("Sharpness [Default: 1] [0.2, 5] : ");
            scanf("%f%*[^\n]", &ret_sharp);

         rifImageFilterSetParameter1f(filter, "sharpness", ret_sharp);
      }

      output_desc->image_width  = (rif_uint)(output_desc->image_width  * ret_scale);
      output_desc->image_height = (rif_uint)(output_desc->image_height * ret_scale);

/*
      status = rifContextCreateImage(context, &output_desc, nullptr, &outputImage);
         if (status != RIF_SUCCESS) return NULL;
*/
      rifImageFilterSetParameter2u(filter, "outSize",
                                   output_desc->image_width,
                                   output_desc->image_height);

// Tone Mapping and Color Changing Filters
   } else if (!strcmp("filmic_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/aces_filmic_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_FILMIC_TONEMAP,
                                  &filter);

      if (!use_default) {
         rif_float ret_param[2];
         rif_bool  ret_apply;
   
         const rif_char *ret_param_desc[2] =
               {  "Input image pre-exposure coefficient [Default: 0] [0, 100] : " ,
                  "Input image pre-contrast coefficient [Default: 1] [0, 10] : "  };
         const rif_char *ret_param_name[2] = { "exposure", "contrast" };
   
         for (i=0; i < 2; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
   
         printf("Apply ToneMap ? [Default: false] [0 (false), 1 (true)] : ");
            scanf("%1d%*[^\n]", &ret_apply);
         rifImageFilterSetParameter1u(filter, "applyToneMap",  ret_apply);
      }

   } else if (!strcmp("autolinear_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/auto_linear_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_AUTOLINEAR_TONEMAP,
                                  &filter);

      if (!use_default) {
         rif_float ret_param;
   
         printf("Gamma correction coefficient [Default: 2.2] [0, 5] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "gamma", ret_param);
      }

   } else if (!strcmp("color_space", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/color_space_changing.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_COLOR_SPACE,
                                  &filter);

      if (!use_default) {
         rif_uint ret_param[1];
         const rif_char *ret_src[2] = { "srcColorSpace", "dstColorSpace" };
         const rif_char *ret_param_desc[2] = { "input", "output" };
   
         for (i=0; i < 2; i++) {
            printf("\nSelect color space of the %s image : \n [0: sRGB]\n [1: Adobe RGB]\n [2: Rec2020]\n [3: DCIP3]\n ", ret_param_desc[i]);
               scanf("%u", &ret_param[i]);
   
            switch (ret_param[i]) {
               case 0:
                  rifImageFilterSetParameter1u(filter,
                                               ret_src[i],
                                               RIF_COLOR_SPACE_SRGB);
                  break;
               case 1:
                  rifImageFilterSetParameter1u(filter,
                                               ret_src[i],
                                               RIF_COLOR_SPACE_ADOBE_RGB);
                  break;
               case 2:
                  rifImageFilterSetParameter1u(filter,
                                               ret_src[i],
                                               RIF_COLOR_SPACE_REC2020);
                  break;
               case 3:
                  rifImageFilterSetParameter1u(filter,
                                               ret_src[i],
                                               RIF_COLOR_SPACE_DCIP3);
                  break;
               default:
                  rifImageFilterSetParameter1u(filter,
                                               ret_src[i],
                                               RIF_COLOR_SPACE_SRGB);
                  break;
            }
         }
      }

   } else if (!strcmp("drago_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/drago_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_DRAGO_TONEMAP,
                                  &filter);

      if (!use_default) {
         rif_float ret_param[3];
   
         const rif_char *ret_param_desc[3] =
                           {  "Bias [Default: 1] : "                             ,
                              "Average input image luminance [Default: 0] : "    ,
                              "Maximum input image luminance [Default: 0.1] : "  };
         const rif_char *ret_param_name[3] =
                           { "bias", "avLum", "maxLum" };
   
         for (i=0; i < 3; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      }

   } else if (!strcmp("exponential_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/exponential_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_EXPONENTIAL_TONEMAP,
                                  &filter);

      if (!use_default) {
         rif_float ret_param[3];
   
         const rif_char *ret_param_desc[3] =
                  {  "Gamma correction coefficient [Default: 2.2] [0, 5] : "           ,
                     "Input image pre-exposure coefficient [Default: 0] [-10, 10] : "  ,
                     "Output image pre-exposure coefficient [Default: 1] [0, 100] : "  };
         const rif_char *ret_param_name[3] =
                  { "gamma", "exposure", "intensity" };
   
         for (i=0; i < 3; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      }

   } else if (!strcmp("filmic_uncharted_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/filmic_uncharted_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_FILMIC_UNCHARTED_TONEMAP,
                                  &filter);

      if (!use_default) {
         rif_float ret_param[9];
   
         const rif_char *ret_param_desc[9] =
               {  "Shoulder strength [Default: 0.15] [0, 5] : "                  ,
                  "Linear strength [Default: 0.5] [0, 1] : "                     ,
                  "Linear angle [Default: 0.5] [0, 1] : "                        ,
                  "Toe strength [Default: 0.2] [0, 1] : "                        ,
                  "Toe numerator [Default: 0.02] [0, 1] : "                      ,
                  "Toe denominator [Default: 0.3] [0, 1] : "                     ,
                  "White [Default: 11.2] [1, 20] : "                             ,
                  "Input image exposure coefficient [Default: 1] [-10, 10] : "   ,
                  "Gamma correction coefficient [Default: 2.2] [0, 5] : "        };
   
         const rif_char *ret_param_name[9] = { "A", "B", "C", "D", "E", "F", "W",
                                                 "exposure", "gamma" };
   
         for (i=0; i < 9; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      }


   } else if (!strcmp("gamma_correct", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/gamma_correction.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_GAMMA_CORRECTION,
                                  &filter);

      if (!use_default) {
         rif_float ret_param;
   
         printf("Gamma [Default: 1] [0, 5] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "gamma", ret_param);
      }

   } else if (!strcmp("hue_saturation", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/hue_saturation_changing.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_HUE_SATURATION,
                                  &filter);

      if (!use_default) {
         rif_float ret_param[3];
   
         const rif_char *ret_param_desc[3] =
                  {  "Saturation [Default: 1] : "              ,
                     "Hue [Default: 0] [-180, 180] : "         ,
                     "Brightness multiplier [Default: 1] : "   };
         const rif_char *ret_param_name[3] =
                  {  "saturation", "hue", "value" };
   
         for (i=0; i < 3; i++ ) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      }

   } else if (!strcmp("photo_linear_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/linear_photographic_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_PHOTO_LINEAR_TONEMAP,
                                  &filter);

      if (!use_default) {
         rif_float ret_param[4];
   
         const rif_char *ret_param_desc[4] =
                           {  "Gamma correction [Default: 2.2] [0, 5] : "        ,
                              "Film exposure time [Default: 0.125] [0, 100] : "  ,
                              "Luminance of the scene [Default: 1] [0, 100] : "  ,
                              "Aperture f-number [Default: 1] [0, 100] : "       };
   
         const rif_char *ret_param_name[4] = {   "gamma"        ,
                                                   "exposureTime" ,
                                                   "sensitivity"  ,
                                                   "fstop"        };
   
         for (i=0; i < 4; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      }

   } else if (!strcmp("linear_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/linear_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_LINEAR_TONEMAP,
                                  &filter);

      if (!use_default) {
         rif_float ret_param;
   
         printf("Linear coefficient [Default: 1] [0, 1e+4f(10000)] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "key", ret_param);
      }

   } else if (!strcmp("white_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/max_white_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_MAXWHITE_TONEMAP,
                                  &filter);

   } else if (!strcmp("photo_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/photographic_tone_mapper.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_PHOTO_TONEMAP,
                                  &filter);

      if (!use_default) {
         rif_float   ret_param[9];
         rif_float   ret_whitepoint[4];
         rif_uint    ret_iso;
         rif_uint    ret_apply[2];
   
   
         const rif_char *ret_param_desc[9] =
                           {  "Gamma correction [Default: 1] [0, 5] : "             ,
                              "Film exposure time [Default: 0.125] [0, 100] : "     ,
                              "Luminance of the scene [Default: 1] [0, 100] : "     ,
                              "Aperture f-number [Default: 1] [0.00001, 100] : "    ,
                              "Camera focal length [Default: 1] [0.00001, 100] : "  ,
                              "Vignette [Default: 0] [0, 1] : "                     ,
                              "Saturation [Default: 0.5] [0, 1] : "                 ,
                              "Highlight [Default: 0.5] [0, 10] : "                 ,
                              "Shadow [Default: 0.5] [0, 10] : "                    };
         const rif_char *ret_param_name[9] =
                           {  "gamma",      "exposureTime", "sensitivity"  ,
                              "fstop",      "focalL",       "vignette"     ,
                              "saturation", "lighten",      "darken"       };
   
         for (i=0; i < 9; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i],  ret_param[i]);
         }
   
         printf("Do negative values clamp to zero? [Default: true] [0 (false), 1 (true)] : ");
            scanf("%1u%*[^\n]", &ret_apply[0]);
         rifImageFilterSetParameter1u(filter, "useclampN",  ret_apply[0]);
   
         printf("Whitepoint [Default: (1, 1, 1, 1)] : ");
           scanf("%f, %f, %f, %f%*[^\n]",
                     &ret_whitepoint[0], &ret_whitepoint[1],
                     &ret_whitepoint[2], &ret_whitepoint[3]);

         rifImageFilterSetParameter4f(filter, "whitepoint",
                                       ret_whitepoint[0], ret_whitepoint[1],
                                       ret_whitepoint[2], ret_whitepoint[3]);
   
         printf("Film ISO [Default: 100] [100, 2000] : ");
            scanf("%5u%*[^\n]", &ret_iso);
         printf("Use ISO? [Default: 0 (false)] [0 (false), 1 (true)] : ");
            scanf("%1u%*[^\n]", &ret_apply[1]);

         rifImageFilterSetParameter1u(filter, "ISO",     ret_iso);
         rifImageFilterSetParameter1u(filter, "useISO",  ret_apply[1]);
      }

   } else if (!strcmp("reinhard_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/reinhard_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_REINHARD02_TONEMAP,
                                  &filter);

      if (!use_default) {
         rif_float ret_param[4];
   
         const rif_char *ret_param_desc[4] =
                  {  "Gamma correction [Default: 2.2] [0, 5] : "                 ,
                     "Controls the amount of light in the scene before tone mapping is applied  [Default: 1] [0, 10] : "   ,
                     "Controls the amount of light in the scene after tone mapping is applied  [Default: 1.2] [0, 10] : "  ,
                     "Please brightness parameter [Default: 0.375] [0, 10] : "   };
         const rif_char *ret_param_name[4] =
                  { "gamma", "preScale", "postScale", "burn" };
   
         for (i=0; i < 4; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      }
   /*
   } else if (!strcmp("ai_denoiser", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ai_denoiser.html
   */
   } else if (!strcmp("median_denoise", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/median_denoiser.html
      rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_MEDIAN_DENOISE,
                                          &filter);
 
      if (!use_default) {
         rif_uint ret_param;

         printf("Radius of the filter window [Default: 1] [0, 50]\n");
            scanf("%u%*[^\n]", &ret_param);

         rifImageFilterSetParameter1u(filter, "radius", ret_param);
      }

   } else if (!strcmp("laplace_edge_detect", filter_name) || !strcmp("laplace", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/laplace_edge_detection.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_LAPLACE,
                                  &filter);

   } else if (!strcmp("sobel_edge_detect", filter_name) || !strcmp("sobel", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/sobel_edge_detection.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_SOBEL,
                                  &filter);

   } else if (!strcmp("sharpness", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/sharpen.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_SHARPEN,
                                  &filter);

      if (!use_default) {
         rif_float ret_param;
   
         printf("Sharpness [Default: 0.5] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "sharpness", ret_param);
      }

   } else if (!strcmp("ai_custom", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ai_custom_model.html
      rifContextCreateImageFilter(context,
                                  RIF_CUSTOM_AI_MODEL,
                                  &filter);

      rif_char ret_model_path[128];
      printf("Path to machine learning model file : ");
         scanf("%127s%*[^\n]", &ret_model_path);

      rifImageFilterSetParameterString(filter, "modelPath", ret_model_path);

   } else if (!strcmp("bgra_to_rgba", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/bgra_to_rgba_conversion.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_BGRA_TO_RGBA,
                                  &filter);

   } else if (!strcmp("bloom", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/bloom.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_BLOOM,
                                  &filter);

      if (!use_default) {
         rif_float ret_param[4];
         const rif_char *ret_param_desc[4] =
                              {  "Radius [Default: 0.1] [0.01, 1] : "               ,
                                 "Threshold [Default: 0] : "                        ,
                                 "Weight [Default: 0.1] : "                         ,
                                 "Light intensity decay [Default: 1 (no decay)] : " };
         const rif_char *ret_param_name[4] =
                              { "radius", "threshold", "weight", "decay" };
   
   
         for (i=0; i < 4; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i],  ret_param[i]);
         }
      }
   } else if (!strcmp("convert", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/convert.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_CONVERT,
                                  &filter);

   } else if (!strcmp("dilate_erode", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/dilate.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_DILATE_ERODE,
                                  &filter);

      if (!use_default) {
         rif_uint ret_param;
   
         printf("Operation mode [Default: 0 (Dilate)] [0 (Dilate), 1 (Erode)] : ");
            scanf("%u%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1u(filter, "mode", ret_param);
      }

   } else if (!strcmp("emboss", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/emboss.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_EMBOSS,
                                  &filter);
      
      if (!use_default) {
         rif_uint ret_param;
   
         printf("Radius [Default: 1] [0, 50] : ");
            scanf("%u%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1u(filter, "radius", ret_param);
      }

/*
   } else if (!strcmp("motion_buffer", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/motion_buffer.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_MOTION_BUFFER,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      rif_float ret_param[16] = {   -1.94444f,  +0.0f,      +0.0f,   +0.0f,
                                    +0.0f,      +1.94444f,  +0.0f,   -1.94444f,
                                    +0.0f,      +0.0f,      +1.0f,   -3.0f,
                                    +0.0f,      +0.0f,      +1.0f,   -3.0f, };
      
      rifImageFilterSetParameter16f(filter, "viewProjMatrix", ret_param);

   } else if (!strcmp("ndc_depth", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ndc_depth.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_NDC_DEPTH,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      rif_float ret_param[16] = {   +1.0f,      +0.0f,      +0.0f,   +0.0f,
                                    +0.0f,      +1.0f,      +0.0f,   +0.0f,
                                    +0.0f,      +0.0f,      +1.0f,   +0.0f,
                                    +0.0f,      +0.0f,      +1.0f,   +1.0f, };

      rifImageFilterSetParameter16f(filter, "viewProjMatrix", ret_param);
*/
   } else if (!strcmp("posterize", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/posterization.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_POSTERIZE,
                                  &filter);

      if (!use_default) {
         rif_int ret_param;
   
         printf("Levels [Default: 1] [0, 65536] : ");
            scanf("%i%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1u(filter, "levels", ret_param);
      }
   
   } else if (!strcmp("range_remap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/range_remap.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_REMAP_RANGE,
                                  &filter);

      if (!use_default) {
         rif_uint    ret_auto;

         printf("The input range is detected automatically [Default: true] [0 (false), 1 (true)] : ");
            scanf("%u%*[^\n]", &ret_auto);
         rifImageFilterSetParameter1u(filter, "srcRangeAuto", ret_auto);
         
         if (!ret_auto) {
            rif_float ret_param[4];
            const rif_char *ret_param_desc[] =
                     {  "The minimum value of the input range [Default: 0] : "      ,
                        "The maximum value of the input range [Default: 1] : "    ,
                        "The minimum value of the output range [Default: 0] : "   ,
                        "The maximum value of the output range [Default: 1] : "   };
            const rif_char *ret_param_name[] =
                     { "srcLo", "srcHi", "dstLo", "dstHi" };

            for (i=0; i < 4; i++) {
               printf("%s", ret_param_desc[i]);
                  scanf("%f%*[^\n]", &ret_param[i]);
               rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
            }
         }
      }

   } else if (!strcmp("rgb_noise", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/rgb_noise.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_RGB_NOISE,
                                  &filter);

      if (!use_default) {
         rif_float ret_param;
         
         printf("Levels [Default: 0.5] [0, 1] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "level", ret_param);
      }
   
   } else if (!strcmp("spread", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/spread.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_SPREAD,
                                  &filter);

      if (!use_default) {
         rif_uint ret_param[2] = {0};
   
         printf("Offsets [(x, y)] : ");
            scanf("%u%*[^\n], %u%*[^\n],", &ret_param[0], &ret_param[1]);
   
         rifImageFilterSetParameter2u(filter, "offsets", ret_param[0], ret_param[1]);
      }

   } else {
      printf("  Unknown filter name: %s\n", filter_name);
      return NULL;
   }

   return filter;
}

int main(int argc, char *argv[]) {
   int i, select_device = 0, quality = 0, backend = BACKEND_TYPE, filter_count = 0;
   rif_bool use_default = false;
   const rif_char *input_path = NULL, *filter_name[9] = { NULL }, *input_ext = NULL, *output_ext = NULL;
   FILE *file_check;

   const rif_char *output_path = "out.png";

   for (i=1; i < argc; i++) {
      if (!strcmp("-i", argv[i])) {
         if (i+1 < argc) {
            i++;
            file_check  = fopen(argv[i], "r");

            if (file_check) {
               input_path  = argv[i];
               input_ext   = strrchr(input_path, '.');
            } else {
               printf("Error: \"%s\" not found\n", argv[i]);
               return -1;
            }
         } else {
            printf("Error: Please enter the path of input image\n");
            return -1;
         }
      } else if (!strcmp("-o", argv[i])) {
         if (i+1 < argc) {
            i++;

            output_path = argv[i];
            output_ext  = strrchr(output_path, '.');
         } else {
            printf("Error: Please enter the path of output image\n");
            return -1;
         }
      } else if (!strcmp("-f", argv[i]) || !strcmp("--filter", argv[i])) {
         if (i+1 < argc) {
            i++;

            filter_name[filter_count] = argv[i];
            filter_count++;
         } else {
            printf("Error: Please enter the filter name\n");
            return -1;
         }
      } else if (!strcmp("-d", argv[i]) || !strcmp("--default", argv[i])) {
         printf("use default value\n");
         use_default = true;
      } else if (!strcmp("-v", argv[i]) || !strcmp("--version", argv[i])) {
         printf("rif-cmd version: 0.0.0\n");
         printf("RIF API version: %s\n", RIF_API_VERSION_STRING);
            return 0;
      } else if (!strcmp("-q", argv[i])) {
         if (i+1 < argc) {
            i++;

            quality = atoi(argv[i]);
         } else {
            printf("Error: Please enter the quality value\n");
            return -1;
         }
      } else if (!strcmp("-g", argv[i]) || !strcmp("--gpu", argv[i])) {
         if (i+1 < argc) {
            i++;

            select_device = atoi(argv[i]);
         } else {
            printf("Error: Please enter the ID of the select device\n");
            return -1;
         }
      } else if (!strcmp("--trace", argv[i])) {
      // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/tracing.html
         if (i+1 < argc) {
            i++;

            setenv("RIF_TRACING_ENABLED", "1", 0);
            setenv("RIF_TRACING_PATH", argv[i], 0);
         } else {
            printf("Error: Please enter the path to output the tracing file\n");
            return -1;
         }
      } else if (!strcmp("--api", argv[i])) {
         if (i+1 < argc) {
            i++;

            if (!strcmp("dx12", argv[i])) {
               backend = RIF_BACKEND_API_DIRECTX12;
            } else if (!strcmp("metal", argv[i])) {
               backend = RIF_BACKEND_API_METAL;
            } else if (!strcmp("ocl", argv[i]) || !strcmp("opencl", argv[i])) {
               backend = RIF_BACKEND_API_OPENCL;
            }
         }
      } else {
         printf("Error: Unknown Option \"%s\"\n", argv[i]);
         return -1;
      }
   }

   if (input_path == NULL) {
      printf("Error: Please enter the path of input image\n"
             " ( %s -i <path> ... )\n", argv[0]);
      return -1;
   }
   for (i=0; i < filter_count; i++) {
      if (filter_name[i] == NULL) {
         printf("Error: Please enter the filte name\n");
         return -1;
      }
   }

   rif_int              status   = RIF_SUCCESS;
   rif_context          context  = nullptr;
   rif_command_queue    queue    = nullptr;
//   rif_image_filter     filter   = nullptr;
   rif_image            inputImage  = nullptr;
   rif_image            outputImage = nullptr;

   rif_char backend_api_name[16];

   switch (backend) {
      case RIF_BACKEND_API_OPENCL:
         strcpy(backend_api_name, "OpenCL");
         break;
      case RIF_BACKEND_API_DIRECTX12: 
         strcpy(backend_api_name, "DirectX12");
         break;
      case RIF_BACKEND_API_METAL:
         strcpy(backend_api_name, "Metal");
         break;
      default:
         strcpy(backend_api_name, "Unknown");
         break;
   }

   printf("Backend API: %s\n\n", backend_api_name);

// First create context and queue
   int deviceCount = 0;
   status = rifGetDeviceCount(backend, &deviceCount);
      if (status != RIF_SUCCESS) return -1;

   if (deviceCount > 0 || status) {
      status = rifCreateContext(RIF_API_VERSION, backend, select_device, nullptr, &context);
         if (status != RIF_SUCCESS || !context) return -1;
   }

   status = rifContextCreateCommandQueue(context, &queue);
      if (status != RIF_SUCCESS || !queue) return -1;

// Create input and output images

/*
   rif_image_desc input_desc;
   memset(&input_desc, 0, sizeof(input_desc));

   rif_int width, height, num_comp;
   rif_float *data = nullptr;
   rif_uchar *raw_data = stbi_load(input_path, &width, &height, &num_comp, 3);

   int array_size  = width * height * num_comp;
   data = (rif_float*) malloc(array_size * sizeof(rif_float) + 1);

   for (i=0; i < array_size; i++) {
//      printf("%f", (rif_float)raw_data[i]);
      data[i] = (rif_float)raw_data[i];
   }
      return -1;

   printf("w: %d, h: %d, comp: %d\n", width, height, num_comp);
//      return -1;

   input_desc.image_width        =  width;
   input_desc.image_height       =  height;
   input_desc.num_components     =  num_comp;
   input_desc.type               =  RIF_COMPONENT_TYPE_FLOAT32;
   
   rifContextCreateImage(context, &input_desc, data, &inputImage);
      free(data);
      data = nullptr;

*/

   inputImage = ImageTools::LoadImage(input_path, context);

// Create output image
   rif_image_desc    output_desc;
   size_t            retSize;

   rifImageGetInfo(inputImage, RIF_IMAGE_DESC, sizeof(output_desc), &output_desc, &retSize);

   if (!strcmp(".jpg", input_ext) || !strcmp(".jpeg", input_ext))
      output_desc.type = RIF_COMPONENT_TYPE_UINT8;

   status = rifContextCreateImage(context, &output_desc, nullptr, &outputImage);
      if (status != RIF_SUCCESS) return -1;

// Create image filter
// Attach filter and set parameters
   printf("fc: %d\n", filter_count);

   rif_image_filter filter[filter_count] = { nullptr };

for (i=0; i < filter_count; i++) {

   printf("filter_name-%d: %s\n", i, filter_name[i]);
   filter[i] = set_param(context,
                         filter_name[i],
                         filter[i],
                         use_default,
                         &output_desc);

   /*
    * https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/info_setting_types/rif_compute_type.html
    * RIF_COMPUTE_TYPE_FLOAT  0x0
    * RIF_COMPUTE_TYPE_HALF   0x1
    * https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/api/rifimagefiltersetcomputetype.html
    * rifImageFilterSetComputeType(filter, RIF_COMPUTE_TYPE_FLOAT);
    */

/*
 *    with buffer
 */
 /*
   if (i == 0) {
      printf("i == 0\n");
      status = rifImageFilterSetParameterImage(filter[i], "srcBuffer", inputImage);
         if (status != RIF_SUCCESS) return -1;
   } else if (i > 0 && i < filter_count - 1) {
      printf("i > 0 && i < filter_count - 1\n");
      status = rifImageFilterSetParameterImage(filter[i], "srcBuffer",
                                               (rif_image)(filter[i-1]));
         if (status != RIF_SUCCESS) return -1;
   } else if (i == filter_count - 1) {
      printf("i == filter_count - 1\n");
      status = rifCommandQueueAttachImageFilter(queue, filter[i],
                                                (rif_image)(filter[i-1]), outputImage);
         if (status != RIF_SUCCESS) return -1;
   }
*/

/*
 *    without buffer
 */
   if (i == 0 || i % 2 == 0) {
      rifCommandQueueAttachImageFilter(queue, filter[i], inputImage,  outputImage);
   } else {
      rifCommandQueueAttachImageFilter(queue, filter[i], outputImage, inputImage);
   }

   if (i == filter_count - 1 && filter_count - 1 % 2 == 1) {
      rif_image_filter fill_filter = nullptr;
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_CONVERT,
                                  &fill_filter);

      rifCommandQueueAttachImageFilter(queue, fill_filter, inputImage, outputImage);
   }

 //     rifCommandQueueAttachImageFilter(queue, nullptr, inputImage,  outputImage);
//   rifCommandQueueAttachImageFilter(queue, filter[i], inputImage, outputImage);
}

// Execute queue
//   rif_performance_statistic perf = {0};
   status = rifContextExecuteCommandQueue(context, queue, nullptr, nullptr, nullptr);
      if (status != RIF_SUCCESS) return -1;
//   printf("exe: %u\ncompile: %f\n", perf.execution_time, perf.compile_time);

   rif_uchar *output_data;
   status = rifImageMap(outputImage, RIF_IMAGE_MAP_READ, (void**)&output_data);
      if (status != RIF_SUCCESS) return -1;

if (!strcmp(".jpg", output_ext) || !strcmp(".jpeg", output_ext)) {
/*
 *     int stbi_write_jpg(char const *filename,
 *                        int w,
 *                        int h,
 *                        int comp,
 *                        const void *data, int quality);
 */
   status = stbi_write_jpg(output_path,
                           output_desc.image_width,
                           output_desc.image_height,
                           output_desc.num_components,
                           output_data, quality);

} else if (!strcmp(".png", output_ext)) {
/*
 *    int stbi_write_png(char const *filename,
 *                       int w,
 *                       int h,
 *                       int comp,
 *                       const void  *data, int stride_in_bytes)
 */
   status = stbi_write_png(output_path,
                           output_desc.image_width,
                           output_desc.image_height,
                           output_desc.num_components,
                           output_data, 0);
}
/*
   status = ImageTools::SaveImage(outputImage, output_path);
      if (status) {
         printf("\nSuccess\n");
      } else {
         printf("\nError: output image\n");
         return -1;
      }
*/

   if (status) {
      printf("\nSuccess: %s\n", realpath(output_path, NULL));
   } else {
      printf("\nError: output image\n");
      return -1;
   }

   rifImageUnmap(outputImage, output_data);
   stbi_image_free(output_data);
   
// Free resources
   for (i=0; i < filter_count; i++) {
      rifCommandQueueDetachImageFilter(queue, filter[i]);
      rifObjectDelete(filter[i]);
   }
   rifObjectDelete(inputImage);
   rifObjectDelete(outputImage);
   rifObjectDelete(queue);
   rifObjectDelete(context);

   return 0;
}
