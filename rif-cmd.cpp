#include "RadeonImageFilter/include/RadeonImageFilters.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "RadeonImageFilter/samples/ImageTools/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "RadeonImageFilter/samples/ImageTools/stb_image_write.h"

#include "RadeonImageFilter/samples/ImageTools/ImageTools.h"

#include <stdio.h>
#include <string.h>

// g++ rif-cmd.cpp -L./RadeonImageFilter/Ubuntu18/Dynamic/ -L./RadeonImageFilter/Ubuntu18/Static/ -lRadeonImageFilters -o rif-cmd

int main(int argc, char *argv[]) {
   int i;
   rif_bool    use_default = false;

   rif_char    input_path[64], filter_name[64];

   const rif_char *output_path = "out.png";

   for (i=1; i < argc; i++) {
      if (!strcmp("-i", argv[i])) {
         if (i + 1 < argc) {
            i++;

            if (strlen(argv[i]) < 64)
               strncpy(input_path, argv[i], 63);
         } else {
            printf("Error: Please input image path\n");
            return -1;
         }
      } else if (!strcmp("-o", argv[i])) {
         if (i + 1 < argc) {
            i++;

            output_path = argv[i];
         }
      } else if (!strcmp("-f", argv[i]) || !strcmp("--filter", argv[i])) {
         if (i + 1 < argc) {
            i++;

            if (strlen(argv[i]) < 64)
               strncpy(filter_name, argv[i], 63);
         } else {
            printf("Error: Please filter name\n");
            return -1;
         }
      } else if (!strcmp("-d", argv[i]) || !strcmp("--default", argv[i])) {
         printf("use default value\n");
         use_default = true;
      } else {
         printf("Error: Unknown Option \"%s\"\n", argv[i]);
         return -1;
      }
   }
   
   rif_int              status   = RIF_SUCCESS;
   rif_context          context  = nullptr;
   rif_command_queue    queue    = nullptr;
   rif_image_filter     filter   = nullptr;
   rif_image            inputImage  = nullptr;
   rif_image            outputImage = nullptr;


// First create context and queue
   int deviceCount = 0;
   status = rifGetDeviceCount(RIF_BACKEND_API_OPENCL, &deviceCount);
      if (status != RIF_SUCCESS) return -1;
   if (deviceCount > 0 || status) {
      status = rifCreateContext(RIF_API_VERSION, RIF_BACKEND_API_OPENCL, 0, nullptr, &context);
         if (status != RIF_SUCCESS || !context) return -1;
   }

   status = rifContextCreateCommandQueue(context, &queue);
      if (status != RIF_SUCCESS || !queue) return -1;

// Create input and output images
   inputImage = ImageTools::LoadImage(input_path, context);
      if (!inputImage) {
         printf("Error: 404 image\n");
         return -1;
      }
   
// Create output image
   rif_image_desc desc;
   size_t retSize;

   rifImageGetInfo(inputImage, RIF_IMAGE_DESC, sizeof(desc), &desc, &retSize);
   status = rifContextCreateImage(context, &desc, nullptr, &outputImage);
      if (status != RIF_SUCCESS) return -1;

   printf("\n");
// Create image filter
// Attach filter and set parameters
if (filter_name) {
// Rotating Filters
   if (!strcmp("flip_h", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/horizontal_flip.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_FLIP_HOR,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

   } else if (!strcmp("flip_v", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/vertical_flip.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_FLIP_VERT,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

   } else if (!strcmp("rotate", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/rotation.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_ROTATE,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

// Blurring and Resampling Filters
   } else if (!strcmp("ai_upscale", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ai_upscale.html
   /*
    *    not work with Polaris11(gfx803) + MIOpen v2.0.5
    */
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_AI_UPSCALE,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      if (!use_default) {
         rif_uint ret_param;
         rif_char ret_model_path[64];

         printf("Select upscale mode : \n [0: Fast] \n [1: Good (Default)] \n [2: Best]\n");
            scanf("%1u%*[^\n]", &ret_param);
   
         switch (ret_param) {
            case 0:
               rifImageFilterSetParameter1u(filter,
                                            "mode",
                                            RIF_AI_UPSCALE_MODE_FAST_2X);
               break;
            case 1:
               rifImageFilterSetParameter1u(filter,
                                            "mode",
                                            RIF_AI_UPSCALE_MODE_GOOD_2X);
               break;
            case 2:
               rifImageFilterSetParameter1u(filter,
                                            "mode",
                                            RIF_AI_UPSCALE_MODE_BEST_2X);
               break;
            deafult:
               rifImageFilterSetParameter1u(filter,
                                            "mode",
                                            RIF_AI_UPSCALE_MODE_FAST_2X);
               break;
         }
         
         printf("Path to model files (Default: ./models) : ");
            scanf("%63s%*[^\n]", ret_model_path);
         
         rifImageFilterSetParameterString(filter, "modelPath", ret_model_path);
      }

   } else if (!strcmp("motion_blur", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/motion_blur.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_MOTION_BLUR,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

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

// Tone Mapping and Color Changing Filters
   } else if (!strcmp("filmic_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/aces_filmic_tone_mapping.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_FILMIC_TONEMAP,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

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
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_AUTOLINEAR_TONEMAP,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      if (!use_default) {
         rif_float ret_param;
   
         printf("Gamma correction coefficient [Default: 2.2] [0, 5] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "gamma", ret_param);
      }

   } else if (!strcmp("color_space", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/color_space_changing.html
      status = rifContextCreateImageFilter(context,
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
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_DRAGO_TONEMAP,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

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
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_EXPONENTIAL_TONEMAP,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

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
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_FILMIC_UNCHARTED_TONEMAP,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

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
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_GAMMA_CORRECTION,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      if (!use_default) {
         rif_float ret_param;
   
         printf("Gamma [Default: 1] [0, 5] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "gamma", ret_param);
      }

   } else if (!strcmp("hue_saturation", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/hue_saturation_changing.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_HUE_SATURATION,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

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
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_PHOTO_LINEAR_TONEMAP,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

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
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_LINEAR_TONEMAP,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      if (!use_default) {
         rif_float ret_param;
   
         printf("Linear coefficient [Default: 1] [0, 1e+4f(10000)] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "key", ret_param);
      }

   } else if (!strcmp("white_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/max_white_tone_mapping.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_MAXWHITE_TONEMAP,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

   } else if (!strcmp("photo_tonemap", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/photographic_tone_mapper.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_PHOTO_TONEMAP,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

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
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_REINHARD02_TONEMAP,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

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
   } else if (!strcmp("laplace_edge_detect", filter_name) || !strcmp("laplace", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/laplace_edge_detection.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_LAPLACE,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

   } else if (!strcmp("sobel_edge_detect", filter_name) || !strcmp("sobel", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/sobel_edge_detection.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_SOBEL,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

   } else if (!strcmp("sharpness", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/sharpen.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_SHARPEN,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      if (!use_default) {
         rif_float ret_param;
   
         printf("Sharpness [Default: 0.5] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "sharpness", ret_param);
      }

   } else if (!strcmp("bgra_to_rgba", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/bgra_to_rgba_conversion.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_BGRA_TO_RGBA,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

   } else if (!strcmp("bloom", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/bloom.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_BLOOM,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

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

   /*
   } else if (!strcmp("convert"), filter_name) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/convert.html
      
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_CONVERT,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;
   */
   } else if (!strcmp("dilate_erode", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/dilate.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_DILATE_ERODE,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      if (!use_default) {
         rif_uint ret_param;
   
         printf("Operation mode [Default: 0 (Dilate)] [0 (Dilate), 1 (Erode)] : ");
            scanf("%u%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1u(filter, "mode", ret_param);
      }

   } else if (!strcmp("emboss", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/emboss.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_EMBOSS,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;
      
      if (!use_default) {
         rif_uint ret_param;
   
         printf("Radius [Default: 1] [0, 50] : ");
            scanf("%u%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1u(filter, "radius", ret_param);
      }

/*
   } else if (!strcmp("motion_buffer", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/motion_buffer.html
      rif_float ret_param[16] = { -1.94444f, +0.0f, +0.0f, +0.0f, +0.0f, +1.94444f, +0.0f, -1.94444f, +0.0f, +0.0f, +1.0f, -3.0f, +0.0f, +0.0f, +1.0f, -3.0f, };
      
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_MOTION_BUFFER,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      rifImageFilterSetParameter16f(filter, "viewProjMatrix", ret_param);

   } else if (!strcmp("ndc_depth", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ndc_depth.html
      rif_float ret_param[16] = { -1.94444f, +0.0f, +0.0f, +0.0f, +0.0f, +1.94444f, +0.0f, -1.94444f, +0.0f, +0.0f, +1.0f, -3.0f, +0.0f, +0.0f, +1.0f, -3.0f, };

      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_NDC_DEPTH,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      rifImageFilterSetParameter16f(filter, "viewProjMatrix", ret_param);

*/
   } else if (!strcmp("posterize", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/posterization.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_POSTERIZE,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      if (!use_default) {
         rif_int ret_param;
   
         printf("Levels [Default: 1] [0, 65536] : ");
            scanf("%i%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1u(filter, "levels", ret_param);
      }
   
   } else if (!strcmp("rgb_noise", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/rgb_noise.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_RGB_NOISE,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      if (!use_default) {
         rif_float ret_param;
         
         printf("Levels [Default: 0.5] [0, 1] : ");
            scanf("%f%*[^\n]", &ret_param);
   
         rifImageFilterSetParameter1f(filter, "level", ret_param);
      }
   
   } else if (!strcmp("spread", filter_name)) {
   // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/spread.html
      status = rifContextCreateImageFilter(context,
                                          RIF_IMAGE_FILTER_SPREAD,
                                          &filter);
         if (status != RIF_SUCCESS) return -1;

      if (!use_default) {
         rif_uint ret_param[2];
   
         printf("Offsets [(x, y)] : ");
            scanf("%u%*[^\n], %u%*[^\n],", &ret_param[0], &ret_param[1]);
   
         rifImageFilterSetParameter2u(filter, "offsets", ret_param[0], ret_param[1]);
      }

   } else {
      printf("  Unknown filter name: %s\n", filter_name);
      return -1;
   }

} else {
   return -1;
}

   status = rifCommandQueueAttachImageFilter(queue, filter, inputImage, outputImage);
      if (status != RIF_SUCCESS) {
         printf("rifCommandQueueAttachImageFilter fail\n");
         return -1;
      }

// Execute queue
   status = rifContextExecuteCommandQueue(context, queue, nullptr, nullptr, nullptr);
      if (status != RIF_SUCCESS) return -1;

   status = ImageTools::SaveImage(outputImage, output_path);
      if (status) {
         printf("\nSuccess\n");
      } else {
         printf("\nError: output image\n");
         return -1;
      }
   
// Free resources
   rifCommandQueueDetachImageFilter(queue, filter);
   rifObjectDelete(inputImage);
   rifObjectDelete(outputImage);
   rifObjectDelete(filter);
   rifObjectDelete(queue);
   rifObjectDelete(context);

   return 0;
}