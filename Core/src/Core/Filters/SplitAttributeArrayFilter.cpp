#include "SplitAttributeArrayFilter.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/IDataArray.hpp"
#include "complex/Filter/Actions/CreateArrayAction.hpp"
#include "complex/Filter/Actions/DeleteDataAction.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/DataObjectNameParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"
#include "complex/Utilities/StringUtilities.hpp"

#include "Core/Filters/Algorithms/SplitAttributeArray.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string SplitAttributeArrayFilter::name() const
{
  return FilterTraits<SplitAttributeArrayFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string SplitAttributeArrayFilter::className() const
{
  return FilterTraits<SplitAttributeArrayFilter>::className;
}

//------------------------------------------------------------------------------
Uuid SplitAttributeArrayFilter::uuid() const
{
  return FilterTraits<SplitAttributeArrayFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string SplitAttributeArrayFilter::humanName() const
{
  return "Split Multicomponent Attribute Array";
}

//------------------------------------------------------------------------------
std::vector<std::string> SplitAttributeArrayFilter::defaultTags() const
{
  return {"#Core", "#Split"};
}

//------------------------------------------------------------------------------
Parameters SplitAttributeArrayFilter::parameters() const
{
  Parameters params;

  params.insertSeparator(Parameters::Separator{"Input Parameters"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_MultiCompArray_Key, "Multicomponent Attribute Array", "", DataPath{}, GetAllDataTypes()));
  params.insert(std::make_unique<StringParameter>(k_Postfix_Key, "Postfix", "", "_Component"));
  params.insert(std::make_unique<BoolParameter>(k_DeleteOriginal_Key, "Remove Original Array", "", false));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer SplitAttributeArrayFilter::clone() const
{
  return std::make_unique<SplitAttributeArrayFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult SplitAttributeArrayFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                  const std::atomic_bool& shouldCancel) const
{
  auto pInputArrayPath = filterArgs.value<ArraySelectionParameter::ValueType>(k_MultiCompArray_Key);
  auto pPostfix = filterArgs.value<std::string>(k_Postfix_Key);
  auto pRemoveOriginal = filterArgs.value<bool>(k_DeleteOriginal_Key);

  PreflightResult preflightResult;
  complex::Result<OutputActions> resultOutputActions;
  std::vector<PreflightValue> preflightUpdatedValues;

  auto* inputArray = dataStructure.getDataAs<IDataArray>(pInputArrayPath);
  if(inputArray == nullptr)
  {
    return {nonstd::make_unexpected(std::vector<Error>{Error{-65400, fmt::format("Cannot find input data array at path '{}'", pInputArrayPath.toString())}})};
  }
  usize numComponents = inputArray->getNumberOfComponents();
  if(numComponents <= 1)
  {
    return {nonstd::make_unexpected(std::vector<Error>{Error{-65401, fmt::format("Selected Array '{}' must have more than 1 component", pInputArrayPath.toString())}})};
  }

  std::vector<usize> tdims = inputArray->getIDataStoreRef().getTupleShape();
  std::vector<usize> cdims(1, 1);
  for(usize i = 0; i < numComponents; i++)
  {
    std::string arrayName = pInputArrayPath.getTargetName() + pPostfix + StringUtilities::number(i);
    DataPath newArrayPath = pInputArrayPath.getParent().createChildPath(arrayName);
    resultOutputActions.value().actions.push_back(std::make_unique<CreateArrayAction>(inputArray->getDataType(), tdims, cdims, newArrayPath));
  }

  if(pRemoveOriginal)
  {
    resultOutputActions.value().deferredActions.push_back(std::make_unique<DeleteDataAction>(pInputArrayPath));
  }

  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> SplitAttributeArrayFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                const std::atomic_bool& shouldCancel) const
{
  SplitAttributeArrayInputValues inputValues;
  inputValues.InputArrayPath = filterArgs.value<ArraySelectionParameter::ValueType>(k_MultiCompArray_Key);
  inputValues.SplitArraysSuffix = filterArgs.value<std::string>(k_Postfix_Key);

  return SplitAttributeArray(dataStructure, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex
