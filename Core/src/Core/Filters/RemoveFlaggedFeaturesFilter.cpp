#include "RemoveFlaggedFeaturesFilter.hpp"

#include "Core/Filters/Algorithms/RemoveFlaggedFeatures.hpp"
#include "complex/DataStructure/AttributeMatrix.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"
#include "complex/Parameters/MultiArraySelectionParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string RemoveFlaggedFeaturesFilter::name() const
{
  return FilterTraits<RemoveFlaggedFeaturesFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string RemoveFlaggedFeaturesFilter::className() const
{
  return FilterTraits<RemoveFlaggedFeaturesFilter>::className;
}

//------------------------------------------------------------------------------
Uuid RemoveFlaggedFeaturesFilter::uuid() const
{
  return FilterTraits<RemoveFlaggedFeaturesFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string RemoveFlaggedFeaturesFilter::humanName() const
{
  return "Remove Flagged Features";
}

//------------------------------------------------------------------------------
std::vector<std::string> RemoveFlaggedFeaturesFilter::defaultTags() const
{
  return {"#Processing", "#Cleanup"};
}

//------------------------------------------------------------------------------
Parameters RemoveFlaggedFeaturesFilter::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<BoolParameter>(k_FillRemovedFeatures_Key, "Fill-in Removed Features", "", false));
  params.insert(std::make_unique<GeometrySelectionParameter>(k_ImageGeometry_Key, "Selected Image Geometry", "", DataPath{}, GeometrySelectionParameter::AllowedTypes{IGeometry::Type::Image}));
  params.insertSeparator(Parameters::Separator{"Cell Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeatureIdsArrayPath_Key, "Feature Ids", "", DataPath({"CellData", "FeatureIds"}), ArraySelectionParameter::AllowedTypes{DataType::int32}));
  params.insertSeparator(Parameters::Separator{"Cell Feature Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_FlaggedFeaturesArrayPath_Key, "Flagged Features", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::boolean}));
  params.insert(std::make_unique<MultiArraySelectionParameter>(k_IgnoredDataArrayPaths_Key, "Attribute Arrays to Ignore", "",
                                                               MultiArraySelectionParameter::ValueType{DataPath(), DataPath(), DataPath()}, complex::GetAllDataTypes()));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer RemoveFlaggedFeaturesFilter::clone() const
{
  return std::make_unique<RemoveFlaggedFeaturesFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult RemoveFlaggedFeaturesFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                    const std::atomic_bool& shouldCancel) const
{
  // auto pFillRemovedFeaturesValue = filterArgs.value<bool>(k_FillRemovedFeatures_Key);
  auto pFeatureIdsArrayPathValue = filterArgs.value<DataPath>(k_FeatureIdsArrayPath_Key);
  auto pFlaggedFeaturesArrayPathValue = filterArgs.value<DataPath>(k_FlaggedFeaturesArrayPath_Key);
  // auto pIgnoredDataArrayPathsValue = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_IgnoredDataArrayPaths_Key);

  PreflightResult preflightResult;
  complex::Result<OutputActions> resultOutputActions;
  std::vector<PreflightValue> preflightUpdatedValues;

  if(dataStructure.getDataAs<Int32Array>(pFeatureIdsArrayPathValue) == nullptr)
  {
    return {nonstd::make_unexpected(std::vector<Error>{Error{-9890, fmt::format("Could not find selected Feature Ids Data Array at path '{}'", pFeatureIdsArrayPathValue.toString())}})};
  }

  if(dataStructure.getDataAs<BoolArray>(pFlaggedFeaturesArrayPathValue) == nullptr)
  {
    return {nonstd::make_unexpected(std::vector<Error>{Error{-9891, fmt::format("Could not find selected Flagged Features Data Array at path '{}'", pFlaggedFeaturesArrayPathValue.toString())}})};
  }

  if(dataStructure.getDataAs<AttributeMatrix>(pFlaggedFeaturesArrayPathValue.getParent()) == nullptr)
  {
    return {nonstd::make_unexpected(std::vector<Error>{
        Error{-9892, fmt::format("Could not find the parent Attribute Matrix for the selected Flagged Features Data Array at path '{}'", pFlaggedFeaturesArrayPathValue.toString())}})};
  }

  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> RemoveFlaggedFeaturesFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                  const std::atomic_bool& shouldCancel) const
{
  RemoveFlaggedFeaturesInputValues inputValues;
  inputValues.FillRemovedFeatures = filterArgs.value<bool>(k_FillRemovedFeatures_Key);
  inputValues.FeatureIdsArrayPath = filterArgs.value<DataPath>(k_FeatureIdsArrayPath_Key);
  inputValues.FlaggedFeaturesArrayPath = filterArgs.value<DataPath>(k_FlaggedFeaturesArrayPath_Key);
  inputValues.IgnoredDataArrayPaths = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_IgnoredDataArrayPaths_Key);
  inputValues.ImageGeometryPath = filterArgs.value<DataPath>(k_ImageGeometry_Key);

  return RemoveFlaggedFeatures(dataStructure, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex