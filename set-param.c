/*
 *    Copyright 2021 Umio-Yasuno
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "RadeonImageFilter/include/RadeonImageFilters.h"
#include "RadeonImageFilter/include/RadeonImageFilters_version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rif-cmd.h"

static void err_need_param (const char *filter_name, const char *desc) {
   printf("[ERROR] %s filter parameter: \"%s\"\n", filter_name, desc);
};

int set_param(rif_context       context,
              Param             filter_param,
              rif_image_filter  filter,
              rif_image_filter  *filterptr,
              rif_bool          use_default,
              rif_image_desc    *output_desc)
{
   int i = 0;
   char *arg_param[24]  = { NULL };
   char *arg_buff       = strtok(filter_param.param_string, ",");

   while (arg_buff != NULL) {
      arg_param[i] = arg_buff;
      arg_buff = strtok(NULL, ",");
      i++;
   }

   if (use_default)
      filter_param.count_param = 0;

// Rotating Filters
   if (!strcmp("flip_h", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/horizontal_flip.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_FLIP_HOR,
                                  &filter);

   } else if (!strcmp("flip_v", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/vertical_flip.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_FLIP_VERT,
                                  &filter);

   } else if (!strcmp("rotate", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/rotation.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_ROTATE,
                                  &filter);

//      rifContextCreateImage(context, output_desc, nullptr, (rif_image*)outputImage);

// Blurring and Resampling Filters
   } else if (!strcmp("ai_upscale", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ai_upscale.html
      /*
       *    not work with Polaris11(gfx803) + MIOpen v2.0.5
       */
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_AI_UPSCALE,
                                  &filter);

      rif_char ret_model_path[128] = "./RadeonImageFilter/models";

      rifImageFilterSetParameterString(filter, "modelPath", ret_model_path);

      if (!use_default && 0 == filter_param.count_param) {
         rif_uint ret_param;
      /*
       *    #define RIF_AI_UPSCALE_MODE_GOOD_2X 0x1u
       *    #define RIF_AI_UPSCALE_MODE_BEST_2X 0x2u
       *    #define RIF_AI_UPSCALE_MODE_FAST_2X 0x3u
       */
         printf("Select upscale mode : \n"
                " [1: Good (Default)] \n [2: Best] \n [3: Fast] \n");
            scanf("%1u%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1u(filter, "mode", ret_param);
         
         printf("Path to model files (Default: ./RadeonImageFilter/models) \n"
                " (enter \".\" if use default path) : ");
            scanf("%127s%*[^\n]", ret_model_path);

         if (strcmp(".", ret_model_path) != 0)
            rifImageFilterSetParameterString(filter, "modelPath", ret_model_path);

      } else if (2 == filter_param.count_param) {
         rifImageFilterSetParameter1u(filter,     "mode",      atoi(arg_param[0]));
         rifImageFilterSetParameterString(filter, "modelPath", arg_param[1]);
      } else {
         err_need_param(filter_param.filter_name, "<mode (int)(1: Good, 2: Best, 3: Fast)>,<path>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("gaussian_blur", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/gaussian_blurring.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_GAUSSIAN_BLUR,
                                  &filter);

      if (!use_default && 0 == filter_param.count_param) {
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

      } else if (3 == filter_param.count_param) {
         rifImageFilterSetParameter1u(filter, "radius",  atoi(arg_param[0]));
         rifImageFilterSetParameter1f(filter, "sigma",   atof(arg_param[1]));
         rifImageFilterSetParameter1u(filter, "tiling",  atoi(arg_param[2]));
      } else {
         err_need_param(filter_param.filter_name, "<radius (int)(0_50)>,<sigma (int)>,<tiling (bool)(0: false, 1: true)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("motion_blur", filter_param.filter_name)) {
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

      if (!use_default && 0 == filter_param.count_param) {
         rif_uint    ret_radius;
         rif_float   ret_param[2];
   
         printf("Radius [Default: 5] [0, 50] : ");
            scanf("%2u%*[^\n]", &ret_radius);
         rifImageFilterSetParameter1u(filter, "radius", ret_radius);
   
         printf("Motion direction [Default: (1, 1)] : ");
            scanf("%f, %f%*[^\n]", &ret_param[0], &ret_param[1]);
         rifImageFilterSetParameter2f(filter, "direction", ret_param[0], ret_param[1]);

      } else if (3 == filter_param.count_param) {
         rifImageFilterSetParameter1u(filter, "radius",     atoi(arg_param[0]));
         rifImageFilterSetParameter2f(filter, "direction",  atof(arg_param[1]),
                                                            atof(arg_param[2]));
      } else {
         err_need_param(filter_param.filter_name, "<radius (int)(0_50)>,<motion direction (float)>,<motion direction (float)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("resample", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/resampling.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_RESAMPLE,
                                  &filter);
      
         rif_float ret_scale = 2.0;

      if (!use_default && 0 == filter_param.count_param) {
         rif_uint ret_operator;

         printf("Interpolation operator : \n"
                "[0]: Nearest,                     [1]: Bilinear\n"
                "[2]: Bicubic,                     [3]: Lanczos\n"
                );
            scanf("%u%*[^\n]", &ret_operator);

         rifImageFilterSetParameter1u(filter, "interpOperator", ret_operator);

         printf("Scale {n}x [Default: 2] : ");
            scanf("%f%*[^\n]", &ret_scale);

      } else if (2 == filter_param.count_param) {
         rifImageFilterSetParameter1u(filter, "interpOperator", atoi(arg_param[0]));
         ret_scale = atof(arg_param[1]);
      } else {
         err_need_param(filter_param.filter_name, "<interpOperator (int)(0: Nearest, 1: Bilinear, 2: Bicubic, 3: Lanczos)><scale (float)>");
         return PARAM_ERROR;
      }

/*
      output_desc->image_width  = (rif_uint)(output_desc->image_width  * ret_scale);
      output_desc->image_height = (rif_uint)(output_desc->image_height * ret_scale);

      status = rifContextCreateImage(context, &output_desc, nullptr, &outputImage);
         if (status != RIF_SUCCESS) return NULL;
*/
      rifImageFilterSetParameter2u(filter, "outSize",
                                   (rif_uint)(output_desc->image_width  * ret_scale),
                                   (rif_uint)(output_desc->image_height * ret_scale));


   } else if (!strcmp("dynamic_resample", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/resampling.html
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/info_setting_types/rif_image_filter_interpolation_operator.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_RESAMPLE_DYNAMIC,
                                  &filter);
      
         rif_float   ret_sharp = 1.0,  ret_scale = 2.0;
         rif_uint    ret_operator = 0, offset = 4;

//         rifImageFilterSetParameter1u(filter, "interpOperator", RIF_IMAGE_INTERPOLATION_LANCZOS4);
//         rifImageFilterSetParameter1f(filter, "sharpness", ret_sharp);

      if (!use_default && 0 == filter_param.count_param) {
//         rif_uint ret_operator, offset = 4;

         printf("Interpolation operator : \n"
                "[0]: Lanczos4,                    [1]: Lanczos6\n"
                "[2]: Lanczos12,                   [3]: Lanczos3\n"
                "[4]: Kaiser,                      [5]: Blackman\n"
                "[6]: Gauss,                       [7]: Box\n"
                "[8]: Tent,                        [9]: Bell\n"
                "[10]: B-spline,                   [11]: Quadratic Interpolation\n"
                "[12]: Quadratic approximation,    [13]: Quadratic mix\n"
                "[14]: Mitchell,                   [15]: Catmull\n");
            scanf("%u%*[^\n]", &ret_operator);

         rifImageFilterSetParameter1u(filter, "interpOperator", ret_operator + offset);

         printf("Scale {n}x [Default: 2] : ");
            scanf("%f%*[^\n]", &ret_scale);

         printf("Sharpness [Default: 1] [0.2, 5] : ");
            scanf("%f%*[^\n]", &ret_sharp);

         rifImageFilterSetParameter1f(filter, "sharpness", ret_sharp);
      
      } else if (3 == filter_param.count_param) {
         rifImageFilterSetParameter1u(filter, "interpOperator",   atoi(arg_param[0]) + offset);
         ret_scale = atof(arg_param[1]);
         rifImageFilterSetParameter1f(filter, "sharpness",        atof(arg_param[1]));

      } else {
         printf("[ERROR] dynamic_resample filter parameter: <interpOperator (int(\n"
                "    [0]: Lanczos4,                    [1]: Lanczos6\n"
                "    [2]: Lanczos12,                   [3]: Lanczos3\n"
                "    [4]: Kaiser,                      [5]: Blackman\n"
                "    [6]: Gauss,                       [7]: Box\n"
                "    [8]: Tent,                        [9]: Bell\n"
                "    [10]: B-spline,                   [11]: Quadratic Interpolation\n"
                "    [12]: Quadratic approximation,    [13]: Quadratic mix\n"
                "    [14]: Mitchell,                   [15]: Catmull)>,\n"
                "  <scale (float)>,<sharpness (float)(0.2_5)>");
         return PARAM_ERROR;
      }

/*
      output_desc->image_width  = (rif_uint)(output_desc->image_width  * ret_scale);
      output_desc->image_height = (rif_uint)(output_desc->image_height * ret_scale);

      status = rifContextCreateImage(context, &output_desc, nullptr, &outputImage);
         if (status != RIF_SUCCESS) return NULL;
*/
      rifImageFilterSetParameter2u(filter, "outSize",
                                   (rif_uint)(output_desc->image_width  * ret_scale),
                                   (rif_uint)(output_desc->image_height * ret_scale));

// Tone Mapping and Color Changing Filters
   } else if (!strcmp("filmic_tonemap", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/aces_filmic_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_FILMIC_TONEMAP,
                                  &filter);

      const rif_char *ret_param_name[2] = { "exposure", "contrast" };

      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param[2];
         rif_bool  ret_apply;
   
         const rif_char *ret_param_desc[2] =
               {  "Input image pre-exposure coefficient [Default: 0] [0, 100] : " ,
                  "Input image pre-contrast coefficient [Default: 1] [0, 10] : "  };
   
         for (i=0; i < 2; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
   
         printf("Apply ToneMap ? [Default: false] [0 (false), 1 (true)] : ");
            scanf("%1d%*[^\n]", &ret_apply);
         rifImageFilterSetParameter1u(filter, "applyToneMap",  ret_apply);

      } else if (3 == filter_param.count_param) {
         rifImageFilterSetParameter1f(filter, ret_param_name[0],  atof(arg_param[0]));
         rifImageFilterSetParameter1f(filter, ret_param_name[1],  atof(arg_param[1]));
         rifImageFilterSetParameter1u(filter, "applyToneMap",     atoi(arg_param[2]));

      } else {
         err_need_param(filter_param.filter_name, "<exposure (int)(0_100)>,<contrast (int)(0_10)><applyToneMap (bool)(0: false, 1: true)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("autolinear_tonemap", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/auto_linear_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_AUTOLINEAR_TONEMAP,
                                  &filter);

      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param;
   
         printf("Gamma correction coefficient [Default: 2.2] [0, 5] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "gamma", ret_param);

      } else if (1 == filter_param.count_param) {
         rifImageFilterSetParameter1f(filter, "gamma", atof(arg_param[0]));

      } else {
         return PARAM_ERROR;
      }

   } else if (!strcmp("color_space", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/color_space_changing.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_COLOR_SPACE,
                                  &filter);

         const rif_char *ret_src[2] = { "srcColorSpace", "dstColorSpace" };

      if (!use_default && 0 == filter_param.count_param) {
         rif_uint ret_param[2];
         const rif_char *ret_param_desc[2] = { "input", "output" };
   
         for (i=0; i < 2; i++) {
         /*
          *    #define RIF_COLOR_SPACE_SRGB 0x1u
          *    #define RIF_COLOR_SPACE_ADOBE_RGB 0x2u
          *    #define RIF_COLOR_SPACE_REC2020 0x3u
          *    #define RIF_COLOR_SPACE_DCIP3 0x4u
          */
            printf("\nSelect color space of the %s image : \n [1: sRGB]\n [2: Adobe RGB]\n"
                   "[3: Rec. 2020]\n [4: DCI-P3]\n ", ret_param_desc[i]);
               scanf("%1u", &ret_param[i]);
   
            rifImageFilterSetParameter1u(filter, ret_src[i], ret_param[i]);
         }

      } else if (2 == filter_param.count_param) {
         rifImageFilterSetParameter1u(filter, ret_src[0], atoi(arg_param[0]));
         rifImageFilterSetParameter1u(filter, ret_src[1], atoi(arg_param[1]));

      } else {
         return PARAM_ERROR;
      }

   } else if (!strcmp("drago_tonemap", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/drago_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_DRAGO_TONEMAP,
                                  &filter);

      const rif_char *ret_param_desc[3] =
                     {  "Bias [Default: 1] : "                             ,
                        "Average input image luminance [Default: 0] : "    ,
                        "Maximum input image luminance [Default: 0.1] : "  };
      const rif_char *ret_param_name[3] = { "bias", "avLum", "maxLum" };

      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param[3];
   
         for (i=0; i < 3; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      } else if (3 == filter_param.count_param) {
         for (i=0; i < 3; i++) {
            rifImageFilterSetParameter1f(filter, ret_param_name[i], atof(arg_param[i]));
         }
      } else {
         err_need_param(filter_param.filter_name, "<bias (int)>,<avLum (int)>,<maxLum (int)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("exponential_tonemap", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/exponential_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_EXPONENTIAL_TONEMAP,
                                  &filter);

      const rif_char *ret_param_name[3] =
                  { "gamma", "exposure", "intensity" };

      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param[3];
   
         const rif_char *ret_param_desc[3] =
                  {  "Gamma correction coefficient [Default: 2.2] [0, 5] : "           ,
                     "Input image pre-exposure coefficient [Default: 0] [-10, 10] : "  ,
                     "Output image pre-exposure coefficient [Default: 1] [0, 100] : "  };
   
         for (i=0; i < 3; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      } else if (3 == filter_param.count_param) {
         for (i=0; i < 3; i++) {
            rifImageFilterSetParameter1f(filter, ret_param_name[i], atof(arg_param[i]));
         }
      } else {
         err_need_param(filter_param.filter_name, "<gamma (float)(0_50)>,<exposure (float)(-10_10)>,<intensity (float)(0_100)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("filmic_uncharted_tonemap", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/filmic_uncharted_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_FILMIC_UNCHARTED_TONEMAP,
                                  &filter);

      const rif_char *ret_param_name[9] = { "A", "B", "C", "D", "E", "F", "W",
                                            "exposure", "gamma" };
      if (!use_default && 0 == filter_param.count_param) {
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
   
   
         for (i=0; i < 9; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      } else if (9 == filter_param.count_param) {
         for (i=0; i < 9; i++) {
            rifImageFilterSetParameter1f(filter, ret_param_name[i], atof(arg_param[i]));
         }
      } else {
         err_need_param(filter_param.filter_name, "<shoulder strength (float)(0_1)>,<linear strength (float)(0_1)>,<linear angle (float)(0_1)>,<toe strength(float)(0_2)>,<toe numerator (float)(0_1)>,<toe denominator (float)(0_1)>,<white (float)(1_20)>,<exposure (float)(-10_10)>,<gamma (float)(0_5)>");
         return PARAM_ERROR;
      }


   } else if (!strcmp("gamma_correct", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/gamma_correction.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_GAMMA_CORRECTION,
                                  &filter);

      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param;
   
         printf("Gamma [Default: 1] [0, 5] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "gamma", ret_param);

      } else if (1 == filter_param.count_param) {
         rifImageFilterSetParameter1f(filter, "gamma", atof(arg_param[0]));

      } else {
         err_need_param(filter_param.filter_name, "<gamma (float)(0_5)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("hue_saturation", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/hue_saturation_changing.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_HUE_SATURATION,
                                  &filter);

      const rif_char *ret_param_name[3] = { "saturation", "hue", "value" };

      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param[3];
   
         const rif_char *ret_param_desc[3] =
                  {  "Saturation [Default: 1] : "              ,
                     "Hue [Default: 0] [-180, 180] : "         ,
                     "Brightness multiplier [Default: 1] : "   };
   
         for (i=0; i < 3; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      } else if (3 == filter_param.count_param) {
         for (i=0; i < 3; i++) {
            rifImageFilterSetParameter1f(filter, ret_param_name[i], atof(arg_param[i]));
         }
      } else {
         err_need_param(filter_param.filter_name, "<saturation (float)>,<hue (float)(-180_180)>,<brightness (float)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("photo_linear_tonemap", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/linear_photographic_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_PHOTO_LINEAR_TONEMAP,
                                  &filter);

      const rif_char *ret_param_name[4] =
                     { "gamma", "exposureTime", "sensitivity", "fstop" };
   
      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param[4];
   
         const rif_char *ret_param_desc[4] =
                           {  "Gamma correction [Default: 2.2] [0, 5] : "        ,
                              "Film exposure time [Default: 0.125] [0, 100] : "  ,
                              "Luminance of the scene [Default: 1] [0, 100] : "  ,
                              "Aperture f-number [Default: 1] [0, 100] : "       };
   
         for (i=0; i < 4; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      } else if (4 == filter_param.count_param) {
         for (i=0; i < 4; i++) {
            rifImageFilterSetParameter1f(filter, ret_param_name[i], atof(arg_param[i]));
         }
      } else {
         err_need_param(filter_param.filter_name, "<gamma (float)(0_5)>,<exposureTime (float)(0_100)>,<sensitivity (float)(0_100)>,<fstop (float)(0_100)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("linear_tonemap", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/linear_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_LINEAR_TONEMAP,
                                  &filter);

      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param;
   
         printf("Linear coefficient [Default: 1] [0, 1e+4f(10000)] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "key", ret_param);

      } else if (1 == filter_param.count_param) {
         rifImageFilterSetParameter1f(filter, "key", atof(arg_param[0]));

      } else {
         err_need_param(filter_param.filter_name, "<linear coefficient (float)(0_10000)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("white_tonemap", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/max_white_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_MAXWHITE_TONEMAP,
                                  &filter);

   } else if (!strcmp("photo_tonemap", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/photographic_tone_mapper.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_PHOTO_TONEMAP,
                                  &filter);

      const rif_char *ret_param_name[9] =
                        {  "gamma",      "exposureTime", "sensitivity"  ,
                           "fstop",      "focalL",       "vignette"     ,
                           "saturation", "lighten",      "darken"       };

      if (!use_default && 0 == filter_param.count_param) {
         rif_float   ret_param[9];
         rif_float   ret_whitepoint[4];
         rif_uint    ret_iso;
         rif_uint    ret_apply[2];
   
   
         const rif_char *ret_param_desc[9] =
                     { "Gamma correction [Default: 1] [0, 5] : "           ,
                       "Film exposure time [Default: 0.125] [0, 100] : "   ,
                       "Luminance of the scene [Default: 1] [0, 100] : "   ,
                       "Aperture f-number [Default: 1] [0.00001, 100] : "  ,
                       "Camera focal length [Default: 1] [0.00001, 100] : ",
                       "Vignette [Default: 0] [0, 1] : "                   ,
                       "Saturation [Default: 0.5] [0, 1] : "               ,
                       "Highlight [Default: 0.5] [0, 10] : "               ,
                       "Shadow [Default: 0.5] [0, 10] : "                  };
   
         for (i=0; i < 9; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i],  ret_param[i]);
         }
   
         printf("Film ISO [Default: 100] [100, 2000] : ");
            scanf("%5u%*[^\n]", &ret_iso);

         printf("Do negative values clamp to zero? [Default: true] [0 (false), 1 (true)] : ");
            scanf("%1u%*[^\n]", &ret_apply[0]);
   
         printf("Whitepoint [Default: (1, 1, 1, 1)] : ");
           scanf("%f, %f, %f, %f%*[^\n]",
                     &ret_whitepoint[0], &ret_whitepoint[1],
                     &ret_whitepoint[2], &ret_whitepoint[3]);

         printf("Use ISO? [Default: 0 (false)] [0 (false), 1 (true)] : ");
            scanf("%1u%*[^\n]", &ret_apply[1]);

         rifImageFilterSetParameter1u(filter, "ISO",        ret_iso);
         rifImageFilterSetParameter1u(filter, "useclampN",  ret_apply[0]);
         rifImageFilterSetParameter4f(filter, "whitepoint",
                                       ret_whitepoint[0], ret_whitepoint[1],
                                       ret_whitepoint[2], ret_whitepoint[3]);
         rifImageFilterSetParameter1u(filter, "useISO",     ret_apply[1]);

      } else if (16 == filter_param.count_param) {
         for (i=0; i < 9; i++) {
            rifImageFilterSetParameter1f(filter, ret_param_name[i], atof(arg_param[i]));
         }

         rifImageFilterSetParameter1u(filter,   "ISO",         atoi(arg_param[9]));
         rifImageFilterSetParameter1u(filter,   "useclampN",   atoi(arg_param[10]));

         rifImageFilterSetParameter4f(filter,   "whitepoint",
                                      atoi(arg_param[11]), atoi(arg_param[12]),
                                      atoi(arg_param[13]), atoi(arg_param[14]));

         rifImageFilterSetParameter1u(filter,   "useISO",      atoi(arg_param[15]));

      } else {
         err_need_param(filter_param.filter_name, "<gamma (float)>,<exposureTime (float)>,<sensitivity (float)>,<fstop (float)>,<focalL (float)>,<vignette (float)>,<saturation (float)>,<lighten (float)>,<darken (float)>,<ISO (int)>,<useclampN (int)(0: false, 1: true)>,<whitepoint0 (float)>,<whitepoint1 (float)>,<whitepoint2 (float)>,<whitepoint3 (float)>,<useISO (int)(0: false, 1: true)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("reinhard_tonemap", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/reinhard_tone_mapping.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_REINHARD02_TONEMAP,
                                  &filter);

      const rif_char *ret_param_name[4] =
               { "gamma", "preScale", "postScale", "burn" };

      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param[4];
         const rif_char *ret_param_desc[4] =
            { "Gamma correction [Default: 2.2] [0, 5] : "               ,
              "Controls the amount of light in the scene before tone mapping is applied [Default: 1] [0, 10] : ",
              "Controls the amount of light in the scene after tone mapping is applied [Default: 1.2] [0, 10] : ",
              "Please brightness parameter [Default: 0.375] [0, 10] : " };

         for (i=0; i < 4; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      } else if (4 == filter_param.count_param) {
        for (i=0; i < 4; i++) {
            rifImageFilterSetParameter1f(filter, ret_param_name[i], atof(arg_param[i]));
         }
      } else {
         err_need_param(filter_param.filter_name, "<gamma (float)(0_5)>,<preScale (float)(0_10)>,<postScale (float)(0_10)>,<burn (float)(0_10)>");
         return PARAM_ERROR;
      }
   /*
   } else if (!strcmp("ai_denoiser", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ai_denoiser.html
   */
   } else if (!strcmp("median_denoise", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/median_denoiser.html
      rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_MEDIAN_DENOISE,
                                          &filter);
 
      if (!use_default && 0 == filter_param.count_param) {
         rif_uint ret_param;

         printf("Radius of the filter window [Default: 1] [0, 50]\n");
            scanf("%u%*[^\n]", &ret_param);

         rifImageFilterSetParameter1u(filter, "radius", ret_param);

      } else if (1 == filter_param.count_param) {
         rifImageFilterSetParameter1u(filter, "radius", atoi(arg_param[0]));

      } else {
         err_need_param(filter_param.filter_name, "<radius (int)(0_50)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("laplace_edge_detect", filter_param.filter_name) || !strcmp("laplace", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/laplace_edge_detection.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_LAPLACE,
                                  &filter);

   } else if (!strcmp("sobel_edge_detect", filter_param.filter_name) || !strcmp("sobel", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/sobel_edge_detection.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_SOBEL,
                                  &filter);

   } else if (!strcmp("sharpness", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/sharpen.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_SHARPEN,
                                  &filter);

      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param;
   
         printf("Sharpness [Default: 0.5] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "sharpness", ret_param);
      } else if (1 == filter_param.count_param) {
         rifImageFilterSetParameter1f(filter, "sharpness", atof(arg_param[0]));

      } else {
         err_need_param(filter_param.filter_name, "<sharpness (float)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("ai_custom", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ai_custom_model.html
      rifContextCreateImageFilter(context,
                                  RIF_CUSTOM_AI_MODEL,
                                  &filter);

      if (1 == filter_param.count_param) {
         rifImageFilterSetParameterString(filter, "modelPath", arg_param[0]);

      } else {
         rif_char ret_model_path[128];
         printf("Path to machine learning model file : ");
            scanf("%127s%*[^\n]", ret_model_path);

         rifImageFilterSetParameterString(filter, "modelPath", ret_model_path);
      }

   } else if (!strcmp("bgra_to_rgba", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/bgra_to_rgba_conversion.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_BGRA_TO_RGBA,
                                  &filter);

   } else if (!strcmp("bloom", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/bloom.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_BLOOM,
                                  &filter);

      const rif_char *ret_param_name[4] =
                        { "radius", "threshold", "weight", "decay" };
   
      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param[4];
         const rif_char *ret_param_desc[4] =
                     { "Radius [Default: 0.1] [0.01, 1] : "               ,
                       "Threshold [Default: 0] : "                        ,
                       "Weight [Default: 0.1] : "                         ,
                       "Light intensity decay [Default: 1 (no decay)] : " };

         for (i=0; i < 4; i++) {
            printf("%s", ret_param_desc[i]);
               scanf("%f%*[^\n]", &ret_param[i]);
   
            rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
         }
      } else if (4 == filter_param.count_param) {
         for (i=0; i < 4; i++) {
            rifImageFilterSetParameter1f(filter, ret_param_name[i], atof(arg_param[i]));
         }
      } else {
         err_need_param(filter_param.filter_name, "<radius (float)(0.01_1)>,<threshold (float)>,<weight (float)>,<decay (float)>");
         return PARAM_ERROR;;
      }

   } else if (!strcmp("convert", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/convert.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_CONVERT,
                                  &filter);

   } else if (!strcmp("dilate_erode", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/dilate.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_DILATE_ERODE,
                                  &filter);


      if (!use_default && 0 == filter_param.count_param) {
         rif_uint ret_param;
   
         printf("Operation mode [Default: 0 (Dilate)] [0 (Dilate), 1 (Erode)] : ");
            scanf("%u%*[^\n]", &ret_param);

         rifImageFilterSetParameter1u(filter, "mode", ret_param);

      } else if (1 == filter_param.count_param) {
         rifImageFilterSetParameter1u(filter, "mode", atoi(arg_param[0]));

      } else {
         err_need_param(filter_param.filter_name, "<mode (int)(0: Dilate, 1: Erode)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("emboss", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/emboss.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_EMBOSS,
                                  &filter);
      
      if (!use_default && 0 == filter_param.count_param) {
         rif_uint ret_param;
   
         printf("Radius [Default: 1] [0, 50] : ");
            scanf("%u%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1u(filter, "radius", ret_param);

      } else if (1 == filter_param.count_param) {
         rifImageFilterSetParameter1u(filter, "radius", atoi(arg_param[0]));

      } else {
         err_need_param(filter_param.filter_name, "<radius (int)(0_50)>");
         return PARAM_ERROR;
      }

/*
   } else if (!strcmp("motion_buffer", filter_param.filter_name)) {
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

   } else if (!strcmp("ndc_depth", filter_param.filter_name)) {
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
   } else if (!strcmp("posterize", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/posterization.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_POSTERIZE,
                                  &filter);

      if (!use_default && 0 == filter_param.count_param) {
         rif_int ret_param;
   
         printf("Levels [Default: 1] [0, 65536] : ");
            scanf("%i%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1u(filter, "levels", ret_param);

      } else if (1 == filter_param.count_param) {
         rifImageFilterSetParameter1u(filter, "levels", atoi(arg_param[0]));

      } else {
         err_need_param(filter_param.filter_name, "<levels (int)>");
         return PARAM_ERROR;
      }
   
   } else if (!strcmp("range_remap", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/range_remap.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_REMAP_RANGE,
                                  &filter);

      const rif_char *ret_param_name[] =
               { "srcLo", "srcHi", "dstLo", "dstHi" };

      if (!use_default && 0 == filter_param.count_param) {
         rif_uint    ret_auto = 0;
         rif_float   ret_param[4];
         const rif_char *ret_param_desc[] =
               { "The minimum value of the input range [Default: 0] : "    ,
                 "The maximum value of the input range [Default: 1] : "    ,
                 "The minimum value of the output range [Default: 0] : "   ,
                 "The maximum value of the output range [Default: 1] : "   };

         printf("The input range is detected automatically [Default: true] [0 (false), 1 (true)] : ");
            scanf("%u%*[^\n]", &ret_auto);
         rifImageFilterSetParameter1u(filter, "srcRangeAuto", ret_auto);
         
         if (!ret_auto) {
            for (i=0; i < 4; i++) {
               printf("%s", ret_param_desc[i]);
                  scanf("%f%*[^\n]", &ret_param[i]);
               rifImageFilterSetParameter1f(filter, ret_param_name[i], ret_param[i]);
            }
         }
      } else if (1 == filter_param.count_param) {
         rifImageFilterSetParameter1u(filter, "srcRangeAuto", 1);

      } else if (4 == filter_param.count_param) {
         for (i=0; i < 4; i++) {
            rifImageFilterSetParameter1f(filter, ret_param_name[i], atof(arg_param[i]));
         }
      } else {
         err_need_param(filter_param.filter_name, "<srcRangeAuto (bool)(0: false, 1: true)>\nor <min input range (int)>,<max input range (int)>,<min output range (int)>,<max output range (int)>");
         return PARAM_ERROR;
      }

   } else if (!strcmp("rgb_noise", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/rgb_noise.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_RGB_NOISE,
                                  &filter);

      if (!use_default && 0 == filter_param.count_param) {
         rif_float ret_param;
         
         printf("Levels [Default: 0.5] [0, 1] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "level", ret_param);

      } else if (1 == filter_param.count_param) {
         rifImageFilterSetParameter1f(filter, "level", atof(arg_param[0]));

      } else {
         err_need_param(filter_param.filter_name, "<level (float)(0_50)>");
         return PARAM_ERROR;
      }
   
   } else if (!strcmp("spread", filter_param.filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/spread.html
      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_SPREAD,
                                  &filter);

      if (!use_default && 0 == filter_param.count_param) {
         rif_uint ret_param[2] = {0};
   
         printf("Offsets [(x, y)] : ");
            scanf("%u%*[^\n], %u%*[^\n],", &ret_param[0], &ret_param[1]);
   
         rifImageFilterSetParameter2u(filter, "offsets", ret_param[0], ret_param[1]);

      } else if (2 == filter_param.count_param) {
         rifImageFilterSetParameter2u(filter, "offsets", atoi(arg_param[0]), atoi(arg_param[1]));

      } else {
         err_need_param(filter_param.filter_name, "<offsets x (int)>,<offsets y (int)>");
         return PARAM_ERROR;
      }

   } else {
      printf("  Unknown filter name: %s\n", filter_param.filter_name);
         return -1;
   }

   *filterptr = filter;

   return RIF_SUCCESS;
}
