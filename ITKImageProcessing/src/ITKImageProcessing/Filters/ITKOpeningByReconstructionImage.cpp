#include "ITKOpeningByReconstructionImage.hpp"

// This filter only works with certain kinds of data so we
// disable the types that the filter will *NOT* compile against. The
// Allowed PixelTypes as defined in SimpleITK is: BasicPixelIDTypeList
#define COMPLEX_ITK_ARRAY_HELPER_USE_uint64 0
#define COMPLEX_ITK_ARRAY_HELPER_USE_int64 0

#include "ITKImageProcessing/Common/ITKArrayHelper.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

#include <itkOpeningByReconstructionImageFilter.h>

using namespace complex;

namespace
{
struct ITKOpeningByReconstructionImageCreationFunctor
{
  std::vector<uint32_t> pKernelRadius;
  itk::simple::KernelEnum pKernelType;
  bool pFullyConnected;
  bool pPreserveIntensities;

  template <typename InputImageType, typename OutputImageType, unsigned int Dimension>
  auto operator()() const
  {
    using FilterType = itk::OpeningByReconstructionImageFilter<InputImageType, OutputImageType, itk::FlatStructuringElement<InputImageType::ImageDimension>>;
    typename FilterType::Pointer filter = FilterType::New();
    auto kernel = itk::simple::CreateKernel<Dimension>(static_cast<itk::simple::KernelEnum>(pKernelType), pKernelRadius);
    filter->SetKernel(kernel);
    filter->SetFullyConnected(pFullyConnected);
    filter->SetPreserveIntensities(pPreserveIntensities);
    return filter;
  }
};
} // namespace

namespace complex
{
//------------------------------------------------------------------------------
std::string ITKOpeningByReconstructionImage::name() const
{
  return FilterTraits<ITKOpeningByReconstructionImage>::name.str();
}

//------------------------------------------------------------------------------
std::string ITKOpeningByReconstructionImage::className() const
{
  return FilterTraits<ITKOpeningByReconstructionImage>::className;
}

//------------------------------------------------------------------------------
Uuid ITKOpeningByReconstructionImage::uuid() const
{
  return FilterTraits<ITKOpeningByReconstructionImage>::uuid;
}

//------------------------------------------------------------------------------
std::string ITKOpeningByReconstructionImage::humanName() const
{
  return "ITK::OpeningByReconstructionImageFilter";
}

//------------------------------------------------------------------------------
std::vector<std::string> ITKOpeningByReconstructionImage::defaultTags() const
{
  return {"ITKImageProcessing", "ITKOpeningByReconstructionImage"};
}

//------------------------------------------------------------------------------
Parameters ITKOpeningByReconstructionImage::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<GeometrySelectionParameter>(k_SelectedImageGeomPath_Key, "Image Geometry", "", DataPath{}, GeometrySelectionParameter::AllowedTypes{DataObject::Type::ImageGeom}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_SelectedImageDataPath_Key, "Input Image", "", DataPath{}));
  params.insert(std::make_unique<ArrayCreationParameter>(k_OutputIamgeDataPath_Key, "Output Image", "", DataPath{}));
  params.insert(std::make_unique<VectorUInt32Parameter>(k_KernelRadius_Key, "KernelRadius", "", std::vector<uint32_t>(3), std::vector<std::string>(3)));
  params.insert(std::make_unique<ChoicesParameter>(k_KernelType_Key, "Kernel Type", "", itk::simple::sitkBall, ChoicesParameter::Choices{"Annulus", "Ball", "Box", "Cross"}));
  params.insert(std::make_unique<BoolParameter>(k_FullyConnected_Key, "FullyConnected", "", false));
  params.insert(std::make_unique<BoolParameter>(k_PreserveIntensities_Key, "PreserveIntensities", "", false));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ITKOpeningByReconstructionImage::clone() const
{
  return std::make_unique<ITKOpeningByReconstructionImage>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ITKOpeningByReconstructionImage::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Write any preflight sanity checking codes in this function
   ***************************************************************************/

  /**
   * These are the values that were gathered from the UI or the pipeline file or
   * otherwise passed into the filter. These are here for your convenience. If you
   * do not need some of them remove them.
   */
  auto pImageGeomPath = filterArgs.value<DataPath>(k_SelectedImageGeomPath_Key);
  auto pSelectedInputArray = filterArgs.value<DataPath>(k_SelectedImageDataPath_Key);
  auto pOutputArrayPath = filterArgs.value<DataPath>(k_OutputIamgeDataPath_Key);
  auto pKernelRadius = filterArgs.value<VectorUInt32Parameter::ValueType>(k_KernelRadius_Key);
  auto pKernelType = filterArgs.value<itk::simple::KernelEnum>(k_KernelType_Key);
  auto pFullyConnected = filterArgs.value<bool>(k_FullyConnected_Key);
  auto pPreserveIntensities = filterArgs.value<bool>(k_PreserveIntensities_Key);

  // Declare the preflightResult variable that will be populated with the results
  // of the preflight. The PreflightResult type contains the output Actions and
  // any preflight updated values that you want to be displayed to the user, typically
  // through a user interface (UI).
  PreflightResult preflightResult;
  // If your filter is going to pass back some `preflight updated values` then this is where you
  // would create the code to store those values in the appropriate object. Note that we
  // in line creating the pair (NOT a std::pair<>) of Key:Value that will get stored in
  // the std::vector<PreflightValue> object.
  std::vector<PreflightValue> preflightUpdatedValues;

  // If your filter is making structural changes to the DataStructure then the filter
  // is going to create OutputActions subclasses that need to be returned. This will
  // store those actions.
  complex::Result<OutputActions> resultOutputActions;

  resultOutputActions = ITK::DataCheck(dataStructure, pSelectedInputArray, pImageGeomPath, pOutputArrayPath);

  // If the filter needs to pass back some updated values via a key:value string:string set of values
  // you can declare and update that string here.

  // If this filter makes changes to the DataStructure in the form of
  // creating/deleting/moving/renaming DataGroups, Geometries, DataArrays then you
  // will need to use one of the `*Actions` classes located in complex/Filter/Actions
  // to relay that information to the preflight and execute methods. This is done by
  // creating an instance of the Action class and then storing it in the resultOutputActions variable.
  // This is done through a `push_back()` method combined with a `std::move()`. For the
  // newly initiated to `std::move` once that code is executed what was once inside the Action class
  // instance variable is *no longer there*. The memory has been moved. If you try to access that
  // variable after this line you will probably get a crash or have subtle bugs. To ensure that this
  // does not happen we suggest using braces `{}` to scope each of the action's declaration and store
  // so that the programmer is not tempted to use the action instance past where it should be used.
  // You have to create your own Actions class if there isn't something specific for your filter's needs

  // Store the preflight updated value(s) into the preflightUpdatedValues vector using
  // the appropriate methods.

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> ITKOpeningByReconstructionImage::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pImageGeomPath = filterArgs.value<DataPath>(k_SelectedImageGeomPath_Key);
  auto pSelectedInputArray = filterArgs.value<DataPath>(k_SelectedImageDataPath_Key);
  auto pOutputArrayPath = filterArgs.value<DataPath>(k_OutputIamgeDataPath_Key);
  auto pKernelRadius = filterArgs.value<VectorUInt32Parameter::ValueType>(k_KernelRadius_Key);
  auto pKernelType = filterArgs.value<itk::simple::KernelEnum>(k_KernelType_Key);
  auto pFullyConnected = filterArgs.value<bool>(k_FullyConnected_Key);
  auto pPreserveIntensities = filterArgs.value<bool>(k_PreserveIntensities_Key);

  /****************************************************************************
   * Create the functor object that will instantiate the correct itk filter
   ***************************************************************************/
  ::ITKOpeningByReconstructionImageCreationFunctor itkFunctor{};
  itkFunctor.pKernelRadius = pKernelRadius;
  itkFunctor.pKernelType = pKernelType;
  itkFunctor.pFullyConnected = pFullyConnected;
  itkFunctor.pPreserveIntensities = pPreserveIntensities;

  /****************************************************************************
   * Associate the output image with the Image Geometry for Visualization
   ***************************************************************************/
  ImageGeom& imageGeom = dataStructure.getDataRefAs<ImageGeom>(pImageGeomPath);
  imageGeom.getLinkedGeometryData().addCellData(pOutputArrayPath);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/
  return ITK::Execute(dataStructure, pSelectedInputArray, pImageGeomPath, pOutputArrayPath, itkFunctor);
}
} // namespace complex
