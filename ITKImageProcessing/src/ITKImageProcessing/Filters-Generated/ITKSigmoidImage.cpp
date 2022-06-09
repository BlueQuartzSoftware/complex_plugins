#include "ITKSigmoidImage.hpp"

#include "ITKImageProcessing/Common/ITKArrayHelper.hpp"
#include "ITKImageProcessing/Common/sitkCommon.hpp"

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"

#include <itkSigmoidImageFilter.h>

using namespace complex;

namespace cxITKSigmoidImage
{
using ArrayOptionsT = ITK::ScalarPixelIdTypeList;

struct ITKSigmoidImageFunctor
{
  float64 alpha = 1;
  float64 beta = 0;
  float64 outputMaximum = 255;
  float64 outputMinimum = 0;

  template <class InputImageT, class OutputImageT, uint32 Dimension>
  auto createFilter() const
  {
    using FilterT = itk::SigmoidImageFilter<InputImageT, OutputImageT>;
    auto filter = FilterT::New();
    filter->SetAlpha(alpha);
    filter->SetBeta(beta);
    filter->SetOutputMaximum(outputMaximum);
    filter->SetOutputMinimum(outputMinimum);
    return filter;
  }
};
} // namespace cxITKSigmoidImage

namespace complex
{
//------------------------------------------------------------------------------
std::string ITKSigmoidImage::name() const
{
  return FilterTraits<ITKSigmoidImage>::name;
}

//------------------------------------------------------------------------------
std::string ITKSigmoidImage::className() const
{
  return FilterTraits<ITKSigmoidImage>::className;
}

//------------------------------------------------------------------------------
Uuid ITKSigmoidImage::uuid() const
{
  return FilterTraits<ITKSigmoidImage>::uuid;
}

//------------------------------------------------------------------------------
std::string ITKSigmoidImage::humanName() const
{
  return "ITK Sigmoid Image Filter";
}

//------------------------------------------------------------------------------
std::vector<std::string> ITKSigmoidImage::defaultTags() const
{
  return {"ITKImageProcessing", "ITKSigmoidImage", "ITKImageIntensity", "ImageIntensity"};
}

//------------------------------------------------------------------------------
Parameters ITKSigmoidImage::parameters() const
{
  Parameters params;
  params.insertSeparator(Parameters::Separator{"Filter Parameters"});
  params.insert(std::make_unique<Float64Parameter>(k_Alpha_Key, "Alpha", "", 1));
  params.insert(std::make_unique<Float64Parameter>(k_Beta_Key, "Beta", "", 0));
  params.insert(std::make_unique<Float64Parameter>(k_OutputMaximum_Key, "OutputMaximum", "", 255));
  params.insert(std::make_unique<Float64Parameter>(k_OutputMinimum_Key, "OutputMinimum", "", 0));

  params.insertSeparator(Parameters::Separator{"Input Data Structure Items"});
  params.insert(std::make_unique<GeometrySelectionParameter>(k_SelectedImageGeomPath_Key, "Image Geometry", "Select the Image Geometry Group from the DataStructure.", DataPath({"Image Geometry"}),
                                                             GeometrySelectionParameter::AllowedTypes{AbstractGeometry::Type::Image}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_SelectedImageDataPath_Key, "Input Image Data Array", "The image data that will be processed by this filter.", DataPath{},
                                                          complex::ITK::GetScalarPixelAllowedTypes()));

  params.insertSeparator(Parameters::Separator{"Created Data Structure Items"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_OutputImageDataPath_Key, "Output Image Data Array", "The result of the processing will be stored in this Data Array.", DataPath{}));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ITKSigmoidImage::clone() const
{
  return std::make_unique<ITKSigmoidImage>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ITKSigmoidImage::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                        const std::atomic_bool& shouldCancel) const
{
  auto imageGeomPath = filterArgs.value<DataPath>(k_SelectedImageGeomPath_Key);
  auto selectedInputArray = filterArgs.value<DataPath>(k_SelectedImageDataPath_Key);
  auto outputArrayPath = filterArgs.value<DataPath>(k_OutputImageDataPath_Key);
  auto alpha = filterArgs.value<float64>(k_Alpha_Key);
  auto beta = filterArgs.value<float64>(k_Beta_Key);
  auto outputMaximum = filterArgs.value<float64>(k_OutputMaximum_Key);
  auto outputMinimum = filterArgs.value<float64>(k_OutputMinimum_Key);

  Result<OutputActions> resultOutputActions = ITK::DataCheck<cxITKSigmoidImage::ArrayOptionsT>(dataStructure, selectedInputArray, imageGeomPath, outputArrayPath);

  return {std::move(resultOutputActions)};
}

//------------------------------------------------------------------------------
Result<> ITKSigmoidImage::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                      const std::atomic_bool& shouldCancel) const
{
  auto imageGeomPath = filterArgs.value<DataPath>(k_SelectedImageGeomPath_Key);
  auto selectedInputArray = filterArgs.value<DataPath>(k_SelectedImageDataPath_Key);
  auto outputArrayPath = filterArgs.value<DataPath>(k_OutputImageDataPath_Key);

  auto alpha = filterArgs.value<float64>(k_Alpha_Key);
  auto beta = filterArgs.value<float64>(k_Beta_Key);
  auto outputMaximum = filterArgs.value<float64>(k_OutputMaximum_Key);
  auto outputMinimum = filterArgs.value<float64>(k_OutputMinimum_Key);

  cxITKSigmoidImage::ITKSigmoidImageFunctor itkFunctor = {alpha, beta, outputMaximum, outputMinimum};

  // LINK GEOMETRY OUTPUT START
  ImageGeom& imageGeom = dataStructure.getDataRefAs<ImageGeom>(imageGeomPath);
  imageGeom.getLinkedGeometryData().addCellData(outputArrayPath);
  // LINK GEOMETRY OUTPUT STOP

  return ITK::Execute<cxITKSigmoidImage::ArrayOptionsT>(dataStructure, selectedInputArray, imageGeomPath, outputArrayPath, itkFunctor);
}
} // namespace complex
