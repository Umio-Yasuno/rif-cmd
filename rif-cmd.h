#ifndef _RIF_CMD_H_
#define _RIF_CMD_H_

#include "RadeonImageFilter/include/RadeonImageFilters.h"
#include "RadeonImageFilter/include/RadeonImageFilters_version.h"

#define PARAM_ERROR -3

typedef struct {
   const char* filter_name = NULL;
   int         count_param = 0;
   char        param_string[128];
} Param;

int set_param(rif_context       context,
              Param             filter_param,
              rif_image_filter  filter,
              rif_image_filter  *filterptr,
              rif_bool          use_default,
              rif_image_desc    *output_desc);

#endif  // _RIF_CMD_H_  
