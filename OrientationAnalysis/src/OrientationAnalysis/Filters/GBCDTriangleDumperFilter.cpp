#include "GBCDTriangleDumperFilter.hpp"

#include "OrientationAnalysis/Filters/Algorithms/GBCDTriangleDumper.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/EmptyAction.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string GBCDTriangleDumperFilter::name() const
{
  return FilterTraits<GBCDTriangleDumperFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string GBCDTriangleDumperFilter::className() const
{
  return FilterTraits<GBCDTriangleDumperFilter>::className;
}

//------------------------------------------------------------------------------
Uuid GBCDTriangleDumperFilter::uuid() const
{
  return FilterTraits<GBCDTriangleDumperFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string GBCDTriangleDumperFilter::humanName() const
{
  return "Export GBCD Triangles File";
}

//------------------------------------------------------------------------------
std::vector<std::string> GBCDTriangleDumperFilter::defaultTags() const
{
  return {"#IO", "#Output", "#Write", "#Export"};
}

//------------------------------------------------------------------------------
Parameters GBCDTriangleDumperFilter::parameters() const
{
  Parameters params;

  // Create the parameter descriptors that are needed for this filter
  params.insertSeparator(Parameters::Separator{"Parameters"});
  params.insert(std::make_unique<FileSystemPathParameter>(k_OutputFile_Key, "Output File", "The output GBCD triangle file path", "", FileSystemPathParameter::ExtensionsType{".ph"},
                                                          FileSystemPathParameter::PathType::OutputFile));
  params.insertSeparator(Parameters::Separator{"Required Face Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_SurfaceMeshFaceLabelsArrayPath_Key, "Face Labels", "Specifies which Features are on either side of each Face", DataPath{},
                                                          ArraySelectionParameter::AllowedTypes{DataType::int32}, ArraySelectionParameter::AllowedComponentShapes{{2}}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_SurfaceMeshFaceNormalsArrayPath_Key, "Face Normals", "Specifies the normal of each Face", DataPath{},
                                                          ArraySelectionParameter::AllowedTypes{DataType::float64}, ArraySelectionParameter::AllowedComponentShapes{{3}}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_SurfaceMeshFaceAreasArrayPath_Key, "Face Areas", "Specifies the area of each Face", DataPath{},
                                                          ArraySelectionParameter::AllowedTypes{DataType::float64}, ArraySelectionParameter::AllowedComponentShapes{{1}}));
  params.insertSeparator(Parameters::Separator{"Required Cell Feature Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeatureEulerAnglesArrayPath_Key, "Average Euler Angles",
                                                          "Three angles defining the orientation of the Feature in Bunge convention (Z-X-Z).", DataPath{},
                                                          ArraySelectionParameter::AllowedTypes{DataType::float32}, ArraySelectionParameter::AllowedComponentShapes{{3}}));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer GBCDTriangleDumperFilter::clone() const
{
  return std::make_unique<GBCDTriangleDumperFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult GBCDTriangleDumperFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                 const std::atomic_bool& shouldCancel) const
{
  auto pOutputFileValue = filterArgs.value<FileSystemPathParameter::ValueType>(k_OutputFile_Key);
  auto pSurfaceMeshFaceLabelsArrayPathValue = filterArgs.value<DataPath>(k_SurfaceMeshFaceLabelsArrayPath_Key);
  auto pSurfaceMeshFaceNormalsArrayPathValue = filterArgs.value<DataPath>(k_SurfaceMeshFaceNormalsArrayPath_Key);
  auto pSurfaceMeshFaceAreasArrayPathValue = filterArgs.value<DataPath>(k_SurfaceMeshFaceAreasArrayPath_Key);
  auto pFeatureEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_FeatureEulerAnglesArrayPath_Key);

  PreflightResult preflightResult;
  complex::Result<OutputActions> resultOutputActions;
  std::vector<PreflightValue> preflightUpdatedValues;

  // make sure all the face data has same number of tuples (i.e. they should all be coming from the same Triangle Geometry)
  std::vector<DataPath> triangleArrayPaths = {pSurfaceMeshFaceLabelsArrayPathValue, pSurfaceMeshFaceNormalsArrayPathValue, pSurfaceMeshFaceAreasArrayPathValue};
  if(!dataStructure.validateNumberOfTuples(triangleArrayPaths))
  {
    return MakePreflightErrorResult(
        -48320,
        "The input triangle geometry face data arrays have inconsistent numbers of tuples.  Make sure the face labels, face normals, and face areas arrays all have the same number of tuples.");
  }

  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> GBCDTriangleDumperFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                               const std::atomic_bool& shouldCancel) const
{
  GBCDTriangleDumperInputValues inputValues;

  inputValues.OutputFile = filterArgs.value<FileSystemPathParameter::ValueType>(k_OutputFile_Key);
  inputValues.SurfaceMeshFaceLabelsArrayPath = filterArgs.value<DataPath>(k_SurfaceMeshFaceLabelsArrayPath_Key);
  inputValues.SurfaceMeshFaceNormalsArrayPath = filterArgs.value<DataPath>(k_SurfaceMeshFaceNormalsArrayPath_Key);
  inputValues.SurfaceMeshFaceAreasArrayPath = filterArgs.value<DataPath>(k_SurfaceMeshFaceAreasArrayPath_Key);
  inputValues.FeatureEulerAnglesArrayPath = filterArgs.value<DataPath>(k_FeatureEulerAnglesArrayPath_Key);

  return GBCDTriangleDumper(dataStructure, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex
