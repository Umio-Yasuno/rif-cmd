#ifdef  _RIF_CMD_H_
#define _RIF_CMD_H_

#include "RadeonImageFilter/include/RadeonImageFilters.h"
#include "RadeonImageFilter/include/RadeonImageFilters_version.h"

int set_param(rif_context       context,
              const rif_char    *filter_name,
              rif_image_filter  filter,
              rif_image_filter  *filterptr,
              rif_bool          use_default,
              rif_image_desc    *output_desc)


#endif  // _RIF_CMD_H_  
