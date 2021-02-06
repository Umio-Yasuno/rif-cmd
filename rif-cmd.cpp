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

#include "rif-cmd.h"

// g++ rif-cmd.cpp -L./RadeonImageFilter/Ubuntu18/Dynamic/ -L./RadeonImageFilter/Ubuntu18/Static/ -lRadeonImageFilters -o rif-cmd

#if defined(WIN32) || defined(_WIN32)
   #define BACKEND_TYPE RIF_BACKEND_API_DIRECTX12
#elif __APPLE__
   #define BACKEND_TYPE RIF_BACKEND_API_METAL
#else
   #define BACKEND_TYPE RIF_BACKEND_API_OPENCL
#endif

#define MAX_FILTER 9

int main(int argc, char *argv[]) {

   int i, select_device = 0, quality = 0, backend = BACKEND_TYPE, filter_count = 0;
   rif_bool use_default = false;
   const rif_char *input_path = NULL, *input_ext = NULL, *output_ext = NULL;
   const rif_char *output_path = "out.png";

   Param filter_param[MAX_FILTER];


   for (i=1; i < argc; i++) {
      if (!strcmp("-i", argv[i])) {
         if (++i < argc) {
            FILE *file_check  = fopen(argv[i], "r");

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
         if (++i < argc) {

            output_path = argv[i];
            output_ext  = strrchr(output_path, '.');
         } else {
            printf("Error: Please enter the path of output image\n");
               return -1;
         }
      } else if (!strcmp("-f", argv[i]) || !strcmp("--filter", argv[i])) {

         if (++i < argc) {
            if (filter_count < MAX_FILTER) {
               filter_param[filter_count].filter_name = argv[i];

            } else {
               printf("Up to %d filters\n", MAX_FILTER);
               return -1;
            }
         } else {
            printf("Error: Please enter the filter name\n");
               return -1;
         }

         if (i+1 < argc && strncmp("-", argv[i+1], 1)) {
            i++;
            int j = 0, length = strlen(argv[i]);
            filter_param[filter_count].count_param = 1;
            const char *delim = ",";

            strncpy(filter_param[filter_count].param_string, argv[i], 127);

            for (j=0; j < length; j++) {
               if (filter_param[filter_count].param_string[j] == delim[0]) {
                  filter_param[filter_count].count_param++;
               }
            }
         }

         filter_count++;

      } else if (!strcmp("-d", argv[i]) || !strcmp("--default", argv[i])) {
         printf("use default value\n");
         use_default = true;

      } else if (!strcmp("-v", argv[i]) || !strcmp("--version", argv[i])) {
         printf("rif-cmd version: 0.0.0\n");
         printf("RIF API version: %s\n", RIF_API_VERSION_STRING);
            return 0;

      } else if (!strcmp("-q", argv[i])) {
         if (++i < argc && strncmp("-", argv[i], 1)) {
            quality = atoi(argv[i]);

         } else {
            printf("Error: Please enter the quality value\n");
               return -1;
         }
      } else if (!strcmp("-g", argv[i]) || !strcmp("--gpu", argv[i])) {
         if (++i < argc && strncmp("-", argv[i], 1)) {

            select_device = atoi(argv[i]);
         } else {
            printf("Error: Please enter the ID of the select device\n");
               return -1;
         }
      } else if (!strcmp("--trace", argv[i])) {
      // https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/tracing.html
         if (++i < argc) {
            setenv("RIF_TRACING_ENABLED", "1", 0);
            setenv("RIF_TRACING_PATH", argv[i], 0);

         } else {
            printf("Error: Please enter the path to output the tracing file\n");
               return -1;
         }
      } else if (!strcmp("--api", argv[i])) {
         if (++i < argc) {

            if (!strcmp("dx12", argv[i])) {
               backend = RIF_BACKEND_API_DIRECTX12;
            } else if (!strcmp("metal", argv[i])) {
               backend = RIF_BACKEND_API_METAL;
            } else if (!strcmp("ocl", argv[i]) || !strcmp("opencl", argv[i])) {
               backend = RIF_BACKEND_API_OPENCL;
            } else {
               printf("Unknown API: %s\n"
                      "(Supported API: opencl/ocl, dx12, metal)\n", argv[i]);
                  return -1;
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

   rif_int              status      = RIF_SUCCESS;
   rif_context          context     = nullptr;
   rif_command_queue    queue       = nullptr;
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
   int device_count = 0;
   status = rifGetDeviceCount(backend, &device_count);
      if (status != RIF_SUCCESS) return -1;

   if (device_count < select_device) {
      printf("[ERROR] non-existent device\n");
         return -1;
   }

   if (device_count > 0 || status) {
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
   rif_image_filter filter[filter_count] = { nullptr };

for (i=0; i < filter_count; i++) {

   status = set_param(context,
                      filter_param[i],
                      filter[i],
                      &filter[i],
                      use_default,
                      &output_desc);

   if (status != RIF_SUCCESS) {
      printf("[ERROR] set_param function failed\n");
      if (status == PARAM_ERROR) {
         printf("[ERROR] There are too few or too many parameters\n"
                "  https://github.com/Umio-Yasuno/rif-cmd/blob/main/FILTERS.md\n"
                "  https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters.html\n");
      }
      return -1;
   }

/*
*  https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/info_setting_types/rif_compute_type.html
*  RIF_COMPUTE_TYPE_FLOAT  0x0
*  RIF_COMPUTE_TYPE_HALF   0x1
*  https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/api/rifimagefiltersetcomputetype.html
*  rifImageFilterSetComputeType(filter, RIF_COMPUTE_TYPE_FLOAT);
*/
/*
 *    https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/combining_filters.html
 *
 *    with buffer
 */
/*
   if (i == 0) {
   //      printf("\ti == 0\n");
      rifImageFilterSetParameterImage(filter[i], "srcBuffer", inputImage);
   } else if (i > 0 && i < filter_count - 1) {
   //      printf("\ti > 0 && i < filter_count - 1\n");
      rifImageFilterSetParameterImage(filter[i], "srcBuffer",
                                      (rif_image)(filter[i-1]));
   } else if (i == filter_count - 1) {
   //      printf("\ti == filter_count - 1\n");
      rifCommandQueueAttachImageFilter(queue, filter[i],
                                       (rif_image)(filter[i-1]), outputImage);
   }
*/
/*
 *    without buffer
 */
   
   if (i % 2 == 0) {
      rifCommandQueueAttachImageFilter(queue, filter[i], inputImage,  outputImage);
   } else {
      rifCommandQueueAttachImageFilter(queue, filter[i], outputImage, inputImage);
   }

   if (i == filter_count - 1 && i % 2 == 1) {
      rif_image_filter fill_filter = nullptr;

      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_CONVERT,
                                  &fill_filter);

      rifCommandQueueAttachImageFilter(queue, fill_filter, inputImage, outputImage);
   }
//    rifCommandQueueAttachImageFilter(queue, nullptr, inputImage,  outputImage);
//    rifCommandQueueAttachImageFilter(queue, filter[i], inputImage, outputImage);
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
      printf("\nSuccess: %s\n", output_path);
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
