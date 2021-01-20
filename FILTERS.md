# Filters

[Filters — AMD Radeon ProRender](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters.html)

 * [Rotating Filters](https://github.com/Umio-Yasuno/rif-cmd/blob/main/FILTERS.md#rotating-filters)
 * [Blurring and Resampling Filters](https://github.com/Umio-Yasuno/rif-cmd/blob/main/FILTERS.md#blurring-and-resampling-filters)
 * [Tone Mapping and Color Changing Filters](https://github.com/Umio-Yasuno/rif-cmd/blob/main/FILTERS.md#tone-mapping-and-color-changing-filters)
 * [Denoising Filters](https://github.com/Umio-Yasuno/rif-cmd/blob/main/FILTERS.md#denoising-filters)
 * [Antialiasing Filters](https://github.com/Umio-Yasuno/rif-cmd/blob/main/FILTERS.md#antialiasing-filters)
 * [Edge Detection](https://github.com/Umio-Yasuno/rif-cmd/blob/main/FILTERS.md#edge-detection)
 * [Blending Filters](https://github.com/Umio-Yasuno/rif-cmd/blob/main/FILTERS.md#blending-filters)
 * [Image Enhance Filters](https://github.com/Umio-Yasuno/rif-cmd/blob/main/FILTERS.md#image-enhance-filters)
 * [Other Filters](https://github.com/Umio-Yasuno/rif-cmd/blob/main/FILTERS.md#other-filters)


## Rotating Filters

| Filter | filter name<br>in rif-cmd | Document Link |
| :--    | :--:                      | :--:          |
| Horizontal Flip Filter | "filp_h" | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/horizontal_flip.html) |
| Vertical Flip Filter   | "filp_v" | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/vertical_flip.html)
| Rotation Filter        | "rotate" | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/rotation.html) |


## Blurring and Resampling Filters

| Filter | filter name<br>in rif-cmd | Document Link |
| :--    |  :--:                     | :--:          |
| AI Upscale Filter         | "ai_upscale" | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ai_upscale.html) |
| Depth of Field Filter     | not yet implemented <!-- depth_of_filed --> | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/depth_of_field.html) |
| Gaussian Blurring Filter  | gaussian_blur | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/gaussian_blurring.html) |
| Motion Blur Filter        | "motion_blur" | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/motion_blur.html) |
| Resampling Filter         | "resample"    | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/resampling.html) |
| Dynamic Resampling Filter | "dynamic_resample" | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/dynamic_resampling.html) |


## Tone Mapping and Color Changing Filters

| Filter | filter name<br>in rif-cmd | Document Link |
| :--    | :--:                      | :--:          |
| ACES Filmic Tone Mapping Filter       | "filmic_tonemap"          | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/aces_filmic_tone_mapping.html) |
| Auto Linear Tone Mapping Filter       | "autolinear_tonemap"      | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/auto_linear_tone_mapping.html)
| Color Space Changing Filter           | "color_space"             | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/color_space_changing.html) |
| Drago Tone Mapping Filter             | "drago_tonemap"           | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/drago_tone_mapping.html) |
| Exponential Tone Mapping Filter       | "exponential_tonemap"     | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/exponential_tone_mapping.html) |
| Filmic Uncharted Tone Mapping Filter  | "filmic_uncharted_tonemap"| [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/filmic_uncharted_tone_mapping.html)
| Gamma Correction Filter               | "gamma_correct"           | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/gamma_correction.html) |
| Hue Saturation Changing Filter        | "hue_saturation"          | [LINK](https://github.com/Umio-Yasuno/rif-cmd/blob/main/rif-cmd.cpp) |
| Linear Photographic Tone Mapping Filter | "photo_linear_tonemap"  | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/linear_photographic_tone_mapping.html) |
| Linear Tone Mapping Filter            | "linear_tonemap"          | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/linear_tone_mapping.html)
| Max White Tone Mapping Filter         | "white_tonemap"           | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/max_white_tone_mapping.html) |
| Photographic Tone Mapping Filter      | "photo_tonemap"           | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/photographic_tone_mapper.html) |
| Reinhard Tone Mapping Filter          | "reinhard_tonemap"        | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/reinhard_tone_mapping.html) |


## Denoising Filters

| Filter | filter name<br>in rif-cmd | Document Link |
| :--    | :--:                      | :--:          |
| AI Denoiser Filter                    | not yet implemented <!-- ai_denoise -->           | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ai_denoiser.html) |
| Bilateral Denoiser Filter             | not yet implemented <!-- bilateral_denoise -->    | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/bilateral_denoiser.html) |
| Edge-avoiding À-Trous Wavelet Filter  | not yet implemented <!-- eaw_denoise -->          | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/edge_avoiding_wavelet.html) |
| Local Weighted Regression Filter      | not yet implemented <!-- eaw_denoise -->          | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/local_weighted_regression.html) |
| Median Denoiser Filter               | median_denoise | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/median_denoiser.html) |
| Temporal Accumulation Filter         | not yet implemented <!-- median_denoise -->       | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/temporal_accumulation.html)


## Antialiasing Filters

| Filter | filter name<br>in rif-cmd | Document Link |
| :--    | :--:                      | :--:          |
| Morphological Antialiasing Filter | not yet implemented <!-- mlaa --> | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/morphological_antialiasing.html) |


## Edge Detection

| Filter | filter name<br>in rif-cmd | Document Link |
| :--    | :--:                      | :--:          |
| Laplace Edge Detection Filter | "laplace" | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/laplace_edge_detection.html) |
| Sobel Edge Detection Filter   | "sobel"   | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/sobel_edge_detection.html)   |


## Blending Filters

| Filter | filter name<br>in rif-cmd | Document Link |
| :--    | :--:                      | :--:          |
| Multiplication Filter | not yet implemented <!-- mult -->         | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/multiplication.html) |
| Weighted Sum Filter   | not yet implemented <!-- weighted_sum --> | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/weighted_sum.html) |


## Image Enhance Filters

| Filter | filter name<br>in rif-cmd | Document Link |
| :--    | :--:                      | :--:          |
| Sharpen Filter | "sharpness" | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/sharpen.html) |


## Other Filters

| Filter | filter name<br>in rif-cmd | Document Link |
| :--    | :--:                      | :--:          |
| AI Custom Model Filter            | not yet implemented <!-- ai_model --> | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ai_custom_model.html) |
| BGRA TO RGBA Conversion Filter    | "bgra_to_rgba"                        | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/bgra_to_rgba_conversion.html) |
| Binary Operation Filter           | not yet implemented <!-- binary_op -->| [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/binary_operation.html) |
| Bloom Filter                      | "bloom"                               | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/bloom.html) |
| Convert Filter                    | not yet implemented <!-- convert -->  | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/convert.html) |
| Dilate (Erode) Filter             | "dilate_erode"                        | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/dilate.html) |
| Emboss Filter                     | "emboss"                              | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/emboss.html) |
| Motion Buffer Filter              | not yet implemented <!-- motion_buffer --> | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/motion_buffer.html) |
| NDC Depth Filter                  | not yet implemented <!-- ndc_depth -->| [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/ndc_depth.html) |
| Posterization Filter              | "posterize"                           | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/posterization.html) |
| Range Remap Filter                | not yet implemented <!-- remap -->    | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/range_remap.html) |
| RGB Noise Filter                  | "rgb_noise"                           | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/rgb_noise.html) |
| Spread Filter                     | "spread"                              | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/spread.html) |
| User Defined Filter               | not yet implemented <!-- user_defined --> | [LINK](https://radeon-pro.github.io/RadeonProRenderDocs/en/rif/filters/user_defined.html) |
