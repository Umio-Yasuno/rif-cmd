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

#include "../RadeonImageFilter/include/RadeonImageFilters.h"
#include "../RadeonImageFilter/include/RadeonImageFilters_version.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../RadeonImageFilter/samples/ImageTools/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../RadeonImageFilter/samples/ImageTools/stb_image_write.h"

#include "../RadeonImageFilter/samples/ImageTools/ImageTools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
// #include <malloc.h>

#include "rif-cmd.h"

#if defined(WIN32) || defined(_WIN32)
   #define BACKEND_TYPE RIF_BACKEND_API_DIRECTX12
#elif __APPLE__
   #define BACKEND_TYPE RIF_BACKEND_API_METAL
#else
   #define BACKEND_TYPE RIF_BACKEND_API_OPENCL
#endif

#define MAX_FILTER 9

static void help_print()
{
   printf("\nOPTIONS:\n"
          "  -i <path>                     input image path\n"
          "  -o <path>                     output image path\n"
          "  -f, --filter <name>           filter name\n"
          "  -q <value>                    JPEG quality [0, 100]\n"
          "  -g, --gpu <device_id>         select device\n"
          "  --api <api name>              Backend API (ocl/opencl, dx12, metal)\n\n"
          "  --trace <dir>                 RIF tracing for debugging\n"
          "      <https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/tracing.html>\n"
          "\nFLAGS:\n"
          "  -d, --default                 use default parameter\n"
          "  --perf                        outputs performance statistic time\n"
          "  -h, --help                    print help\n\n");
}

int i, select_device = 0, quality = 0, backend = BACKEND_TYPE, filter_count = 0;
bool use_default = false, perf_output = false;
const char *input_path = NULL, *input_ext = NULL, *output_ext = NULL, *output_path = "out.png";
Param filter_param[MAX_FILTER];

static int opt_parse(int argc, char *argv[])
{
   for (i=1; i < argc; i++) {
      if (!strcmp("-i", argv[i])) {
         if (++i < argc) {

            if (!access(argv[i], F_OK)) {
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
            char delim[] = ",";

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

      } else if (!strcmp("--perf", argv[i])) {
         perf_output = true;
         
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
      } else if (!strcmp("--help", argv[i]) || !strcmp("-h", argv[i])) {
         help_print();
            return 0;
      } else {
         printf("Error: Unknown Option \"%s\"\n", argv[i]);
         help_print();
            return -1;
      }
   }
   return 0;
}

static void api_backend_print(int backend)
{
   char api_name[16];
   size_t ret = sizeof(api_name);

   switch (backend) {
      case RIF_BACKEND_API_OPENCL:
         strncpy(api_name, "OpenCL",  ret);
         break;
      case RIF_BACKEND_API_DIRECTX12: 
         strncpy(api_name, "DX12",    ret);
         break;
      case RIF_BACKEND_API_METAL:
         strncpy(api_name, "Metal",   ret);
         break;
      default:
         strncpy(api_name, "Unknown", ret);
         break;
   }

   printf("Backend API: \t %s\n", api_name);
}

static void get_device_info(int backend, int deviceid)
{
   char clinfo[128], d_name[128], vendor[128];
   size_t ret;

   rifGetDeviceInfo(backend, deviceid, RIF_DEVICE_NAME, 
                     sizeof(d_name), &d_name, &ret);
   rifGetDeviceInfo(backend, deviceid, RIF_DEVICE_VENDOR, 
                     sizeof(vendor), &vendor, &ret);

   printf("Vendor: \t %s\n"
          "Device Name: \t %s\n\n",
          vendor, d_name);
}

/*
 *    RadeonImageFilter/RadeonImageFilters.h#L327
 *
 *    struct _rif_performance_statistic
 *    {
 *       rif_uint64 execution_time;       // nanosecond, 1 / 1e9, 1e-9
 *       rif_bool measure_execution_time;
 *       rif_float  compile_time;         // millisecond, 1 / 1e3, 1e-3
 *       rif_bool measure_compile_time;
 *    };
 */
static void exe_perf(rif_context ctx, rif_command_queue queue)
{
   uint8_t status = 0;
   rif_performance_statistic perf = {0,1,0,1};
   struct timeval start = {0,0}, end = {0,0};
   double total_time, exe_compile_time;

   gettimeofday(&start, NULL);

   status = rifContextExecuteCommandQueue(ctx, queue, nullptr, nullptr, &perf);
      if (status != RIF_SUCCESS) return;

   rifSyncronizeQueue(queue);
   gettimeofday(&end, NULL);

   total_time        = ((double)(((end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec)))) / 1e6;
//   printf("total_time: %f\n", total_time);
   exe_compile_time  = (double)(perf.execution_time / 1e9 + perf.compile_time / 1e3);

   printf("\nExecution time:\t\t%6.3fs\n"
          "Kernel compile time:\t%6.3fs\n"
          "Other (I/O etc) time:\t%6.3fs\n"
          ,(double)(perf.execution_time / 1e9)
          ,(perf.compile_time / 1e3)
          ,(total_time - exe_compile_time));
}

static int rif_cmd_write_image (rif_image       outputImage,
                                const char      *output_path,
                                const char      *output_ext,
                                rif_image_desc  *output_desc,
                                int             quality)
{
   int status = 0;
   rif_uchar *output_data = nullptr;

   status = rifImageMap(outputImage, RIF_IMAGE_MAP_READ, (void**)&output_data);
      if (status != RIF_SUCCESS) return status;
   // printf("%d, %d", output_desc->image_width, output_desc->image_height);

   if (!strcmp(".jpg", output_ext) || !strcmp(".jpeg", output_ext)) {
   /*
    *     int stbi_write_jpg(char const *filename,
    *                        int w,
    *                        int h,
    *                        int comp,
    *                        const void *data,
    *                        int quality);
    */
      status = stbi_write_jpg(output_path,
                              output_desc->image_width,
                              output_desc->image_height,
                              output_desc->num_components,
                              output_data,
                              quality);

   } else if (!strcmp(".png", output_ext)) {
   /*
    *    int stbi_write_png(char const *filename,
    *                       int w,
    *                       int h,
    *                       int comp,
    *                       const void  *data,
    *                       int stride_in_bytes)
    */
      status = stbi_write_png(output_path,
                              output_desc->image_width,
                              output_desc->image_height,
                              output_desc->num_components,
                              output_data,
                              0);
   }

#if 0
   status = ImageTools::SaveImage(outputImage, output_path);
      if (status) {
         printf("\nSuccess\n");
      } else {
         printf("\nError: output image\n");
         return -1;
      }
#endif

   rifImageUnmap(outputImage, output_data);
   stbi_image_free(output_data);

   if (status) {
      printf("\nSuccess: %s\n", output_path);
   } else {
      printf("\nError: output image\n");
   }

   return status;
}

static void print_context_info(rif_context context)
{
   char kernel_src[256], kernel_cache[256];
   size_t ret;
   rifContextGetInfo(context, RIF_CONTEXT_KERNELS_SOURCE_DIR, sizeof(kernel_src),   &kernel_src,   &ret);
   rifContextGetInfo(context, RIF_CONTEXT_KERNELS_CACHE_DIR,  sizeof(kernel_cache), &kernel_cache, &ret);
   printf("Kernel Source Dir: %s\n", kernel_src);
   printf("Kernel Cache  Dir: %s\n", kernel_cache);
}

static void rif_cmd_attach_filter(rif_context          context,
                                  rif_command_queue    queue,
                                  rif_image_filter     filter,
                                  int                  filter_number,
                                  rif_image            inputImage,
                                  rif_image            outputImage)
{
/*
 *    https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/info_setting_types/rif_compute_type.html
 *    RIF_COMPUTE_TYPE_FLOAT  0x0
 *    RIF_COMPUTE_TYPE_HALF   0x1
 *    https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/api/rifimagefiltersetcomputetype.html
 *    rifImageFilterSetComputeType(filter, RIF_COMPUTE_TYPE_FLOAT);
 */
/*    with buffer
 *    https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/combining_filters.html
 */
#if 0
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
#endif
   /* without buffer */
   if (filter_number % 2 == 0) {
      rifCommandQueueAttachImageFilter(queue, filter, inputImage,  outputImage);
   } else {
      rifCommandQueueAttachImageFilter(queue, filter, outputImage, inputImage);
   }

   if (filter_number == filter_count - 1 && filter_number % 2 == 1) {
      rif_image_filter fill_filter = nullptr;

      rifContextCreateImageFilter(context,
                                  RIF_IMAGE_FILTER_CONVERT,
                                  &fill_filter);

      rifCommandQueueAttachImageFilter(queue, fill_filter, inputImage, outputImage);
   }
}


int main(int argc, char *argv[])
{

   if (opt_parse(argc, argv) != 0) {
      printf("Failed opt_parse function\n");
      return -1;
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

   api_backend_print(backend);
   get_device_info(backend, select_device);

   /* First create context and queue */
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
#if 0
   rif_image_desc input_desc;
//   memset(&input_desc, 0, sizeof(input_desc));

   rif_int width, height, num_comp;
   rif_float *data = nullptr;
   rif_uchar *raw_data = stbi_load(input_path, &width, &height, &num_comp, 3);

   int array_size  = width * height * num_comp;
   printf("array_size: %d\n", array_size);
   data = (rif_float*) malloc(array_size * sizeof(rif_float) + 1);

   for (i=0; i < array_size; i++) {
//      printf("%f", (rif_float)raw_data[i]);
      data[i] = (rif_float)raw_data[i];
   }
//      return -1;

   printf("w: %d, h: %d, comp: %d\n", width, height, num_comp);
//      return -1;

   input_desc.image_width        =  width;
   input_desc.image_height       =  height;
   input_desc.num_components     =  num_comp;
   input_desc.type               =  RIF_COMPONENT_TYPE_FLOAT32;
   
   rifContextCreateImage(context, &input_desc, data, &inputImage);
//      free(data);
//      data = nullptr;
#endif

   inputImage = ImageTools::LoadImage(input_path, context);

/* Create output image */
   rif_image_desc    output_desc;
   size_t            retSize;

   rifImageGetInfo(inputImage, RIF_IMAGE_DESC, sizeof(output_desc), &output_desc, &retSize);

   if (!strcmp(".jpg", input_ext) || !strcmp(".jpeg", input_ext)) {
      output_desc.type = RIF_COMPONENT_TYPE_UINT8;
   }

   status = rifContextCreateImage(context, &output_desc, nullptr, &outputImage);
      if (status != RIF_SUCCESS) return -1;

// Create image filter
// Attach filter and set parameters
   rif_image_filter filter[MAX_FILTER] = { nullptr };

   for (i=0; i < filter_count; i++) {
      /* set-param.c */
      status = set_param(context,
                         &filter_param[i],
                         filter[i],
                         &filter[i],
                         use_default,
                         &outputImage,
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

      rif_cmd_attach_filter(context,
                            queue,
                            filter[i],
                            i,
                            inputImage,
                            outputImage);
   } /* set filter parm loop */

   print_context_info(context);
   printf("Execute\n");

   /* Execute queue */
   if (perf_output) {
      exe_perf(context, queue);
   } else {
      status = rifContextExecuteCommandQueue(context, queue, nullptr, nullptr, nullptr);
         if (status != RIF_SUCCESS) return -1;
   }

   rif_cmd_write_image(outputImage,
                       output_path,
                       output_ext,
                       &output_desc,
                       quality);

   /* Free resources */
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
