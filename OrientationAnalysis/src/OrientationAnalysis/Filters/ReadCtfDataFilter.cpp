#include "ReadCtfDataFilter.hpp"

#include "OrientationAnalysis/Filters/Algorithms/ReadCtfData.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/CreateArrayAction.hpp"
#include "complex/Filter/Actions/CreateDataGroupAction.hpp"
#include "complex/Filter/Actions/CreateImageGeometryAction.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"

#include "EbsdLib/IO/HKL/CtfFields.h"
#include "EbsdLib/IO/HKL/CtfPhase.h"
#include "EbsdLib/IO/HKL/CtfReader.h"

#include <filesystem>
namespace fs = std::filesystem;

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string ReadCtfDataFilter::name() const
{
  return FilterTraits<ReadCtfDataFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string ReadCtfDataFilter::className() const
{
  return FilterTraits<ReadCtfDataFilter>::className;
}

//------------------------------------------------------------------------------
Uuid ReadCtfDataFilter::uuid() const
{
  return FilterTraits<ReadCtfDataFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string ReadCtfDataFilter::humanName() const
{
  return "Import Oxford Instr. EBSD Data (.ctf)";
}

//------------------------------------------------------------------------------
std::vector<std::string> ReadCtfDataFilter::defaultTags() const
{
  return {"#IO", "#Input", "#Read", "#Import"};
}

//------------------------------------------------------------------------------
Parameters ReadCtfDataFilter::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<FileSystemPathParameter>(k_InputFile_Key, "Input File", "", fs::path("input.ctf"), FileSystemPathParameter::ExtensionsType{".ctf"},
                                                          FileSystemPathParameter::PathType::InputFile));
  params.insert(std::make_unique<BoolParameter>(k_DegreesToRadians_Key, "Convert Euler Angles to Radians", "", false));
  params.insert(std::make_unique<BoolParameter>(k_EdaxHexagonalAlignment_Key, "Convert Hexagonal X-Axis to EDAX Standard", "", false));

  params.insertSeparator(Parameters::Separator{"Created Data Structure Objects"});
  params.insert(std::make_unique<DataGroupCreationParameter>(k_DataContainerName_Key, "Created Image Geometry", "", DataPath({"DataContainer"})));
  //  params.insertSeparator(Parameters::Separator{"Cell Data"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_CellAttributeMatrixName_Key, "Created Cell Attribute Matrix", "", DataPath({"DataContainer", "CellData"})));
  // params.insertSeparator(Parameters::Separator{"Cell Ensemble Data"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_CellEnsembleAttributeMatrixName_Key, "Created Cell Ensemble Attribute Matrix", "", DataPath({"DataContainer", "CellEnsembleData"})));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ReadCtfDataFilter::clone() const
{
  return std::make_unique<ReadCtfDataFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ReadCtfDataFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                          const std::atomic_bool& shouldCancel) const
{
  auto pInputFileValue = filterArgs.value<FileSystemPathParameter::ValueType>(k_InputFile_Key);
  auto pDegreesToRadiansValue = filterArgs.value<bool>(k_DegreesToRadians_Key);
  auto pEdaxHexagonalAlignmentValue = filterArgs.value<bool>(k_EdaxHexagonalAlignment_Key);
  auto pDataContainerNameValue = filterArgs.value<DataPath>(k_DataContainerName_Key);
  auto pCellAttributeMatrixNameValue = filterArgs.value<DataPath>(k_CellAttributeMatrixName_Key);
  auto pCellEnsembleAttributeMatrixNameValue = filterArgs.value<DataPath>(k_CellEnsembleAttributeMatrixName_Key);
  auto imageGeomPath = filterArgs.value<DataPath>(k_DataContainerName_Key);


  PreflightResult preflightResult;

  complex::Result<OutputActions> resultOutputActions;

  std::vector<PreflightValue> preflightUpdatedValues;

  CtfReader reader;
  reader.setFileName(pInputFileValue.string());
  int32_t err = reader.readHeaderOnly();
  if(err < 0)
  {
    return {MakeErrorResult<OutputActions>(reader.getErrorCode(), reader.getErrorMessage())};
  }

  CreateImageGeometryAction::DimensionType imageGeomDims = {static_cast<size_t>(reader.getXDimension()), static_cast<size_t>(reader.getYDimension()), static_cast<size_t>(1)};
  std::vector<size_t> tupleDims = {imageGeomDims[2], imageGeomDims[1], imageGeomDims[0]};

  CreateImageGeometryAction::SpacingType spacing = {reader.getXStep(), reader.getYStep(), 1.0F};
  CreateImageGeometryAction::OriginType origin = {0.0F, 0.0F, 0.0F};

  resultOutputActions.value().actions.push_back(std::make_unique<CreateImageGeometryAction>(std::move(imageGeomPath), std::move(imageGeomDims), std::move(origin), std::move(spacing)));

  // Create the Cell AttributeMatrix
  {
    auto createDataGroupAction = std::make_unique<CreateDataGroupAction>(pCellAttributeMatrixNameValue);
    resultOutputActions.value().actions.push_back(std::move(createDataGroupAction));
  }

  // Create the Ensemble AttributeMatrix
  {
    auto createDataGroupAction = std::make_unique<CreateDataGroupAction>(pCellEnsembleAttributeMatrixNameValue);
    resultOutputActions.value().actions.push_back(std::move(createDataGroupAction));
  }

  CtfFields ctfFeatures;
  std::vector<std::string> names = ctfFeatures.getFilterFeatures<std::vector<std::string>>();
  std::vector<size_t> cDims = {1ULL};

  for(const auto& name : names)
  {
    if(reader.getPointerType(name) == EbsdLib::NumericTypes::Type::Int32)
    {
      DataPath dataArrayPath = pCellAttributeMatrixNameValue.createChildPath(name);
      auto action = std::make_unique<CreateArrayAction>(complex::DataType::int32, tupleDims, cDims, dataArrayPath);
      resultOutputActions.value().actions.push_back(std::move(action));
    }
    else if(reader.getPointerType(name) == EbsdLib::NumericTypes::Type::Float)
    {
      DataPath dataArrayPath = pCellAttributeMatrixNameValue.createChildPath(name);
      auto action = std::make_unique<CreateArrayAction>(complex::DataType::float32, tupleDims, cDims, dataArrayPath);
      resultOutputActions.value().actions.push_back(std::move(action));
    }
  }

  // Create the Cell Phases Array
  {
    cDims[0] = 1;
    DataPath dataArrayPath = pCellAttributeMatrixNameValue.createChildPath(EbsdLib::CtfFile::Phases);
    auto action = std::make_unique<CreateArrayAction>(complex::DataType::int32, tupleDims, cDims, dataArrayPath);
    resultOutputActions.value().actions.push_back(std::move(action));
  }

  // Create the Cell Euler Angles Array
  {
    cDims[0] = 3;
    DataPath dataArrayPath = pCellAttributeMatrixNameValue.createChildPath(EbsdLib::CtfFile::EulerAngles);
    auto action = std::make_unique<CreateArrayAction>(complex::DataType::float32, tupleDims, cDims, dataArrayPath);
    resultOutputActions.value().actions.push_back(std::move(action));
  }

  std::vector<std::shared_ptr<CtfPhase>> angPhases = reader.getPhaseVector();
  tupleDims = {angPhases.size() + 1}; // Always create 1 extra slot for the phases.
  // Create the Crystal Structures Array
  {
    cDims[0] = 1;
    DataPath dataArrayPath = pCellEnsembleAttributeMatrixNameValue.createChildPath(EbsdLib::CtfFile::CrystalStructures);
    auto action = std::make_unique<CreateArrayAction>(complex::DataType::uint32, tupleDims, cDims, dataArrayPath);
    resultOutputActions.value().actions.push_back(std::move(action));
  }
  // Create the Lattice Constants Array
  {
    cDims[0] = 6;
    DataPath dataArrayPath = pCellEnsembleAttributeMatrixNameValue.createChildPath(EbsdLib::CtfFile::LatticeConstants);
    auto action = std::make_unique<CreateArrayAction>(complex::DataType::float32, tupleDims, cDims, dataArrayPath);
    resultOutputActions.value().actions.push_back(std::move(action));
  }
  // Create the Material Names Array
  {
    cDims[0] = 256;
    DataPath dataArrayPath = pCellEnsembleAttributeMatrixNameValue.createChildPath(EbsdLib::CtfFile::MaterialName);
    auto action = std::make_unique<CreateArrayAction>(complex::DataType::int8, tupleDims, cDims, dataArrayPath);
    resultOutputActions.value().actions.push_back(std::move(action));
  }

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> ReadCtfDataFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                        const std::atomic_bool& shouldCancel) const
{
  ReadCtfDataInputValues inputValues;

  inputValues.InputFile = filterArgs.value<FileSystemPathParameter::ValueType>(k_InputFile_Key);
  inputValues.DegreesToRadians = filterArgs.value<bool>(k_DegreesToRadians_Key);
  inputValues.EdaxHexagonalAlignment = filterArgs.value<bool>(k_EdaxHexagonalAlignment_Key);
  inputValues.DataContainerName = filterArgs.value<DataPath>(k_DataContainerName_Key);
  inputValues.CellAttributeMatrixName = filterArgs.value<DataPath>(k_CellAttributeMatrixName_Key);
  inputValues.CellEnsembleAttributeMatrixName = filterArgs.value<DataPath>(k_CellEnsembleAttributeMatrixName_Key);

   ReadCtfData readCtfData(dataStructure, messageHandler, shouldCancel, &inputValues);
   return readCtfData();
}
} // namespace complex
