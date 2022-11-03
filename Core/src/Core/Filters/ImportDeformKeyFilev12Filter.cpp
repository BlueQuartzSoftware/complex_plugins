#include "ImportDeformKeyFilev12Filter.hpp"

#include "Core/Filters/Algorithms/ImportDeformKeyFilev12.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/EmptyAction.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/DataObjectNameParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string ImportDeformKeyFilev12Filter::name() const
{
  return FilterTraits<ImportDeformKeyFilev12Filter>::name.str();
}

//------------------------------------------------------------------------------
std::string ImportDeformKeyFilev12Filter::className() const
{
  return FilterTraits<ImportDeformKeyFilev12Filter>::className;
}

//------------------------------------------------------------------------------
Uuid ImportDeformKeyFilev12Filter::uuid() const
{
  return FilterTraits<ImportDeformKeyFilev12Filter>::uuid;
}

//------------------------------------------------------------------------------
std::string ImportDeformKeyFilev12Filter::humanName() const
{
  return "Import Deform Key File (v12)";
}

//------------------------------------------------------------------------------
std::vector<std::string> ImportDeformKeyFilev12Filter::defaultTags() const
{
  return {"#Unsupported", "#SimulationIO"};
}

//------------------------------------------------------------------------------
Parameters ImportDeformKeyFilev12Filter::parameters() const
{
  Parameters params;

  // Create the parameter descriptors that are needed for this filter
  params.insertSeparator(Parameters::Separator{"Input Parameters"});
  params.insert(std::make_unique<FileSystemPathParameter>(k_DEFORMInputFile_Key, "Input File", "The input DEFORM V12 file path", fs::path(), FileSystemPathParameter::ExtensionsType{"*.key"},
                                                          FileSystemPathParameter::PathType::InputFile));
  params.insert(std::make_unique<BoolParameter>(k_VerboseOutput_Key, "Verbose Output", "", false));
  params.insertSeparator(Parameters::Separator{"Created Data Objects"});
  params.insert(std::make_unique<DataGroupCreationParameter>(k_DataContainerName_Key, "Created Quad Geometry", "", DataPath({"DEFORM Geometry"})));
  params.insert(std::make_unique<DataObjectNameParameter>(k_VertexAttributeMatrixName_Key, "Created Vertex Attribute Matrix Name", "", "VertexData"));
  params.insert(std::make_unique<DataObjectNameParameter>(k_CellAttributeMatrixName_Key, "Created Face Attribute Matrix Name", "", "FaceData"));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ImportDeformKeyFilev12Filter::clone() const
{
  return std::make_unique<ImportDeformKeyFilev12Filter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ImportDeformKeyFilev12Filter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                     const std::atomic_bool& shouldCancel) const
{
  /****************************************************************************
   * Write any preflight sanity checking codes in this function
   ***************************************************************************/

  /**
   * These are the values that were gathered from the UI or the pipeline file or
   * otherwise passed into the filter. These are here for your convenience. If you
   * do not need some of them remove them.
   */
  auto pDEFORMInputFileValue = filterArgs.value<FileSystemPathParameter::ValueType>(k_DEFORMInputFile_Key);
  auto pVerboseOutputValue = filterArgs.value<bool>(k_VerboseOutput_Key);
  auto pDataContainerNameValue = filterArgs.value<StringParameter::ValueType>(k_DataContainerName_Key);
  auto pVertexAttributeMatrixNameValue = filterArgs.value<StringParameter::ValueType>(k_VertexAttributeMatrixName_Key);
  auto pCellAttributeMatrixNameValue = filterArgs.value<StringParameter::ValueType>(k_CellAttributeMatrixName_Key);

  // Declare the preflightResult variable that will be populated with the results
  // of the preflight. The PreflightResult type contains the output Actions and
  // any preflight updated values that you want to be displayed to the user, typically
  // through a user interface (UI).
  PreflightResult preflightResult;

  // If your filter is making structural changes to the DataStructure then the filter
  // is going to create OutputActions subclasses that need to be returned. This will
  // store those actions.
  complex::Result<OutputActions> resultOutputActions;

  // If your filter is going to pass back some `preflight updated values` then this is where you
  // would create the code to store those values in the appropriate object. Note that we
  // in line creating the pair (NOT a std::pair<>) of Key:Value that will get stored in
  // the std::vector<PreflightValue> object.
  std::vector<PreflightValue> preflightUpdatedValues;

  // If the filter needs to pass back some updated values via a key:value string:string set of values
  // you can declare and update that string here.
  // None found in this filter based on the filter parameters

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
  // None found based on the filter parameters

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> ImportDeformKeyFilev12Filter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                   const std::atomic_bool& shouldCancel) const
{

  ImportDeformKeyFilev12InputValues inputValues;

  inputValues.DEFORMInputFile = filterArgs.value<FileSystemPathParameter::ValueType>(k_DEFORMInputFile_Key);
  inputValues.VerboseOutput = filterArgs.value<bool>(k_VerboseOutput_Key);
  inputValues.DataContainerName = filterArgs.value<StringParameter::ValueType>(k_DataContainerName_Key);
  inputValues.VertexAttributeMatrixName = filterArgs.value<StringParameter::ValueType>(k_VertexAttributeMatrixName_Key);
  inputValues.CellAttributeMatrixName = filterArgs.value<StringParameter::ValueType>(k_CellAttributeMatrixName_Key);

  return ImportDeformKeyFilev12(dataStructure, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex
