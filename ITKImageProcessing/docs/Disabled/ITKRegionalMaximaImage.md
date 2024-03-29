# ITK::Regional Maxima Image Filter

This filter directly wraps an ITK filter of the same name.

## Group (Subgroup) ##

ITKImageProcessing (ITKImageProcessing)

## Description ##

Produce a binary image where foreground is the regional maxima of the input image.

Regional maxima are flat zones surrounded by pixels of lower value.

If the input image is constant, the entire image can be considered as a maxima or not. The desired behavior can be selected with the SetFlatIsMaxima() method.

\author Gaetan Lehmann

This class was contributed to the Insight Journal by author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France. The paper can be found at https://hdl.handle.net/1926/153

\see ValuedRegionalMaximaImageFilter

\see HConvexImageFilter

\see RegionalMinimaImageFilter

\par Wiki Examples:

\li All Examples

\li RegionalMaximaImageFilter

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| BackgroundValue | double| Set/Get the value used as "background" in the output image. Defaults to NumericTraits<PixelType>::NonpositiveMin() . |
| ForegroundValue | double| Set/Get the value in the output image to consider as "foreground". Defaults to maximum value of PixelType. |
| FullyConnected | bool| Set/Get whether the connected components are defined strictly by face connectivity or by face+edge+vertex connectivity. Default is FullyConnectedOff. For objects that are 1 pixel wide, use FullyConnectedOn. |
| FlatIsMaxima | bool| Set/Get wether a flat image must be considered as a maxima or not. Defaults to true. |


## Required Geometry ##

Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | None | N/A | (1)  | Array containing input image

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | None | uint32_t | (1)  | Array containing filtered image

## References ##

[1] T.S. Yoo, M. J. Ackerman, W. E. Lorensen, W. Schroeder, V. Chalana, S. Aylward, D. Metaxas, R. Whitaker. Engineering and Algorithm Design for an Image Processing API: A Technical Report on ITK - The Insight Toolkit. In Proc. of Medicine Meets Virtual Reality, J. Westwood, ed., IOS Press Amsterdam pp 586-592 (2002). 
[2] H. Johnson, M. McCormick, L. Ibanez. The ITK Software Guide: Design and Functionality. Fourth Edition. Published by Kitware Inc. 2015 ISBN: 9781-930934-28-3
[3] H. Johnson, M. McCormick, L. Ibanez. The ITK Software Guide: Introduction and Development Guidelines. Fourth Edition. Published by Kitware Inc. 2015 ISBN: 9781-930934-27-6

## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
