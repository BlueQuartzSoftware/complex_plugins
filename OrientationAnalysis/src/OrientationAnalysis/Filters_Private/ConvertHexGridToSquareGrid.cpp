#include "ConvertHexGridToSquareGrid.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/ConvertHexGridToSquareGridFilterParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string ConvertHexGridToSquareGrid::name() const
{
  return FilterTraits<ConvertHexGridToSquareGrid>::name.str();
}

//------------------------------------------------------------------------------
std::string ConvertHexGridToSquareGrid::className() const
{
  return FilterTraits<ConvertHexGridToSquareGrid>::className;
}

//------------------------------------------------------------------------------
Uuid ConvertHexGridToSquareGrid::uuid() const
{
  return FilterTraits<ConvertHexGridToSquareGrid>::uuid;
}

//------------------------------------------------------------------------------
std::string ConvertHexGridToSquareGrid::humanName() const
{
  return "Convert Hexagonal Grid Data to Square Grid Data (TSL - .ang)";
}

//------------------------------------------------------------------------------
std::vector<std::string> ConvertHexGridToSquareGrid::defaultTags() const
{
  return {"Sampling", "Spacing"};
}

//------------------------------------------------------------------------------
Parameters ConvertHexGridToSquareGrid::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  /*[x]*/ params.insert(std::make_unique<ConvertHexGridToSquareGridFilterParameter>(k_HexGridStack_Key, "Convert Hex Grid ANG Files", "", {}));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ConvertHexGridToSquareGrid::clone() const
{
  return std::make_unique<ConvertHexGridToSquareGrid>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ConvertHexGridToSquareGrid::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
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
  auto pHexGridStackValue = filterArgs.value<<<<NOT_IMPLEMENTED>>>>(k_HexGridStack_Key);

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
Result<> ConvertHexGridToSquareGrid::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                 const std::atomic_bool& shouldCancel) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pHexGridStackValue = filterArgs.value<<<<NOT_IMPLEMENTED>>>>(k_HexGridStack_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
