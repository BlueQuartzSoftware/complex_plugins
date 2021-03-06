#include "CombineAttributeArrays.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/MultiArraySelectionParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string CombineAttributeArrays::name() const
{
  return FilterTraits<CombineAttributeArrays>::name.str();
}

//------------------------------------------------------------------------------
std::string CombineAttributeArrays::className() const
{
  return FilterTraits<CombineAttributeArrays>::className;
}

//------------------------------------------------------------------------------
Uuid CombineAttributeArrays::uuid() const
{
  return FilterTraits<CombineAttributeArrays>::uuid;
}

//------------------------------------------------------------------------------
std::string CombineAttributeArrays::humanName() const
{
  return "Combine Attribute Arrays";
}

//------------------------------------------------------------------------------
std::vector<std::string> CombineAttributeArrays::defaultTags() const
{
  return {"#Core", "#Memory Management"};
}

//------------------------------------------------------------------------------
Parameters CombineAttributeArrays::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<BoolParameter>(k_NormalizeData_Key, "Normalize Data", "", false));
  params.insert(std::make_unique<BoolParameter>(k_MoveValues_Key, "Move Data", "", false));
  params.insert(std::make_unique<MultiArraySelectionParameter>(k_SelectedDataArrayPaths_Key, "Attribute Arrays to Combine", "",
                                                               MultiArraySelectionParameter::ValueType{DataPath(), DataPath(), DataPath()}, MultiArraySelectionParameter::AllowedTypes{}));
  params.insert(std::make_unique<StringParameter>(k_StackedDataArrayName_Key, "Combined Data", "", "SomeString"));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer CombineAttributeArrays::clone() const
{
  return std::make_unique<CombineAttributeArrays>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult CombineAttributeArrays::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
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
  auto pNormalizeDataValue = filterArgs.value<bool>(k_NormalizeData_Key);
  auto pMoveValuesValue = filterArgs.value<bool>(k_MoveValues_Key);
  auto pSelectedDataArrayPathsValue = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_SelectedDataArrayPaths_Key);
  auto pStackedDataArrayNameValue = filterArgs.value<StringParameter::ValueType>(k_StackedDataArrayName_Key);

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
Result<> CombineAttributeArrays::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                             const std::atomic_bool& shouldCancel) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pNormalizeDataValue = filterArgs.value<bool>(k_NormalizeData_Key);
  auto pMoveValuesValue = filterArgs.value<bool>(k_MoveValues_Key);
  auto pSelectedDataArrayPathsValue = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_SelectedDataArrayPaths_Key);
  auto pStackedDataArrayNameValue = filterArgs.value<StringParameter::ValueType>(k_StackedDataArrayName_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
