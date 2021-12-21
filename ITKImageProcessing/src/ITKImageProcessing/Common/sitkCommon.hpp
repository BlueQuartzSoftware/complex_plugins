#pragma once

#include <itkFlatStructuringElement.h>
// Copied from sitkKernel.h (SimpleITK)
// Copied from sitkCreateKernel.h (SimpleITK)
namespace itk
{
namespace simple
{
enum PixelIDTypeList
{
  BasicPixelIDTypeList = 1,       // uint8, int8, uint16, int16, uint32, int32, float, double
  IntegerPixelIDTypeList = 2,     // uint8, int8, uint16, int16, uint32, int32
  NonLabelPixelIDTypeList = 3,    //  BasicPixelIDTypeList, ComplexPixelIDTypeList, VectorPixelIDTypeList
  RealPixelIDTypeList = 4,        // float, double
  RealVectorPixelIDTypeList = 5,  // VectorPixelID< float >, VectorPixelID< double >
  ScalarPixelIDTypeList = 6,      // uint8, int8, uint16, int16, uint32, int32, float, double
  SignedPixelIDTypeList = 7,      // int8, int16, int32, float, double
  VectorPixelIDTypeList = 8,      // Vector: uint8, int8, uint16, int16, uint32, int32, float, double
  SignedVectorPixelIDTypeList = 9 // Vector: int8, int16, int32, float, double
};
enum PixelIDValueEnum
{
  sitkUnknown = -1,
  sitkInt8 = -2,
  sitkUInt8 = -3
};
enum KernelEnum
{
  sitkAnnulus = 0,
  sitkBall = 1,
  sitkBox = 2,
  sitkCross = 3
};

enum SeedEnum
{
  /// A sentinel value used for "seed" parameters to indicate it
  /// should be initialized by the wall clock for pseudo-random behavior.
  sitkWallClock = 0
};

template <unsigned int Dimension>
itk::FlatStructuringElement<Dimension> CreateKernel(KernelEnum kernelType, const std::vector<uint32_t>& size)
{
  using StructuringElementType = itk::FlatStructuringElement<Dimension>;
  using RadiusType = typename StructuringElementType::RadiusType;

  RadiusType elementRadius;
  if(RadiusType::Dimension > 0)
  {
    elementRadius[0] = static_cast<typename RadiusType::SizeValueType>(size[0]);
    if(RadiusType::Dimension > 1)
    {
      elementRadius[1] = static_cast<typename RadiusType::SizeValueType>(size[1]);
      if(RadiusType::Dimension > 2)
      {
        elementRadius[2] = static_cast<typename RadiusType::SizeValueType>(size[2]);
      }
    }
  }

  StructuringElementType structuringElement;
  switch(kernelType)
  {
  case KernelEnum::sitkAnnulus:
    structuringElement = StructuringElementType::Annulus(elementRadius, false);
    break;
  case KernelEnum::sitkBall:
    structuringElement = StructuringElementType::Ball(elementRadius, false);
    break;
  case KernelEnum::sitkBox:
    structuringElement = StructuringElementType::Box(elementRadius);
    break;
  case KernelEnum::sitkCross:
    structuringElement = StructuringElementType::Cross(elementRadius);
    break;
  default:
    break;
  }
  return structuringElement;
}
template <unsigned int Dimension>
itk::FlatStructuringElement<Dimension> CreateKernel(KernelEnum kernelType, const std::vector<float>& sizef)
{
  std::vector<uint32_t> size;
  for(const auto& f : sizef)
  {
    size.push_back(static_cast<uint32_t>(f));
  }
  typedef itk::FlatStructuringElement<Dimension> StructuringElementType;
  typedef typename StructuringElementType::RadiusType RadiusType;
  RadiusType elementRadius;
  if(RadiusType::Dimension > 0)
  {
    elementRadius[0] = static_cast<typename RadiusType::SizeValueType>(size[0]);
    if(RadiusType::Dimension > 1)
    {
      elementRadius[1] = static_cast<typename RadiusType::SizeValueType>(size[1]);
      if(RadiusType::Dimension > 2)
      {
        elementRadius[2] = static_cast<typename RadiusType::SizeValueType>(size[2]);
      }
    }
  }

  StructuringElementType structuringElement;
  switch(kernelType)
  {
  case 0:
    structuringElement = StructuringElementType::Annulus(elementRadius, false);
    break;
  case 1:
    structuringElement = StructuringElementType::Ball(elementRadius, false);
    break;
  case 2:
    structuringElement = StructuringElementType::Box(elementRadius);
    break;
  case 3:
    structuringElement = StructuringElementType::Cross(elementRadius);
    break;
  default:
    break;
  }
  return structuringElement;
}

} // end namespace simple
} // end namespace itk
