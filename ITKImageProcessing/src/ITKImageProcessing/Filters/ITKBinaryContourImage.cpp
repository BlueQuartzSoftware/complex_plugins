#include "ITKBinaryContourImage.hpp"

#include "ITKImageProcessing/Common/ITKArrayHelper.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"

#include <itkBinaryContourImageFilter.h>

using namespace complex;

namespace cxITKBinaryContourImage
{
using ArrayOptionsT = ITK::IntegerScalarPixelIdTypeList;

struct ITKBinaryContourImageFunctor
{
  bool fullyConnected = false;
  float64 backgroundValue = 0.0;
  float64 foregroundValue = 1.0;

  template <class InputImageT, class OutputImageT, uint32 Dimension>
  auto createFilter() const
  {
    using FilterT = itk::BinaryContourImageFilter<InputImageT, OutputImageT>;
    auto filter = FilterT::New();
    filter->SetFullyConnected(fullyConnected);
    filter->SetBackgroundValue(backgroundValue);
    filter->SetForegroundValue(foregroundValue);
    return filter;
  }
};
} // namespace cxITKBinaryContourImage

namespace complex
{
//------------------------------------------------------------------------------
std::string ITKBinaryContourImage::name() const
{
  return FilterTraits<ITKBinaryContourImage>::name;
}

//------------------------------------------------------------------------------
std::string ITKBinaryContourImage::className() const
{
  return FilterTraits<ITKBinaryContourImage>::className;
}

//------------------------------------------------------------------------------
Uuid ITKBinaryContourImage::uuid() const
{
  return FilterTraits<ITKBinaryContourImage>::uuid;
}

//------------------------------------------------------------------------------
std::string ITKBinaryContourImage::humanName() const
{
  return "ITK Binary Contour Image Filter";
}

//------------------------------------------------------------------------------
std::vector<std::string> ITKBinaryContourImage::defaultTags() const
{
  return {"ITKImageProcessing", "ITKBinaryContourImage", "ITKImageLabel", "ImageLabel"};
}

//------------------------------------------------------------------------------
Parameters ITKBinaryContourImage::parameters() const
{
  Parameters params;

  params.insertSeparator(Parameters::Separator{"Filter Parameters"});
  params.insert(std::make_unique<BoolParameter>(
      k_FullyConnected_Key, "FullyConnected",
      "Whether the connected components are defined strictly by face connectivity or by face+edge+vertex connectivity. For objects that are 1 pixel wide, use FullyConnectedOn.", false));
  params.insert(std::make_unique<Float64Parameter>(k_BackgroundValue_Key, "BackgroundValue", "Set/Get the background value used to mark the pixels not on the border of the objects.", 0.0));
  params.insert(std::make_unique<Float64Parameter>(k_ForegroundValue_Key, "ForegroundValue", "Set/Get the foreground value used to identify the objects in the input and output images.", 1.0));

  params.insertSeparator(Parameters::Separator{"Input Data Structure Items"});
  params.insert(std::make_unique<GeometrySelectionParameter>(k_SelectedImageGeomPath_Key, "Image Geometry", "Select the Image Geometry Group from the DataStructure.", DataPath({"Image Geometry"}),
                                                             GeometrySelectionParameter::AllowedTypes{IGeometry::Type::Image}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_SelectedImageDataPath_Key, "Input Image Data Array", "The image data that will be processed by this filter.", DataPath{},
                                                          complex::ITK::GetIntegerScalarPixelAllowedTypes()));

  params.insertSeparator(Parameters::Separator{"Created Data Structure Items"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_OutputImageDataPath_Key, "Output Image Data Array", "The result of the processing will be stored in this Data Array.", DataPath{}));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ITKBinaryContourImage::clone() const
{
  return std::make_unique<ITKBinaryContourImage>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ITKBinaryContourImage::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                              const std::atomic_bool& shouldCancel) const
{
  auto imageGeomPath = filterArgs.value<DataPath>(k_SelectedImageGeomPath_Key);
  auto selectedInputArray = filterArgs.value<DataPath>(k_SelectedImageDataPath_Key);
  auto outputArrayPath = filterArgs.value<DataPath>(k_OutputImageDataPath_Key);
  auto fullyConnected = filterArgs.value<bool>(k_FullyConnected_Key);
  auto backgroundValue = filterArgs.value<float64>(k_BackgroundValue_Key);
  auto foregroundValue = filterArgs.value<float64>(k_ForegroundValue_Key);

  Result<OutputActions> resultOutputActions = ITK::DataCheck<cxITKBinaryContourImage::ArrayOptionsT>(dataStructure, selectedInputArray, imageGeomPath, outputArrayPath);

  return {std::move(resultOutputActions)};
}

//------------------------------------------------------------------------------
Result<> ITKBinaryContourImage::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                            const std::atomic_bool& shouldCancel) const
{
  auto imageGeomPath = filterArgs.value<DataPath>(k_SelectedImageGeomPath_Key);
  auto selectedInputArray = filterArgs.value<DataPath>(k_SelectedImageDataPath_Key);
  auto outputArrayPath = filterArgs.value<DataPath>(k_OutputImageDataPath_Key);

  auto fullyConnected = filterArgs.value<bool>(k_FullyConnected_Key);
  auto backgroundValue = filterArgs.value<float64>(k_BackgroundValue_Key);
  auto foregroundValue = filterArgs.value<float64>(k_ForegroundValue_Key);

  cxITKBinaryContourImage::ITKBinaryContourImageFunctor itkFunctor = {fullyConnected, backgroundValue, foregroundValue};

  // LINK GEOMETRY OUTPUT START
  ImageGeom& imageGeom = dataStructure.getDataRefAs<ImageGeom>(imageGeomPath);
  imageGeom.getLinkedGeometryData().addCellData(outputArrayPath);
  // LINK GEOMETRY OUTPUT STOP

  return ITK::Execute<cxITKBinaryContourImage::ArrayOptionsT>(dataStructure, selectedInputArray, imageGeomPath, outputArrayPath, itkFunctor, shouldCancel);
}
} // namespace complex
