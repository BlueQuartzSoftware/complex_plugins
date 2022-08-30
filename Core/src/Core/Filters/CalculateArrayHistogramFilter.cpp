#include "CalculateArrayHistogramFilter.hpp"
#include "Algorithms/CalculateArrayHistogram.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/IDataArray.hpp"
#include "complex/Filter/Actions/CreateArrayAction.hpp"
#include "complex/Filter/Actions/CreateDataGroupAction.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/DataGroupSelectionParameter.hpp"
#include "complex/Parameters/MultiArraySelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string CalculateArrayHistogramFilter::name() const
{
  return FilterTraits<CalculateArrayHistogramFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string CalculateArrayHistogramFilter::className() const
{
  return FilterTraits<CalculateArrayHistogramFilter>::className;
}

//------------------------------------------------------------------------------
Uuid CalculateArrayHistogramFilter::uuid() const
{
  return FilterTraits<CalculateArrayHistogramFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string CalculateArrayHistogramFilter::humanName() const
{
  return "Calculate Frequency Histogram";
}

//------------------------------------------------------------------------------
std::vector<std::string> CalculateArrayHistogramFilter::defaultTags() const
{
  return {"#Statistics", "#Ensemble"};
}

//------------------------------------------------------------------------------
Parameters CalculateArrayHistogramFilter::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<Int32Parameter>(k_NumberOfBins_Key, "Number of Bins", "", 1));
  params.insertLinkableParameter(std::make_unique<BoolParameter>(k_UserDefinedRange_Key, "Use Custom Min & Max Range", "", false));
  params.insert(std::make_unique<Float64Parameter>(k_MinRange_Key, "Min Value", "", 2.3456789));
  params.insert(std::make_unique<Float64Parameter>(k_MaxRange_Key, "Max Value", "", 2.3456789));
  params.insertLinkableParameter(std::make_unique<BoolParameter>(k_NewDataGroup_Key, "Create New DataGroup for Histograms", "", true));
  params.insert(std::make_unique<MultiArraySelectionParameter>(k_SelectedArrayPaths_Key, "DataArray(s) to Histogram", "", MultiArraySelectionParameter::ValueType{}, complex::GetAllNumericTypes()));
  params.insert(std::make_unique<DataGroupCreationParameter>(k_NewDataGroupName_Key, "New DataGroup Path", "", DataPath{}));
  params.insert(std::make_unique<DataGroupSelectionParameter>(k_DataGroupName_Key, "Output DataGroup Path", "", DataPath{}));
  // Associate the Linkable Parameter(s) to the children parameters that they control
  params.linkParameters(k_UserDefinedRange_Key, k_MinRange_Key, true);
  params.linkParameters(k_UserDefinedRange_Key, k_MaxRange_Key, true);
  params.linkParameters(k_NewDataGroup_Key, k_NewDataGroupName_Key, true);
  params.linkParameters(k_NewDataGroup_Key, k_DataGroupName_Key, false);

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer CalculateArrayHistogramFilter::clone() const
{
  return std::make_unique<CalculateArrayHistogramFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult CalculateArrayHistogramFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                      const std::atomic_bool& shouldCancel) const
{
  auto pNumberOfBinsValue = filterArgs.value<int32>(k_NumberOfBins_Key);
  auto pUserDefinedRangeValue = filterArgs.value<bool>(k_UserDefinedRange_Key); // verify and calculate range values here if false
  auto pMinRangeValue = filterArgs.value<float64>(k_MinRange_Key);
  auto pMaxRangeValue = filterArgs.value<float64>(k_MaxRange_Key);
  auto pNewDataGroupValue = filterArgs.value<bool>(k_NewDataGroup_Key);
  auto pDataGroupNameValue = filterArgs.value<DataPath>(k_DataGroupName_Key);
  auto pSelectedArrayPathsValue = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_SelectedArrayPaths_Key);
  auto pNewDataGroupNameValue = filterArgs.value<DataPath>(k_NewDataGroupName_Key); // sanity check if is Attribute matrix after impending complex update

  PreflightResult preflightResult;

  complex::Result<OutputActions> resultOutputActions;

  std::vector<PreflightValue> preflightUpdatedValues;
  if(pNewDataGroupValue)
  {
    auto createDataGroupAction = std::make_unique<CreateDataGroupAction>(pNewDataGroupNameValue);
    resultOutputActions.value().actions.push_back(std::move(createDataGroupAction));
  }
  for(auto& selectedArrayPath : pSelectedArrayPathsValue)
  {
    const auto* dataArray = dataStructure.getDataAs<IDataArray>(selectedArrayPath);
    if(dataArray == nullptr)
    {
      return {MakeErrorResult<OutputActions>(-19570, fmt::format("DataArray {} does not exist!", selectedArrayPath.toString()))};
    }
    DataPath childPath;
    if(pNewDataGroupValue)
    {
      childPath = pNewDataGroupNameValue.createChildPath((dataArray->getName() + "Histogram"));
    }
    else
    {
      childPath = pDataGroupNameValue.createChildPath((dataArray->getName() + "Histogram"));
    }
    auto createArrayAction = std::make_unique<CreateArrayAction>(complex::DataType::float32, std::vector<usize>{static_cast<usize>(pNumberOfBinsValue)}, std::vector<usize>{1}, childPath);
    resultOutputActions.value().actions.push_back(std::move(createArrayAction));
  }

  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> CalculateArrayHistogramFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                    const std::atomic_bool& shouldCancel) const
{
  CalculateArrayHistogramInputValues inputValues;

  inputValues.NumberOfBins = filterArgs.value<int32>(k_NumberOfBins_Key);
  inputValues.UserDefinedRange = filterArgs.value<bool>(k_UserDefinedRange_Key);
  inputValues.MinRange = filterArgs.value<float64>(k_MinRange_Key);
  inputValues.MaxRange = filterArgs.value<float64>(k_MaxRange_Key);
  inputValues.SelectedArrayPaths = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_SelectedArrayPaths_Key);

  DataPath& dataGroupPath = *std::move(DataPath::FromString("temp"));
  if(filterArgs.value<bool>(k_NewDataGroup_Key))
  {
    dataGroupPath = filterArgs.value<DataPath>(k_NewDataGroupName_Key);
  }
  else
  {
    dataGroupPath = filterArgs.value<DataPath>(k_DataGroupName_Key);
  }

  MultiArraySelectionParameter::ValueType createdDataPaths;
  for(const auto& selectedDataPath : inputValues.SelectedArrayPaths)
  {
    DataPath createdDataPath = dataGroupPath.createChildPath(dataStructure.getDataAs<IDataArray>(selectedDataPath)->getName() + "Histogram");
    dataStructure.getDataAs<Float32Array>(createdDataPath)->fill(0.0f); // load with zeroes
    createdDataPaths.push_back(createdDataPath);
  }
  inputValues.CreatedHistogramDataPaths = createdDataPaths;

  return CalculateArrayHistogram(dataStructure, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex
