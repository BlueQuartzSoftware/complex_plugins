#include "FindAvgOrientationsFilter.hpp"

#include "OrientationAnalysis/Filters/Algorithms/FindAvgOrientations.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/CreateArrayAction.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"

using namespace complex;

namespace
{

using FeatureIdsArrayType = Int32Array;
using GoodVoxelsArrayType = BoolArray;

inline constexpr int32 k_IncorrectInputArray = -7000;
inline constexpr int32 k_MissingInputArray = -7001;
inline constexpr int32 k_MissingOrIncorrectGoodVoxelsArray = -7002;
} // namespace

namespace complex
{
//------------------------------------------------------------------------------
std::string FindAvgOrientationsFilter::name() const
{
  return FilterTraits<FindAvgOrientationsFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string FindAvgOrientationsFilter::className() const
{
  return FilterTraits<FindAvgOrientationsFilter>::className;
}

//------------------------------------------------------------------------------
Uuid FindAvgOrientationsFilter::uuid() const
{
  return FilterTraits<FindAvgOrientationsFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string FindAvgOrientationsFilter::humanName() const
{
  return "Find Feature Average Orientations";
}

//------------------------------------------------------------------------------
std::vector<std::string> FindAvgOrientationsFilter::defaultTags() const
{
  return {"#Statistics", "#Crystallography"};
}

//------------------------------------------------------------------------------
Parameters FindAvgOrientationsFilter::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insertSeparator(Parameters::Separator{"Input Element Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_CellFeatureIdsArrayPath_Key, "Cell Feature Ids", "", DataPath({"FeatureIds"}), ArraySelectionParameter::AllowedTypes{DataType::int32}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_CellPhasesArrayPath_Key, "Cell Phases", "", DataPath({"Phases"}), ArraySelectionParameter::AllowedTypes{DataType::int32}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_CellQuatsArrayPath_Key, "Cell Quaternions", "", DataPath({"Quats"}), ArraySelectionParameter::AllowedTypes{DataType::float32}));
  params.insertSeparator(Parameters::Separator{"Input Ensemble Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_CrystalStructuresArrayPath_Key, "Crystal Structures", "", DataPath{}, std::set<complex::DataType>({complex::DataType::uint32})));
  params.insertSeparator(Parameters::Separator{"Created Feature Data"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_AvgQuatsArrayPath_Key, "Average Quaternions", "", DataPath({"AvgQuats"})));
  params.insert(std::make_unique<ArrayCreationParameter>(k_AvgEulerAnglesArrayPath_Key, "Average Euler Angles", "", DataPath({"EulerAngles"})));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer FindAvgOrientationsFilter::clone() const
{
  return std::make_unique<FindAvgOrientationsFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult FindAvgOrientationsFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                  const std::atomic_bool& shouldCancel) const
{
  auto pCellFeatureIdsArrayPathValue = filterArgs.value<DataPath>(k_CellFeatureIdsArrayPath_Key);
  auto pCellPhasesArrayPathValue = filterArgs.value<DataPath>(k_CellPhasesArrayPath_Key);
  auto pCellQuatsArrayPathValue = filterArgs.value<DataPath>(k_CellQuatsArrayPath_Key);
  auto pCrystalStructuresArrayPathValue = filterArgs.value<DataPath>(k_CrystalStructuresArrayPath_Key);
  auto pAvgQuatsArrayPathValue = filterArgs.value<DataPath>(k_AvgQuatsArrayPath_Key);
  auto pAvgEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_AvgEulerAnglesArrayPath_Key);

  // Validate the Crystal Structures array
  const UInt32Array& crystalStructures = dataStructure.getDataRefAs<UInt32Array>(pCrystalStructuresArrayPathValue);
  if(crystalStructures.getNumberOfComponents() != 1)
  {
    return {nonstd::make_unexpected(std::vector<Error>{Error{k_IncorrectInputArray, "Crystal Structures Input Array must be a 1 component Int32 array"}})};
  }

  std::vector<DataPath> dataPaths;

  // Validate the Quats array
  const Float32Array& quats = dataStructure.getDataRefAs<Float32Array>(pCellQuatsArrayPathValue);
  if(quats.getNumberOfComponents() != 4)
  {
    return {nonstd::make_unexpected(std::vector<Error>{Error{k_IncorrectInputArray, "Quaternion Input Array must be a 4 component Float32 array"}})};
  }
  dataPaths.push_back(pCellQuatsArrayPathValue);

  // Validate the Phases array
  const Int32Array& phases = dataStructure.getDataRefAs<Int32Array>(pCellPhasesArrayPathValue);
  if(phases.getNumberOfComponents() != 1)
  {
    return {nonstd::make_unexpected(std::vector<Error>{Error{k_IncorrectInputArray, "Phases Input Array must be a 1 component Int32 array"}})};
  }
  dataPaths.push_back(pCellPhasesArrayPathValue);

  // Validate the FeatureIds array
  const Int32Array& featureIds = dataStructure.getDataRefAs<Int32Array>(pCellFeatureIdsArrayPathValue);
  if(featureIds.getNumberOfComponents() != 1)
  {
    return {nonstd::make_unexpected(std::vector<Error>{Error{k_IncorrectInputArray, "FeatureIds Input Array must be a 1 component Int32 array"}})};
  }
  dataPaths.push_back(pCellFeatureIdsArrayPathValue);

  // Make sure all the arrays have the same number of Tuples
  if(!dataStructure.validateNumberOfTuples(dataPaths))
  {
    return {nonstd::make_unexpected(std::vector<Error>{Error{-651, fmt::format("Input arrays do have matching tuple counts.")}})};
  }

  // Create output DataStructure Items
  auto createAvgQuatAction = std::make_unique<CreateArrayAction>(DataType::float32, std::vector<usize>{1}, std::vector<usize>{4}, pAvgQuatsArrayPathValue);
  auto createAvgEulerAction = std::make_unique<CreateArrayAction>(DataType::float32, std::vector<usize>{1}, std::vector<usize>{3}, pAvgEulerAnglesArrayPathValue);

  OutputActions actions;
  actions.actions.push_back(std::move(createAvgQuatAction));
  actions.actions.push_back(std::move(createAvgEulerAction));

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(actions)};
}

//------------------------------------------------------------------------------
Result<> FindAvgOrientationsFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                const std::atomic_bool& shouldCancel) const
{
  FindAvgOrientationsInputValues inputValues;

  inputValues.cellFeatureIdsArrayPath = filterArgs.value<DataPath>(k_CellFeatureIdsArrayPath_Key);
  inputValues.cellPhasesArrayPath = filterArgs.value<DataPath>(k_CellPhasesArrayPath_Key);
  inputValues.cellQuatsArrayPath = filterArgs.value<DataPath>(k_CellQuatsArrayPath_Key);
  inputValues.crystalStructuresArrayPath = filterArgs.value<DataPath>(k_CrystalStructuresArrayPath_Key);
  inputValues.avgQuatsArrayPath = filterArgs.value<DataPath>(k_AvgQuatsArrayPath_Key);
  inputValues.avgEulerAnglesArrayPath = filterArgs.value<DataPath>(k_AvgEulerAnglesArrayPath_Key);

  // Let the Algorithm instance do the work
  return FindAvgOrientations(dataStructure, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex
