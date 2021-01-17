# rif-cmd

rif-cmd is a command line tool for image processing with [RadeonImageFilter](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonImageFilter).  

## Build

    g++ rif-cmd.cpp \ 
    -L./RadeonImageFilter/Ubuntu18/Dynamic/ \
    -L./RadeonImageFilter/Ubuntu18/Static/ \ 
    -lRadeonImageFilters -o rif-cmd
    
## Usage

    export LD_LIBRARY_PATH="./RadeonImageFilter/Ubuntu18/Dynamic/":$LD_LIBRARY_PATH

```
OPTIONS:
  -i <path>                       input image path
  -o <path>                      output image path
  -f, --filter <name>            filter name
FLAGS:
  -d, --default                 use default parameter
```

## Filters

[FILTERS.md](./FILTERS.md)

## Supported

  * Linux x86_64 (OpenCL)

  RIF SDK is a cross-platform. (DX12, OpenCL, Metal)  
  I can't verify it because I don't have the environment, but I'll add support.  

## Known Issues

  * AI Upscale filter not work with gfx803 + MIOpen v2.0.5.  
    * It's probably problem MIOpen.
      * [ROCm 2.7 Unable to find a suitable algorithm for doing forward convolution · Issue #869 · RadeonOpenCompute/ROCm](https://github.com/RadeonOpenCompute/ROCm/issues/869)
  
  * Filter $X is not implemented.
    * I'll add it soon, maybe.

## RIF Document

 * [Radeon Image Filters — AMD Radeon ProRender](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/about.html)
 * [Radeon™ Image Filtering Library - GPUOpen](https://gpuopen.com/radeon-image-filtering-library/)
