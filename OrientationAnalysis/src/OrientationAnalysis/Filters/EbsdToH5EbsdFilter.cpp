#include "EbsdToH5EbsdFilter.hpp"

#include "OrientationAnalysis/Filters/Algorithms/EbsdToH5Ebsd.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/GeneratedFileListParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"

#include <filesystem>

namespace fs = std::filesystem;

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string EbsdToH5EbsdFilter::name() const
{
  return FilterTraits<EbsdToH5EbsdFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string EbsdToH5EbsdFilter::className() const
{
  return FilterTraits<EbsdToH5EbsdFilter>::className;
}

//------------------------------------------------------------------------------
Uuid EbsdToH5EbsdFilter::uuid() const
{
  return FilterTraits<EbsdToH5EbsdFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string EbsdToH5EbsdFilter::humanName() const
{
  return "Import Orientation File(s) to H5EBSD";
}

//------------------------------------------------------------------------------
std::vector<std::string> EbsdToH5EbsdFilter::defaultTags() const
{
  return {"#IO", "#Input", "#Read", "#Import", "#Ebsd", "#EDAX", "#Oxford", "#Convert"};
}

//------------------------------------------------------------------------------
Parameters EbsdToH5EbsdFilter::parameters() const
{
  Parameters params;
  params.insertSeparator(Parameters::Separator{"Conversion Parameters"});
  params.insert(std::make_unique<Float32Parameter>(k_ZSpacing_Key, "Z Spacing (Microns)", "The spacing between each slice of data", 1.0F));
  params.insert(std::make_unique<ChoicesParameter>(k_StackingOrder_Key, "Stacking Order", "The order the files should be placed into the ", EbsdToH5EbsdInputConstants::k_LowToHigh,
                                                   EbsdToH5EbsdInputConstants::k_StackingChoices));
  params.insert(std::make_unique<ChoicesParameter>(k_ReferenceFrame_Key, "Reference Frame Options", "", EbsdToH5EbsdInputConstants::k_Edax, EbsdToH5EbsdInputConstants::k_TransformChoices));

  params.insertSeparator(Parameters::Separator{"Output Parameters"});
  params.insert(std::make_unique<FileSystemPathParameter>(k_OutputPath_Key, "Output H5Ebsd File", "", fs::path(""), FileSystemPathParameter::ExtensionsType{".h5ebsd"},
                                                          FileSystemPathParameter::PathType::OutputDir, true));

  params.insertSeparator(Parameters::Separator{"Orientation Source Data"});
  params.insert(std::make_unique<GeneratedFileListParameter>(k_InputFileListInfo_Key, "Input File List", "", GeneratedFileListParameter::ValueType{}));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer EbsdToH5EbsdFilter::clone() const
{
  return std::make_unique<EbsdToH5EbsdFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult EbsdToH5EbsdFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                           const std::atomic_bool& shouldCancel) const
{
  //  auto zSpacing = filterArgs.value<Float32Parameter::ValueType>(k_ZSpacing_Key);
  //  auto stackingOrder = filterArgs.value<ChoicesParameter::ValueType>(k_StackingOrder_Key);
  //  auto referenceFrame = filterArgs.value<ChoicesParameter::ValueType>(k_ReferenceFrame_Key);
  //  auto outputPath = filterArgs.value<FileSystemPathParameter::ValueType>(k_OutputPath_Key);
  auto generatedFileListInfo = filterArgs.value<GeneratedFileListParameter::ValueType>(k_InputFileListInfo_Key);

  PreflightResult preflightResult;

  complex::Result<OutputActions> resultOutputActions;

  std::vector<PreflightValue> preflightUpdatedValues;

  if(generatedFileListInfo.fileExtension != ".ang" && generatedFileListInfo.fileExtension != ".ctf")
  {
    return {MakePreflightErrorResult(-60800, "Only .ang and .ctf files are supported")};
  }

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> EbsdToH5EbsdFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                         const std::atomic_bool& shouldCancel) const
{
  EbsdToH5EbsdInputValues inputValues;

  inputValues.ZSpacing = filterArgs.value<Float32Parameter::ValueType>(k_ZSpacing_Key);
  inputValues.StackingOrder = filterArgs.value<ChoicesParameter::ValueType>(k_StackingOrder_Key);
  inputValues.ReferenceFrame = filterArgs.value<ChoicesParameter::ValueType>(k_ReferenceFrame_Key);
  inputValues.OutputPath = filterArgs.value<FileSystemPathParameter::ValueType>(k_OutputPath_Key);
  inputValues.InputFileListInfo = filterArgs.value<GeneratedFileListParameter::ValueType>(k_InputFileListInfo_Key);
  // Grab the stacking order that the user selected and push that into the InputFileListInfo. This is an additional step
  // over the SIMPL based codes because in SIMPL the FileListInfo did not have the ordering.
  inputValues.InputFileListInfo.ordering = static_cast<FilePathGenerator::Ordering>(inputValues.StackingOrder);

  return EbsdToH5Ebsd(dataStructure, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex
