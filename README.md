# rif-cmd

rif-cmd is a command line tool for image processing with [RadeonImageFilter](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonImageFilter).  

## Build

    g++ rif-cmd.cpp \ 
    -L./RadeonImageFilter/Ubuntu18/Dynamic/ -L./RadeonImageFilter/Ubuntu18/Static/ \ 
    -lRadeonImageFilters -o rif-cmd
    
## Usage
```
 export LD_LIBRARY_PATH="./RadeonImageFilter/Ubuntu18/Dynamic/":$LD_LIBRARY_PATH
```

```
OPTIONS:
  -i <path>                       input image path
  -o <path>                      output image path
  -f, --filter <name>            filter name       
FLAGS:
  -d, --default                 use default parameter
```

## Supported

  * Linux x86_64 (OpenCL)
  
## Known Issues

  * AI Upscale filter not work with gfx803 + MIOpen v2.0.5.  
    * It's probably problem MIOpen.
      * [ROCm 2.7 Unable to find a suitable algorithm for doing forward convolution · Issue #869 · RadeonOpenCompute/ROCm](https://github.com/RadeonOpenCompute/ROCm/issues/869)
  
  * Filter $X is not implemented.
    * I'll add it soon, maybe.
