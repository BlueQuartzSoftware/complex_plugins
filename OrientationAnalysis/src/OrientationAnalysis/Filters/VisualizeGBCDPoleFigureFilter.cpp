#include "VisualizeGBCDPoleFigureFilter.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/Filter/Actions/CreateArrayAction.hpp"
#include "complex/Filter/Actions/CreateImageGeometryAction.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/DataObjectNameParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

#include "OrientationAnalysis/Filters/Algorithms/VisualizeGBCDPoleFigure.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string VisualizeGBCDPoleFigureFilter::name() const
{
  return FilterTraits<VisualizeGBCDPoleFigureFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string VisualizeGBCDPoleFigureFilter::className() const
{
  return FilterTraits<VisualizeGBCDPoleFigureFilter>::className;
}

//------------------------------------------------------------------------------
Uuid VisualizeGBCDPoleFigureFilter::uuid() const
{
  return FilterTraits<VisualizeGBCDPoleFigureFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string VisualizeGBCDPoleFigureFilter::humanName() const
{
  return "Export GBCD Pole Figure (VTK)";
}

//------------------------------------------------------------------------------
std::vector<std::string> VisualizeGBCDPoleFigureFilter::defaultTags() const
{
  return {"#IO", "#Output", "#Write", "#Export"};
}

//------------------------------------------------------------------------------
Parameters VisualizeGBCDPoleFigureFilter::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insertSeparator(Parameters::Separator{"Input Parameters"});
  params.insert(std::make_unique<Int32Parameter>(k_PhaseOfInterest_Key, "Phase of Interest", "Index of the Ensemble for which to plot the pole figure", 1));
  params.insert(std::make_unique<VectorFloat32Parameter>(k_MisorientationRotation_Key, "Misorientation Angle-Axis", "Angle-Axis values for drawing GBCD", std::vector<float32>{60.0F, 1.0F, 1.0F, 1.0F},
                                                         std::vector<std::string>{"Angle (Deg)", "h", "k", "l"}));
  params.insert(std::make_unique<FileSystemPathParameter>(k_OutputFile_Key, "Output Regular Grid VTK File", "The output .vtk file path", FileSystemPathParameter::ValueType{},
                                                          FileSystemPathParameter::ExtensionsType{".vtk"}, FileSystemPathParameter::PathType::OutputFile, false));
  params.insert(std::make_unique<Int32Parameter>(k_OutputImageDimension_Key, "Output Image Dimension", "The value to use for the dimensions for the image geometry", 1));
  params.insertSeparator(Parameters::Separator{"Required Face Ensemble Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_GBCDArrayPath_Key, "Input GBCD",
                                                          "5 parameter GBCD data. The 6th component is used internally to track the northern vs. southern hemisphere of the Lambert sphere",
                                                          DataPath({"FaceEnsembleData", "GBCD"}), ArraySelectionParameter::AllowedTypes{DataType::float64}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_CrystalStructuresArrayPath_Key, "Crystal Structures", "Enumeration representing the crystal structure for each Ensemble",
                                                          DataPath({"Ensemble Data", "CrystalStructures"}), ArraySelectionParameter::AllowedTypes{DataType::uint32},
                                                          ArraySelectionParameter::AllowedComponentShapes{{1}}));
  params.insertSeparator(Parameters::Separator{"Output Pole Figure"});
  params.insert(std::make_unique<DataGroupCreationParameter>(k_ImageGeometryName_Key, "Image Geometry", "The path to the created image geometry", DataPath({"[ImageGeometry]"})));
  params.insert(std::make_unique<DataObjectNameParameter>(k_CellAttributeMatrixName_Key, "Cell Attribute Matrix Name", "The name of the cell attribute matrix for the created image geometry",
                                                          ImageGeom::k_CellDataName));
  params.insert(std::make_unique<DataObjectNameParameter>(k_CellIntensityArrayName_Key, "Cell Intensity Array Name", "The name of the created cell intensity data array", "Intesity"));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer VisualizeGBCDPoleFigureFilter::clone() const
{
  return std::make_unique<VisualizeGBCDPoleFigureFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult VisualizeGBCDPoleFigureFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                      const std::atomic_bool& shouldCancel) const
{
  auto pPhaseOfInterestValue = filterArgs.value<int32>(k_PhaseOfInterest_Key);
  auto pMisorientationRotationValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_MisorientationRotation_Key);
  auto pOutputFileValue = filterArgs.value<FileSystemPathParameter::ValueType>(k_OutputFile_Key);
  auto pOutputImageDimension = static_cast<usize>(filterArgs.value<int32>(k_OutputImageDimension_Key));
  auto pGBCDArrayPathValue = filterArgs.value<DataPath>(k_GBCDArrayPath_Key);
  auto pCrystalStructuresArrayPathValue = filterArgs.value<DataPath>(k_CrystalStructuresArrayPath_Key);
  auto pImageGeometryPath = filterArgs.value<DataPath>(k_ImageGeometryName_Key);
  auto pCellAttributeMatrixName = filterArgs.value<std::string>(k_CellAttributeMatrixName_Key);
  auto pCellIntensityArrayName = filterArgs.value<std::string>(k_CellIntensityArrayName_Key);

  PreflightResult preflightResult;
  complex::Result<OutputActions> resultOutputActions;
  std::vector<PreflightValue> preflightUpdatedValues;

  if(dataStructure.getDataAs<UInt32Array>(pCrystalStructuresArrayPathValue) == nullptr)
  {
    return {MakeErrorResult<OutputActions>(-34640, fmt::format("Could not find crystal structures array at path '{}'", pCrystalStructuresArrayPathValue.toString()))};
  }

  const auto gbcd = dataStructure.getDataAs<Float64Array>(pGBCDArrayPathValue);
  if(gbcd == nullptr)
  {
    return {MakeErrorResult<OutputActions>(-34641, fmt::format("Could not find GBCD array at path '{}'", pGBCDArrayPathValue.toString()))};
  }

  auto numEnsembles = gbcd->getNumberOfTuples();
  if(pPhaseOfInterestValue >= numEnsembles)
  {
    return {MakeErrorResult<OutputActions>(-34642, fmt::format("The phase index {} is larger than the number of Ensembles {}", pPhaseOfInterestValue, numEnsembles))};
  }

  std::vector<usize> tDims = {pOutputImageDimension, pOutputImageDimension, 1ULL};
  auto createImageGeometryAction = std::make_unique<CreateImageGeometryAction>(
      pImageGeometryPath, tDims, CreateImageGeometryAction::OriginType{-1.0f, -1.0f, 0.0f},
      CreateImageGeometryAction::SpacingType{2.0f / pOutputImageDimension, 2.0f / pOutputImageDimension, 2.0f / pOutputImageDimension}, pCellAttributeMatrixName);
  resultOutputActions.value().actions.push_back(std::move(createImageGeometryAction));

  std::vector<usize> cDims = {1};
  DataPath cellIntensityArrayPath = pImageGeometryPath.createChildPath(pCellAttributeMatrixName).createChildPath(pCellIntensityArrayName);
  auto createArrayAction = std::make_unique<CreateArrayAction>(DataType::float64, tDims, cDims, cellIntensityArrayPath);
  resultOutputActions.value().actions.push_back(std::move(createArrayAction));

  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> VisualizeGBCDPoleFigureFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                    const std::atomic_bool& shouldCancel) const
{
  VisualizeGBCDPoleFigureInputValues inputValues;
  inputValues.PhaseOfInterest = filterArgs.value<int32>(k_PhaseOfInterest_Key);
  inputValues.MisorientationRotation = filterArgs.value<VectorFloat32Parameter::ValueType>(k_MisorientationRotation_Key);
  inputValues.OutputFile = filterArgs.value<FileSystemPathParameter::ValueType>(k_OutputFile_Key);
  inputValues.GBCDArrayPath = filterArgs.value<DataPath>(k_GBCDArrayPath_Key);
  inputValues.CrystalStructuresArrayPath = filterArgs.value<DataPath>(k_CrystalStructuresArrayPath_Key);
  inputValues.OutputImageDimension = filterArgs.value<int32>(k_OutputImageDimension_Key);
  inputValues.ImageGeometryPath = filterArgs.value<DataPath>(k_ImageGeometryName_Key);
  inputValues.CellAttributeMatrixName = filterArgs.value<std::string>(k_CellAttributeMatrixName_Key);
  inputValues.CellIntensityArrayName = filterArgs.value<std::string>(k_CellIntensityArrayName_Key);

  return VisualizeGBCDPoleFigure(dataStructure, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex
