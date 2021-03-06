#include "FindFeatureCentroidsFilter.hpp"

#include "Core/Filters/Algorithms/FindFeatureCentroids.hpp"

#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/CreateArrayAction.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/DataGroupSelectionParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string FindFeatureCentroidsFilter::name() const
{
  return FilterTraits<FindFeatureCentroidsFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string FindFeatureCentroidsFilter::className() const
{
  return FilterTraits<FindFeatureCentroidsFilter>::className;
}

//------------------------------------------------------------------------------
Uuid FindFeatureCentroidsFilter::uuid() const
{
  return FilterTraits<FindFeatureCentroidsFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string FindFeatureCentroidsFilter::humanName() const
{
  return "Find Feature Centroids";
}

//------------------------------------------------------------------------------
std::vector<std::string> FindFeatureCentroidsFilter::defaultTags() const
{
  return {"#Generic", "#Morphological"};
}

//------------------------------------------------------------------------------
Parameters FindFeatureCentroidsFilter::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insertSeparator(Parameters::Separator{"Required Input Cell Data"});
  params.insert(
      std::make_unique<GeometrySelectionParameter>(k_SelectedImageGeometry_Key, "Selected Image Geometry", "", DataPath{}, GeometrySelectionParameter::AllowedTypes{AbstractGeometry::Type::Image}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeatureIdsArrayPath_Key, "Feature Ids", "", DataPath({"FeatureIds"}), ArraySelectionParameter::AllowedTypes{DataType::int32}));
  params.insert(std::make_unique<DataGroupSelectionParameter>(k_FeatureAttributeMatrix_Key, "Cell Feature AttributeMatrix", "", DataPath({"Data Container", "Feature Data"})));

  params.insertSeparator(Parameters::Separator{"Created Output Feature Data"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_CentroidsArrayPath_Key, "Centroids", "", DataPath({"Centroids"})));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer FindFeatureCentroidsFilter::clone() const
{
  return std::make_unique<FindFeatureCentroidsFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult FindFeatureCentroidsFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                   const std::atomic_bool& shouldCancel) const
{
  /**
   * These are the values that were gathered from the UI or the pipeline file or
   * otherwise passed into the filter. These are here for your convenience. If you
   * do not need some of them remove them.
   */
  auto pFeatureIdsArrayPathValue = filterArgs.value<DataPath>(k_FeatureIdsArrayPath_Key);
  auto pCentroidsArrayPathValue = filterArgs.value<DataPath>(k_CentroidsArrayPath_Key);
  auto featureAttrMatrixPath = filterArgs.value<DataPath>(k_FeatureAttributeMatrix_Key);

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

  IDataStore::ShapeType tupleShape;

  // Feature Data:
  // Validating the Feature Attribute Matrix and trying to find a child of the Group
  // that is an IDataArray subclass, so we can get the proper tuple shape
  const auto* featureAttrMatrix = dataStructure.getDataAs<DataGroup>(featureAttrMatrixPath);
  if(featureAttrMatrix == nullptr)
  {
    return {nonstd::make_unexpected(std::vector<Error>{Error{-12700, "Feature Attribute Matrix Path is NOT a DataGroup"}})};
  }
  const auto& featureAttrMatrixChildren = featureAttrMatrix->getDataMap();
  bool childDataArrayFound = false;
  for(const auto& child : featureAttrMatrixChildren)
  {
    if(child.second->getDataObjectType() == DataObject::Type::DataArray)
    {
      const auto* childDataArray = dynamic_cast<IDataArray*>(child.second.get());
      tupleShape = childDataArray->getIDataStore()->getTupleShape();
      childDataArrayFound = true;
      break;
    }
  }
  // We must find a child IDataArray subclass to get the tuple shape correct.
  if(!childDataArrayFound)
  {
    return {nonstd::make_unexpected(std::vector<Error>{Error{-12701, "Feature Attribute Matrix does not have a child IDataArray"}})};
  }

  // Create the CreateArrayAction within a scope so that we do not accidentally use the variable is it is getting "moved"
  {
    auto createFeatureCentroidsAction = std::make_unique<CreateArrayAction>(DataType::float32, tupleShape, std::vector<usize>{3ULL}, pCentroidsArrayPathValue);
    resultOutputActions.value().actions.push_back(std::move(createFeatureCentroidsAction));
  }
  // Store the preflight updated value(s) into the preflightUpdatedValues vector using
  // the appropriate methods.
  // None found based on the filter parameters

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> FindFeatureCentroidsFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                 const std::atomic_bool& shouldCancel) const
{
  FindFeatureCentroidsInputValues inputValues;

  inputValues.FeatureIdsArrayPath = filterArgs.value<DataPath>(k_FeatureIdsArrayPath_Key);
  inputValues.CentroidsArrayPath = filterArgs.value<DataPath>(k_CentroidsArrayPath_Key);
  inputValues.FeatureAttributeMatrixPath = filterArgs.value<DataPath>(k_FeatureAttributeMatrix_Key);
  inputValues.ImageGeometryPath = filterArgs.value<DataPath>(k_SelectedImageGeometry_Key);

  return FindFeatureCentroids(dataStructure, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex
