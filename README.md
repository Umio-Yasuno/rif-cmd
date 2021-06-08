# rif-cmd

rif-cmd is a command line tool for image processing with [RadeonImageFilter](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonImageFilter).  

## Build

    mkdir build
    cd build
    cmake ../
    make
    
## Usage
### Ubuntu

    export LD_LIBRARY_PATH="./RadeonImageFilter/Ubuntu18/Dynamic/":$LD_LIBRARY_PATH

```
OPTIONS:
  -i <path>                     input image path
  -o <path>                     output image path
  -f, --filter <name>           filter name
  -q <value>                    JPEG quality [0, 100]
  -g, --gpu <device_id>         select device
  --api <api name>              Backend API (ocl/opencl, dx12, metal)

  --trace <dir>                 RIF tracing for debugging
      <https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/tracing.html>

FLAGS:
  -d, --default                 use default parameter
  --perf                        outputs performance statistic time
  -h, --help                    print help

```

### example

```
//  Basic
$ ./rif-cmd -i input.png -o output.png -f bloom
//   or
$ ./rif-cmd -i input.png -o output.png -f bloom "0.1,0,0.1,1"

//  Combining Filter
$ ./rif-cmd -i input.png -o output.png -f bloom "0.1,0,0.1,1" -f motion_blur "5,1,1"

```

## Filters

[FILTERS.md](./FILTERS.md)

## Supported

  * OpenCL, DX12, Metal

  RIF SDK is a cross-platform. (DX12, OpenCL, Metal)  

## Known Issues

  * AI Upscale filter not work with gfx803 + MIOpen v2.0.5.  
    * <https://github.com/GPUOpen-LibrariesAndSDKs/RadeonImageFilter/issues/13>
  * Filter $X is not implemented.
    * I'll add it soon, maybe.

## RIF Documents

 * [Radeon Image Filters — AMD Radeon ProRender](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/about.html)
 * [Radeon™ Image Filtering Library - GPUOpen](https://gpuopen.com/radeon-image-filtering-library/)
