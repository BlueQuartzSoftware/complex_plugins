#include "ITKSpeckleNoiseImage.hpp"

#include "ITKImageProcessing/Common/ITKArrayHelper.hpp"
#include "ITKImageProcessing/Common/sitkCommon.hpp"

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"

#include <itkSpeckleNoiseImageFilter.h>

using namespace complex;

namespace cxITKSpeckleNoiseImage
{
using ArrayOptionsT = ITK::ScalarPixelIdTypeList;
// VectorPixelIDTypeList;

struct ITKSpeckleNoiseImageFunctor
{
  float64 standardDeviation = 1.0;
  uint32 seed = (uint32_t)itk::simple::sitkWallClock;

  template <class InputImageT, class OutputImageT, uint32 Dimension>
  auto createFilter() const
  {
    using FilterT = itk::SpeckleNoiseImageFilter<InputImageT, OutputImageT>;
    auto filter = FilterT::New();
    filter->SetStandardDeviation(standardDeviation);
    filter->SetSeed(seed);
    return filter;
  }
};
} // namespace cxITKSpeckleNoiseImage

namespace complex
{
//------------------------------------------------------------------------------
std::string ITKSpeckleNoiseImage::name() const
{
  return FilterTraits<ITKSpeckleNoiseImage>::name;
}

//------------------------------------------------------------------------------
std::string ITKSpeckleNoiseImage::className() const
{
  return FilterTraits<ITKSpeckleNoiseImage>::className;
}

//------------------------------------------------------------------------------
Uuid ITKSpeckleNoiseImage::uuid() const
{
  return FilterTraits<ITKSpeckleNoiseImage>::uuid;
}

//------------------------------------------------------------------------------
std::string ITKSpeckleNoiseImage::humanName() const
{
  return "ITK Speckle Noise Image Filter";
}

//------------------------------------------------------------------------------
std::vector<std::string> ITKSpeckleNoiseImage::defaultTags() const
{
  return {"ITKImageProcessing", "ITKSpeckleNoiseImage", "ITKImageNoise", "ImageNoise"};
}

//------------------------------------------------------------------------------
Parameters ITKSpeckleNoiseImage::parameters() const
{
  Parameters params;
  params.insertSeparator(Parameters::Separator{"Filter Parameters"});
  params.insert(std::make_unique<Float64Parameter>(k_StandardDeviation_Key, "StandardDeviation", "", 1.0));
  params.insert(std::make_unique<UInt32Parameter>(k_Seed_Key, "Seed", "", (uint32_t)itk::simple::sitkWallClock));

  params.insertSeparator(Parameters::Separator{"Input Data Structure Items"});
  params.insert(std::make_unique<GeometrySelectionParameter>(k_SelectedImageGeomPath_Key, "Image Geometry", "Select the Image Geometry Group from the DataStructure.", DataPath({"Image Geometry"}),
                                                             GeometrySelectionParameter::AllowedTypes{IGeometry::Type::Image}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_SelectedImageDataPath_Key, "Input Image Data Array", "The image data that will be processed by this filter.", DataPath{},
                                                          complex::ITK::GetScalarPixelAllowedTypes()));

  params.insertSeparator(Parameters::Separator{"Created Data Structure Items"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_OutputImageDataPath_Key, "Output Image Data Array", "The result of the processing will be stored in this Data Array.", DataPath{}));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ITKSpeckleNoiseImage::clone() const
{
  return std::make_unique<ITKSpeckleNoiseImage>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ITKSpeckleNoiseImage::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                             const std::atomic_bool& shouldCancel) const
{
  auto imageGeomPath = filterArgs.value<DataPath>(k_SelectedImageGeomPath_Key);
  auto selectedInputArray = filterArgs.value<DataPath>(k_SelectedImageDataPath_Key);
  auto outputArrayPath = filterArgs.value<DataPath>(k_OutputImageDataPath_Key);
  auto standardDeviation = filterArgs.value<float64>(k_StandardDeviation_Key);
  auto seed = filterArgs.value<uint32>(k_Seed_Key);

  Result<OutputActions> resultOutputActions = ITK::DataCheck<cxITKSpeckleNoiseImage::ArrayOptionsT>(dataStructure, selectedInputArray, imageGeomPath, outputArrayPath);

  return {std::move(resultOutputActions)};
}

//------------------------------------------------------------------------------
Result<> ITKSpeckleNoiseImage::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                           const std::atomic_bool& shouldCancel) const
{
  auto imageGeomPath = filterArgs.value<DataPath>(k_SelectedImageGeomPath_Key);
  auto selectedInputArray = filterArgs.value<DataPath>(k_SelectedImageDataPath_Key);
  auto outputArrayPath = filterArgs.value<DataPath>(k_OutputImageDataPath_Key);

  auto standardDeviation = filterArgs.value<float64>(k_StandardDeviation_Key);
  auto seed = filterArgs.value<uint32>(k_Seed_Key);

  cxITKSpeckleNoiseImage::ITKSpeckleNoiseImageFunctor itkFunctor = {standardDeviation, seed};

  // LINK GEOMETRY OUTPUT START
  ImageGeom& imageGeom = dataStructure.getDataRefAs<ImageGeom>(imageGeomPath);
  imageGeom.getLinkedGeometryData().addCellData(outputArrayPath);
  // LINK GEOMETRY OUTPUT STOP

  return ITK::Execute<cxITKSpeckleNoiseImage::ArrayOptionsT>(dataStructure, selectedInputArray, imageGeomPath, outputArrayPath, itkFunctor, messageHandler);
}
} // namespace complex
