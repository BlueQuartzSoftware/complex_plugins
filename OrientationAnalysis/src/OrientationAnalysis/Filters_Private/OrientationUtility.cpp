#include "OrientationUtility.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/OrientationUtilityFilterParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string OrientationUtility::name() const
{
  return FilterTraits<OrientationUtility>::name.str();
}

//------------------------------------------------------------------------------
std::string OrientationUtility::className() const
{
  return FilterTraits<OrientationUtility>::className;
}

//------------------------------------------------------------------------------
Uuid OrientationUtility::uuid() const
{
  return FilterTraits<OrientationUtility>::uuid;
}

//------------------------------------------------------------------------------
std::string OrientationUtility::humanName() const
{
  return "Orientation Utility";
}

//------------------------------------------------------------------------------
std::vector<std::string> OrientationUtility::defaultTags() const
{
  return {"#Utilities", "#OrientationAnalysis"};
}

//------------------------------------------------------------------------------
Parameters OrientationUtility::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  /*[x]*/ params.insert(std::make_unique<OrientationUtilityFilterParameter>(k__Key, "", "", {}));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer OrientationUtility::clone() const
{
  return std::make_unique<OrientationUtility>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult OrientationUtility::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
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
  auto pValue = filterArgs.value<<<<NOT_IMPLEMENTED>>>>(k__Key);

  PreflightResult preflightResult;

  complex::Result<OutputActions> resultOutputActions;

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
Result<> OrientationUtility::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                         const std::atomic_bool& shouldCancel) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pValue = filterArgs.value<<<<NOT_IMPLEMENTED>>>>(k__Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
