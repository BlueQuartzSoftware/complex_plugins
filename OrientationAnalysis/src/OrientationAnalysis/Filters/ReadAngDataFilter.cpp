#include "ReadAngDataFilter.hpp"

#include "OrientationAnalysis/Filters/Algorithms/ReadAngData.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/CreateArrayAction.hpp"
#include "complex/Filter/Actions/CreateDataGroupAction.hpp"
#include "complex/Filter/Actions/CreateImageGeometryAction.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"

#include "EbsdLib/IO/TSL/AngFields.h"
#include "EbsdLib/IO/TSL/AngPhase.h"
#include "EbsdLib/IO/TSL/AngReader.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace complex
{
//------------------------------------------------------------------------------
std::string ReadAngDataFilter::name() const
{
  return FilterTraits<ReadAngDataFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string ReadAngDataFilter::className() const
{
  return FilterTraits<ReadAngDataFilter>::className;
}

//------------------------------------------------------------------------------
Uuid ReadAngDataFilter::uuid() const
{
  return FilterTraits<ReadAngDataFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string ReadAngDataFilter::humanName() const
{
  return "Import EDAX EBSD Data (.ang)";
}

//------------------------------------------------------------------------------
std::vector<std::string> ReadAngDataFilter::defaultTags() const
{
  return {"#IO", "#Input", "#Read", "#Import"};
}

//------------------------------------------------------------------------------
Parameters ReadAngDataFilter::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<FileSystemPathParameter>(k_InputFile_Key, "Input File", "", fs::path("<default file to read goes here>"), FileSystemPathParameter::ExtensionsType{".ang"},
                                                          FileSystemPathParameter::PathType::InputFile));

  params.insertSeparator(Parameters::Separator{"Created Data Structure Objects"});
  params.insert(std::make_unique<DataGroupCreationParameter>(k_DataContainerName_Key, "Created Image Geometry", "", DataPath({"DataContainer"})));
  // params.insertSeparator(Parameters::Separator{"Cell Data"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_CellAttributeMatrixName_Key, "Created Cell Attribute Matrix", "", DataPath({"DataContainer", "CellData"})));
  // params.insertSeparator(Parameters::Separator{"Cell Ensemble Data"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_CellEnsembleAttributeMatrixName_Key, "Created Cell Ensemble Attribute Matrix", "", DataPath({"DataContainer", "CellEnsembleData"})));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ReadAngDataFilter::clone() const
{
  return std::make_unique<ReadAngDataFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ReadAngDataFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                          const std::atomic_bool& shouldCancel) const
{

  auto pInputFileValue = filterArgs.value<FileSystemPathParameter::ValueType>(k_InputFile_Key);
  auto imageGeomPath = filterArgs.value<DataPath>(k_DataContainerName_Key);
  auto pCellAttributeMatrixNameValue = filterArgs.value<DataPath>(k_CellAttributeMatrixName_Key);
  auto pCellEnsembleAttributeMatrixNameValue = filterArgs.value<DataPath>(k_CellEnsembleAttributeMatrixName_Key);

  PreflightResult preflightResult;

  complex::Result<OutputActions> resultOutputActions;

  std::vector<PreflightValue> preflightUpdatedValues;

  AngReader reader;
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

  AngFields angfeatures;
  std::vector<std::string> names = angfeatures.getFilterFeatures<std::vector<std::string>>();
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
    DataPath dataArrayPath = pCellAttributeMatrixNameValue.createChildPath(EbsdLib::AngFile::Phases);
    auto action = std::make_unique<CreateArrayAction>(complex::DataType::int32, tupleDims, cDims, dataArrayPath);
    resultOutputActions.value().actions.push_back(std::move(action));
  }

  // Create the Cell Euler Angles Array
  {
    cDims[0] = 3;
    DataPath dataArrayPath = pCellAttributeMatrixNameValue.createChildPath(EbsdLib::AngFile::EulerAngles);
    auto action = std::make_unique<CreateArrayAction>(complex::DataType::float32, tupleDims, cDims, dataArrayPath);
    resultOutputActions.value().actions.push_back(std::move(action));
  }

  std::vector<std::shared_ptr<AngPhase>> angPhases = reader.getPhaseVector();
  tupleDims = {angPhases.size() + 1}; // Always create 1 extra slot for the phases.
  // Create the Crystal Structures Array
  {
    cDims[0] = 1;
    DataPath dataArrayPath = pCellEnsembleAttributeMatrixNameValue.createChildPath(EbsdLib::AngFile::CrystalStructures);
    auto action = std::make_unique<CreateArrayAction>(complex::DataType::uint32, tupleDims, cDims, dataArrayPath);
    resultOutputActions.value().actions.push_back(std::move(action));
  }
  // Create the Lattice Constants Array
  {
    cDims[0] = 6;
    DataPath dataArrayPath = pCellEnsembleAttributeMatrixNameValue.createChildPath(EbsdLib::AngFile::LatticeConstants);
    auto action = std::make_unique<CreateArrayAction>(complex::DataType::float32, tupleDims, cDims, dataArrayPath);
    resultOutputActions.value().actions.push_back(std::move(action));
  }
  // Create the Material Names Array
  {
    cDims[0] = 256;
    DataPath dataArrayPath = pCellEnsembleAttributeMatrixNameValue.createChildPath(EbsdLib::AngFile::MaterialName);
    auto action = std::make_unique<CreateArrayAction>(complex::DataType::int8, tupleDims, cDims, dataArrayPath);
    resultOutputActions.value().actions.push_back(std::move(action));
  }

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> ReadAngDataFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                        const std::atomic_bool& shouldCancel) const
{

  ReadAngDataInputValues inputValues;

  inputValues.inputFile = filterArgs.value<FileSystemPathParameter::ValueType>(k_InputFile_Key);
  inputValues.dataContainerPath = filterArgs.value<DataPath>(k_DataContainerName_Key);
  inputValues.cellAttributeMatrixPath = filterArgs.value<DataPath>(k_CellAttributeMatrixName_Key);
  inputValues.cellEnsemblePath = filterArgs.value<DataPath>(k_CellEnsembleAttributeMatrixName_Key);

  ReadAngData readAngData(dataStructure, messageHandler, shouldCancel, &inputValues);
  return readAngData();
}
} // namespace complex
