#include "FindTriangleGeomShapesFilter.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/DataGroupSelectionParameter.hpp"
#include "complex/Parameters/DataObjectNameParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"

#include "Core/Filters/Algorithms/FindTriangleGeomShapes.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string FindTriangleGeomShapesFilter::name() const
{
  return FilterTraits<FindTriangleGeomShapesFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string FindTriangleGeomShapesFilter::className() const
{
  return FilterTraits<FindTriangleGeomShapesFilter>::className;
}

//------------------------------------------------------------------------------
Uuid FindTriangleGeomShapesFilter::uuid() const
{
  return FilterTraits<FindTriangleGeomShapesFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string FindTriangleGeomShapesFilter::humanName() const
{
  return "Find Feature Shapes from Triangle Geometry";
}

//------------------------------------------------------------------------------
std::vector<std::string> FindTriangleGeomShapesFilter::defaultTags() const
{
  return {"#Statistics", "#Morphological", "#SurfaceMesh"};
}

//------------------------------------------------------------------------------
Parameters FindTriangleGeomShapesFilter::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<GeometrySelectionParameter>(k_TriGeometryDataPath_Key, "Triangle Geometry", "The complete path to the Geometry for which to calculate the normals", DataPath{},
                                                             GeometrySelectionParameter::AllowedTypes{IGeometry::Type::Triangle}));
  params.insertSeparator(Parameters::Separator{"Required Face Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_FaceLabelsArrayPath_Key, "Face Labels", "", DataPath{}, ArraySelectionParameter::AllowedTypes{}));

  params.insertSeparator(Parameters::Separator{"Required Face Feature Data"});
  params.insert(std::make_unique<DataGroupSelectionParameter>(k_FeatureAttributeMatrixName_Key, "Face Feature Attribute Matrix", "", DataPath{},
                                                              DataGroupSelectionParameter::AllowedTypes{BaseGroup::GroupType::AttributeMatrix}));
  params.insert(
      std::make_unique<ArraySelectionParameter>(k_CentroidsArrayPath_Key, "Feature Centroids", "", DataPath({"FeatureData", "Centroids"}), ArraySelectionParameter::AllowedTypes{DataType::float32}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_VolumesArrayPath_Key, "Feature Volumes", "", DataPath{}, ArraySelectionParameter::AllowedTypes{}));

  params.insertSeparator(Parameters::Separator{"Created Face Feature Data Arrays"});
  params.insert(std::make_unique<DataObjectNameParameter>(k_Omega3sArrayName_Key, "Omega3s", "", "Omega3s"));
  params.insert(std::make_unique<DataObjectNameParameter>(k_AxisLengthsArrayName_Key, "Axis Lengths", "", "AxisLengths"));
  params.insert(std::make_unique<DataObjectNameParameter>(k_AxisEulerAnglesArrayName_Key, "Axis Euler Angles", "", "AxisEulerAngles"));
  params.insert(std::make_unique<DataObjectNameParameter>(k_AspectRatiosArrayName_Key, "Aspect Ratios", "", "AspectRatios"));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer FindTriangleGeomShapesFilter::clone() const
{
  return std::make_unique<FindTriangleGeomShapesFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult FindTriangleGeomShapesFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
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
  auto pFaceLabelsArrayPathValue = filterArgs.value<DataPath>(k_FaceLabelsArrayPath_Key);
  auto pFeatureAttributeMatrixNameValue = filterArgs.value<DataPath>(k_FeatureAttributeMatrixName_Key);
  auto pCentroidsArrayPathValue = filterArgs.value<DataPath>(k_CentroidsArrayPath_Key);
  auto pVolumesArrayPathValue = filterArgs.value<DataPath>(k_VolumesArrayPath_Key);
  auto omega3sArrayNameValue = filterArgs.value<DataObjectNameParameter::ValueType>(k_Omega3sArrayName_Key);
  auto axisLengthsArrayNameValue = filterArgs.value<DataObjectNameParameter::ValueType>(k_AxisLengthsArrayName_Key);
  auto axisEulerAnglesArrayNameValue = filterArgs.value<DataObjectNameParameter::ValueType>(k_AxisEulerAnglesArrayName_Key);
  auto aspectRatiosArrayNameValue = filterArgs.value<DataObjectNameParameter::ValueType>(k_AspectRatiosArrayName_Key);

  DataPath omega3sArrayPath = pFeatureAttributeMatrixNameValue.createChildPath(omega3sArrayNameValue);
  DataPath axisLengthsArrayPath = pFeatureAttributeMatrixNameValue.createChildPath(axisLengthsArrayNameValue);
  DataPath axisEulerAnglesArrayPath = pFeatureAttributeMatrixNameValue.createChildPath(axisEulerAnglesArrayNameValue);
  DataPath aspectRatiosArrayPath = pFeatureAttributeMatrixNameValue.createChildPath(aspectRatiosArrayNameValue);

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
Result<> FindTriangleGeomShapesFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                   const std::atomic_bool& shouldCancel) const
{
  FindTriangleGeomShapesInputValues inputValues;
  inputValues.FaceLabelsArrayPath = filterArgs.value<DataPath>(k_FaceLabelsArrayPath_Key);
  inputValues.FeatureAttributeMatrixName = filterArgs.value<DataPath>(k_FeatureAttributeMatrixName_Key);
  inputValues.CentroidsArrayPath = filterArgs.value<DataPath>(k_CentroidsArrayPath_Key);
  inputValues.VolumesArrayPath = filterArgs.value<DataPath>(k_VolumesArrayPath_Key);

  auto omega3sArrayNameValue = filterArgs.value<DataObjectNameParameter::ValueType>(k_Omega3sArrayName_Key);
  auto axisLengthsArrayNameValue = filterArgs.value<DataObjectNameParameter::ValueType>(k_AxisLengthsArrayName_Key);
  auto axisEulerAnglesArrayNameValue = filterArgs.value<DataObjectNameParameter::ValueType>(k_AxisEulerAnglesArrayName_Key);
  auto aspectRatiosArrayNameValue = filterArgs.value<DataObjectNameParameter::ValueType>(k_AspectRatiosArrayName_Key);

  DataPath omega3sArrayPath = inputValues.FeatureAttributeMatrixName.createChildPath(omega3sArrayNameValue);
  DataPath axisLengthsArrayPath = inputValues.FeatureAttributeMatrixName.createChildPath(axisLengthsArrayNameValue);
  DataPath axisEulerAnglesArrayPath = inputValues.FeatureAttributeMatrixName.createChildPath(axisEulerAnglesArrayNameValue);
  DataPath aspectRatiosArrayPath = inputValues.FeatureAttributeMatrixName.createChildPath(aspectRatiosArrayNameValue);

  inputValues.Omega3sArrayName = omega3sArrayPath;
  inputValues.AxisLengthsArrayName = axisLengthsArrayPath;
  inputValues.AxisEulerAnglesArrayName = axisEulerAnglesArrayPath;
  inputValues.AspectRatiosArrayName = aspectRatiosArrayPath;

  return FindTriangleGeomShapes(dataStructure, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex
