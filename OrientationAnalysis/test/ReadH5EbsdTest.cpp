#include <catch2/catch.hpp>

#include "complex/Core/Application.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/Dream3dImportParameter.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"

#include "OrientationAnalysis/Filters/ReadH5EbsdFilter.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"
#include "OrientationAnalysis/Parameters/H5EbsdReaderParameter.h"

#include <filesystem>
namespace fs = std::filesystem;

using namespace complex;

constexpr float EPSILON = 0.000001;

namespace EbsdLib
{
namespace Ang
{
const std::string ConfidenceIndex("Confidence Index");
const std::string ImageQuality("Image Quality");
} // namespace Ang

namespace EnsembleData
{
inline const std::string CrystalStructures("CrystalStructures");
inline const std::string LatticeConstants("LatticeConstants");
inline const std::string MaterialName("MaterialName");
} // namespace EnsembleData
} // namespace EbsdLib

template <typename T>
void CompareDataArrays(const IDataArray& left, const IDataArray& right)
{
  const auto& oldDataStore = left.getIDataStoreRefAs<AbstractDataStore<T>>();
  const auto& newDataStore = right.getIDataStoreRefAs<AbstractDataStore<T>>();
  usize start = 0;
  usize end = oldDataStore.getSize();
  for(usize i = start; i < end; i++)
  {
    if(oldDataStore[i] != newDataStore[i])
    {
      auto oldVal = oldDataStore[i];
      auto newVal = newDataStore[i];
      float diff = std::fabs(static_cast<float>(oldVal - newVal));
      REQUIRE(diff < EPSILON);
      break;
    }
  }
}

struct make_shared_enabler : public complex::Application
{
};

TEST_CASE("OrientationAnalysis::ReadH5Ebsd: Instantiation and Parameter Check", "[OrientationAnalysis][ReadH5Ebsd]")
{
  ReadH5EbsdFilter filter;
  DataStructure ds;
  Arguments args;

  H5EbsdReaderParameter::ValueType h5ebsdParamVal;

  args.insertOrAssign(ReadH5EbsdFilter::k_ReadH5EbsdFilter_Key, std::make_any<H5EbsdReaderParameter::ValueType>(h5ebsdParamVal));
  args.insertOrAssign(ReadH5EbsdFilter::k_DataContainerName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(ReadH5EbsdFilter::k_CellAttributeMatrixName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(ReadH5EbsdFilter::k_CellEnsembleAttributeMatrixName_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  COMPLEX_RESULT_REQUIRE_INVALID(preflightResult.outputActions);

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  COMPLEX_RESULT_REQUIRE_INVALID(executeResult.result);
}

TEST_CASE("OrientationAnalysis::ReadH5Ebsd: Valid filter execution", "[OrientationAnalysis][ReadH5Ebsd]")
{
  std::shared_ptr<make_shared_enabler> app = std::make_shared<make_shared_enabler>();
  app->loadPlugins(unit_test::k_BuildDir.view(), true);
  auto* filterList = Application::Instance()->getFilterList();

  // Make sure we can load the needed filters from the plugins
  const Uuid k_ComplexCorePluginId = *Uuid::FromString("05cc618b-781f-4ac0-b9ac-43f26ce1854f");
  const Uuid k_ImportDream3dFilterId = *Uuid::FromString("0dbd31c7-19e0-4077-83ef-f4a6459a0e2d");
  const FilterHandle k_ImportDream3dFilterHandle(k_ImportDream3dFilterId, k_ComplexCorePluginId);

  const std::string k_Quats("Quats");
  const std::string k_Phases("Phases");
  const std::string k_ConfidenceIndex = EbsdLib::Ang::ConfidenceIndex;
  const std::string k_ImageQuality = EbsdLib::Ang::ImageQuality;

  const std::string k_Mask("Mask");
  const std::string k_DataContainer("DataContainer");
  const DataPath k_DataContainerPath({k_DataContainer});

  const DataPath k_CalculatedShiftsPath = k_DataContainerPath.createChildPath("Calculated Shifts");

  const DataPath k_CellAttributeMatrix = k_DataContainerPath.createChildPath("CellData");
  const DataPath k_EulersArrayPath = k_CellAttributeMatrix.createChildPath(EbsdLib::CellData::EulerAngles);
  const DataPath k_QuatsArrayPath = k_CellAttributeMatrix.createChildPath(k_Quats);
  const DataPath k_PhasesArrayPath = k_CellAttributeMatrix.createChildPath(k_Phases);
  const DataPath k_ConfidenceIndexArrayPath = k_CellAttributeMatrix.createChildPath(k_ConfidenceIndex);
  const DataPath k_ImageQualityArrayPath = k_CellAttributeMatrix.createChildPath(k_ImageQuality);
  const DataPath k_MaskArrayPath = k_CellAttributeMatrix.createChildPath(k_Mask);

  const DataPath k_CellEnsembleAttributeMatrix = k_DataContainerPath.createChildPath("CellEnsembleData");
  const DataPath k_CrystalStructuresArrayPath = k_CellEnsembleAttributeMatrix.createChildPath(EbsdLib::EnsembleData::CrystalStructures);

  DataStructure exemplarDataStructure;
  // Read Exemplar DREAM3D File Filter
  {
    constexpr StringLiteral k_ImportFileData = "Import_File_Data";

    auto filter = filterList->createFilter(k_ImportDream3dFilterHandle);
    REQUIRE(nullptr != filter);

    Dream3dImportParameter::ImportData parameter;
    parameter.FilePath = fs::path(fmt::format("{}/TestFiles/Small_IN100.dream3d", unit_test::k_DREAM3DDataDir));

    Arguments args;
    args.insertOrAssign(k_ImportFileData, std::make_any<Dream3dImportParameter::ImportData>(parameter));

    // Preflight the filter and check result
    auto preflightResult = filter->preflight(exemplarDataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter->execute(exemplarDataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  DataStructure dataStructure;
  // ReadH5EbsdFilter
  {
    ReadH5EbsdFilter filter;
    Arguments args;

    H5EbsdReaderParameter::ValueType h5ebsdParamVal;
    h5ebsdParamVal.inputFilePath = fmt::format("{}/TestFiles/SmallIN100.h5ebsd", unit_test::k_DREAM3DDataDir);
    h5ebsdParamVal.startSlice = 1;
    h5ebsdParamVal.endSlice = 117;
    h5ebsdParamVal.eulerRepresentation = EbsdLib::AngleRepresentation::Radians;
    h5ebsdParamVal.hdf5DataPaths = {"Confidence Index", "EulerAngles", "Fit", "Image Quality", "Phases", "SEM Signal", "X Position", "Y Position"};
    h5ebsdParamVal.useRecommendedTransform = true;

    args.insertOrAssign(ReadH5EbsdFilter::k_ReadH5EbsdFilter_Key, std::make_any<H5EbsdReaderParameter::ValueType>(h5ebsdParamVal));
    args.insertOrAssign(ReadH5EbsdFilter::k_DataContainerName_Key, std::make_any<DataPath>(k_DataContainerPath));
    args.insertOrAssign(ReadH5EbsdFilter::k_CellAttributeMatrixName_Key, std::make_any<DataPath>(k_CellAttributeMatrix));
    args.insertOrAssign(ReadH5EbsdFilter::k_CellEnsembleAttributeMatrixName_Key, std::make_any<DataPath>(k_CellEnsembleAttributeMatrix));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Loop and compare each array from the 'Exemplar Data / CellData' to the 'Data Container / CellData' group
  {
    auto& cellDataGroup = dataStructure.getDataRefAs<DataGroup>(k_CellAttributeMatrix);
    auto& cellEnsembleDataGroup = dataStructure.getDataRefAs<DataGroup>(k_CellEnsembleAttributeMatrix);
    std::vector<DataPath> selectedArrays;

    // Create the vector of selected cell DataPaths
    for(auto& child : cellDataGroup)
    {
      selectedArrays.push_back(k_CellAttributeMatrix.createChildPath(child.second->getName()));
    }
    for(auto& child : cellEnsembleDataGroup)
    {
      selectedArrays.push_back(k_CellEnsembleAttributeMatrix.createChildPath(child.second->getName()));
    }

    for(const auto& arrayPath : selectedArrays)
    {
      const auto& generatedDataArray = dataStructure.getDataRefAs<IDataArray>(arrayPath);
      DataType type = generatedDataArray.getDataType();
      auto& exemplarDataArray = exemplarDataStructure.getDataRefAs<IDataArray>(arrayPath);
      DataType exemplarType = exemplarDataArray.getDataType();

      if(type != exemplarType)
      {
        std::cout << fmt::format("DataArray {} and {} do not have the same type: {} vs {}. Data Will not be compared.", generatedDataArray.getName(), exemplarDataArray.getName(), type, exemplarType)
                  << std::endl;
        continue;
      }

      switch(type)
      {
      case DataType::boolean: {
        CompareDataArrays<bool>(generatedDataArray, exemplarDataArray);
        break;
      }
      case DataType::int8: {
        CompareDataArrays<int8>(generatedDataArray, exemplarDataArray);
        break;
      }
      case DataType::int16: {
        CompareDataArrays<int16>(generatedDataArray, exemplarDataArray);
        break;
      }
      case DataType::int32: {
        CompareDataArrays<int32>(generatedDataArray, exemplarDataArray);
        break;
      }
      case DataType::int64: {
        CompareDataArrays<int64>(generatedDataArray, exemplarDataArray);
        break;
      }
      case DataType::uint8: {
        CompareDataArrays<uint8>(generatedDataArray, exemplarDataArray);
        break;
      }
      case DataType::uint16: {
        CompareDataArrays<uint16>(generatedDataArray, exemplarDataArray);
        break;
      }
      case DataType::uint32: {
        CompareDataArrays<uint32>(generatedDataArray, exemplarDataArray);
        break;
      }
      case DataType::uint64: {
        CompareDataArrays<uint64>(generatedDataArray, exemplarDataArray);
        break;
      }
      case DataType::float32: {
        CompareDataArrays<float32>(generatedDataArray, exemplarDataArray);
        break;
      }
      case DataType::float64: {
        CompareDataArrays<float64>(generatedDataArray, exemplarDataArray);
        break;
      }
      default: {
        throw std::runtime_error("Invalid DataType");
      }
      }
    }
  }
}
