
#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/DataObjectNameParameter.hpp"
#include "complex/Parameters/DynamicTableParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"
#include "complex/Parameters/util/CSVWizardData.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"

#include "OrientationAnalysis/Filters/FindGBCDFilter.hpp"
#include "OrientationAnalysis/Filters/VisualizeGBCDGMTFilter.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"

#include "complex_plugins/Utilities/TestUtilities.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace complex;
using namespace complex::UnitTest;

namespace
{
inline constexpr StringLiteral k_Grain_Data("Grain Data");
inline constexpr StringLiteral k_Phase_Data("Phase Data");
inline constexpr StringLiteral k_TriangleDataContainerName("TriangleDataContainer");
inline constexpr StringLiteral k_FaceData("FaceData");
inline constexpr StringLiteral k_FaceEnsembleDataPath("FaceEnsembleData [NX]");
inline constexpr StringLiteral k_GBCD_Name("GBCD");

inline constexpr StringLiteral k_FaceLabels("FaceLabels");
inline constexpr StringLiteral k_FaceNormals("FaceNormals");
inline constexpr StringLiteral k_FaceAreas("FaceAreas");

inline constexpr StringLiteral k_AvgEulerAngles("AvgEulerAngles");

inline constexpr StringLiteral k_CrystalStructures("CrystalStructures");

inline constexpr StringLiteral k_WizardData_Key = "wizard_data";
inline constexpr StringLiteral k_TupleDims_Key = "tuple_dimensions";
inline constexpr StringLiteral k_UseExistingGroup_Key = "use_existing_group";
inline constexpr StringLiteral k_SelectedDataGroup_Key = "selected_data_group";
inline constexpr StringLiteral k_CreatedDataGroup_Key = "created_data_group";

inline constexpr StringLiteral k_ExemplarGMT1("ExemplarGMT1");
inline constexpr StringLiteral k_ExemplarGMT2("ExemplarGMT2");
inline constexpr StringLiteral k_ExemplarGMT3("ExemplarGMT3");
inline constexpr StringLiteral k_GMT1("GMT1");
inline constexpr StringLiteral k_GMT2("GMT2");
inline constexpr StringLiteral k_GMT3("GMT3");

} // namespace

TEST_CASE("OrientationAnalysis::VisualizeGBCDGMTFilter", "[OrientationAnalysis][VisualizeGBCDGMT]")
{
  std::shared_ptr<make_shared_enabler> app = std::make_shared<make_shared_enabler>();
  app->loadPlugins(unit_test::k_BuildDir.view(), true);
  auto* filterList = Application::Instance()->getFilterList();

  // Read the Small IN100 Data set
  auto baseDataFilePath = fs::path(fmt::format("{}/TestFiles/6_6_Small_IN100_GBCD.dream3d", unit_test::k_DREAM3DDataDir));
  DataStructure dataStructure = UnitTest::LoadDataStructure(baseDataFilePath);
  DataPath smallIn100Group({complex::Constants::k_SmallIN100});
  DataPath featureDataPath = smallIn100Group.createChildPath(::k_Grain_Data);
  DataPath avgEulerAnglesPath = featureDataPath.createChildPath(::k_AvgEulerAngles);
  DataPath featurePhasesPath = featureDataPath.createChildPath(k_Phases);

  DataPath ensembleDataPath = smallIn100Group.createChildPath(::k_Phase_Data);
  DataPath crystalStructurePath = ensembleDataPath.createChildPath(::k_CrystalStructures);

  DataPath triangleDataContainerPath({::k_TriangleDataContainerName});
  DataPath faceDataGroup = triangleDataContainerPath.createChildPath(k_FaceData);
  DataPath faceEnsemblePath = triangleDataContainerPath.createChildPath(k_FaceEnsembleDataPath);

  DataPath faceLabels = faceDataGroup.createChildPath(k_FaceLabels);
  DataPath faceNormals = faceDataGroup.createChildPath(k_FaceNormals);
  DataPath faceAreas = faceDataGroup.createChildPath(k_FaceAreas);

  DataPath gbcdArrayPath = triangleDataContainerPath.createChildPath("FaceEnsembleData").createChildPath(k_GBCD_Name);

  DataPath gmtGroupPath = triangleDataContainerPath.createChildPath("GMTResults");

  SECTION("Pole Figures for Sigma 3  60@[111]")
  {
    // Create Pole Figure
    auto outputFile = fs::path(fmt::format("{}/small_in100_sigma3_1.dat", unit_test::k_BinaryTestOutputDir));
    {
      VisualizeGBCDGMTFilter gmtFilter;
      Arguments args;

      args.insertOrAssign(VisualizeGBCDGMTFilter::k_PhaseOfInterest_Key, std::make_any<Int32Parameter::ValueType>(1));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_MisorientationRotation_Key, std::make_any<VectorFloat32Parameter::ValueType>({60.0F, 1.0F, 1.0F, 1.0F}));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_OutputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(outputFile));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_GBCDArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(gbcdArrayPath));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_CrystalStructuresArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(crystalStructurePath));

      // Preflight the filter and check result
      auto preflightResult = gmtFilter.preflight(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

      // Execute the filter and check the result
      auto executeResult = gmtFilter.execute(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
    }

    // Compare the Output Pole Figure
    auto importDataFilter = filterList->createFilter(k_ImportCSVDataFilterHandle);
    REQUIRE(nullptr != importDataFilter);

    {
      Arguments args;
      CSVWizardData data;
      data.inputFilePath = fmt::format("{}/TestFiles/6_6_Small_IN100_GBCD_Sigma3_1.dat", unit_test::k_DREAM3DDataDir);
      data.dataHeaders = {k_ExemplarGMT1, k_ExemplarGMT2, k_ExemplarGMT3};
      data.dataTypes = {DataType::float32, DataType::float32, DataType::float32};
      data.beginIndex = 2;
      data.spaceAsDelimiter = true;
      data.delimiters = {' '};
      data.numberOfLines = 3752;

      args.insertOrAssign(k_WizardData_Key, std::make_any<CSVWizardData>(data));
      args.insertOrAssign(k_TupleDims_Key, std::make_any<DynamicTableParameter::ValueType>(DynamicTableInfo::TableDataType{{static_cast<float64>(3751)}}));
      args.insertOrAssign(k_UseExistingGroup_Key, std::make_any<bool>(false));
      args.insertOrAssign(k_CreatedDataGroup_Key, std::make_any<DataPath>(gmtGroupPath));
      args.insertOrAssign(k_SelectedDataGroup_Key, std::make_any<DataPath>(faceEnsemblePath));

      auto executeResult = importDataFilter->execute(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
    }

    {
      Arguments args;
      CSVWizardData data;
      data.inputFilePath = outputFile.string();
      data.dataHeaders = {k_GMT1, k_GMT2, k_GMT3};
      data.dataTypes = {DataType::float32, DataType::float32, DataType::float32};
      data.beginIndex = 2;
      data.spaceAsDelimiter = true;
      data.delimiters = {' '};
      data.numberOfLines = 3752;

      args.insertOrAssign(k_WizardData_Key, std::make_any<CSVWizardData>(data));
      args.insertOrAssign(k_TupleDims_Key, std::make_any<DynamicTableParameter::ValueType>(DynamicTableInfo::TableDataType{{static_cast<float64>(3751)}}));
      args.insertOrAssign(k_UseExistingGroup_Key, std::make_any<bool>(true));
      args.insertOrAssign(k_CreatedDataGroup_Key, std::make_any<DataPath>(gmtGroupPath));
      args.insertOrAssign(k_SelectedDataGroup_Key, std::make_any<DataPath>(gmtGroupPath));

      auto executeResult = importDataFilter->execute(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
    }

    DataPath gmt1ArrayPath = gmtGroupPath.createChildPath(k_GMT1);
    DataPath gmt2ArrayPath = gmtGroupPath.createChildPath(k_GMT2);
    DataPath gmt3ArrayPath = gmtGroupPath.createChildPath(k_GMT3);
    DataPath exemplarGmt1ArrayPath = gmtGroupPath.createChildPath(k_ExemplarGMT1);
    DataPath exemplarGmt2ArrayPath = gmtGroupPath.createChildPath(k_ExemplarGMT2);
    DataPath exemplarGmt3ArrayPath = gmtGroupPath.createChildPath(k_ExemplarGMT3);
    REQUIRE(dataStructure.getDataAs<Float32Array>(gmt1ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(gmt2ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(gmt3ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(exemplarGmt1ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(exemplarGmt2ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(exemplarGmt3ArrayPath) != nullptr);

    UnitTest::CompareFloatArraysWithNans<float32>(dataStructure, exemplarGmt1ArrayPath, gmt1ArrayPath);
    UnitTest::CompareFloatArraysWithNans<float32>(dataStructure, exemplarGmt2ArrayPath, gmt2ArrayPath);
    UnitTest::CompareFloatArraysWithNans<float32>(dataStructure, exemplarGmt3ArrayPath, gmt3ArrayPath);
  }

  SECTION("Pole Figures for Sigma 9  39@[110]")
  {
    // Create Pole Figure
    auto outputFile = fs::path(fmt::format("{}/small_in100_sigma_9_1.dat", unit_test::k_BinaryTestOutputDir));

    {
      VisualizeGBCDGMTFilter filter;
      Arguments args;

      args.insertOrAssign(VisualizeGBCDGMTFilter::k_PhaseOfInterest_Key, std::make_any<Int32Parameter::ValueType>(1));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_MisorientationRotation_Key, std::make_any<VectorFloat32Parameter::ValueType>({39.0F, 1.0F, 1.0F, 0.0F}));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_OutputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(outputFile));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_GBCDArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(gbcdArrayPath));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_CrystalStructuresArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(crystalStructurePath));

      // Preflight the filter and check result
      auto preflightResult = filter.preflight(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

      // Execute the filter and check the result
      auto executeResult = filter.execute(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
    }

    // Compare the Output Pole Figure
    auto importDataFilter = filterList->createFilter(k_ImportCSVDataFilterHandle);
    REQUIRE(nullptr != importDataFilter);

    {
      Arguments args;
      CSVWizardData data;
      data.inputFilePath = fmt::format("{}/TestFiles/6_6_Small_IN100_GBCD_Sigma9_1.dat", unit_test::k_DREAM3DDataDir);
      data.dataHeaders = {k_ExemplarGMT1, k_ExemplarGMT2, k_ExemplarGMT3};
      data.dataTypes = {DataType::float32, DataType::float32, DataType::float32};
      data.beginIndex = 2;
      data.spaceAsDelimiter = true;
      data.delimiters = {' '};
      data.numberOfLines = 3752;

      args.insertOrAssign(k_WizardData_Key, std::make_any<CSVWizardData>(data));
      args.insertOrAssign(k_TupleDims_Key, std::make_any<DynamicTableParameter::ValueType>(DynamicTableInfo::TableDataType{{static_cast<float64>(3751)}}));
      args.insertOrAssign(k_UseExistingGroup_Key, std::make_any<bool>(false));
      args.insertOrAssign(k_CreatedDataGroup_Key, std::make_any<DataPath>(gmtGroupPath));
      args.insertOrAssign(k_SelectedDataGroup_Key, std::make_any<DataPath>(faceEnsemblePath));

      auto executeResult = importDataFilter->execute(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
    }

    {
      Arguments args;
      CSVWizardData data;
      data.inputFilePath = outputFile.string();
      data.dataHeaders = {k_GMT1, k_GMT2, k_GMT3};
      data.dataTypes = {DataType::float32, DataType::float32, DataType::float32};
      data.beginIndex = 2;
      data.spaceAsDelimiter = true;
      data.delimiters = {' '};
      data.numberOfLines = 3752;

      args.insertOrAssign(k_WizardData_Key, std::make_any<CSVWizardData>(data));
      args.insertOrAssign(k_TupleDims_Key, std::make_any<DynamicTableParameter::ValueType>(DynamicTableInfo::TableDataType{{static_cast<float64>(3751)}}));
      args.insertOrAssign(k_UseExistingGroup_Key, std::make_any<bool>(true));
      args.insertOrAssign(k_CreatedDataGroup_Key, std::make_any<DataPath>(gmtGroupPath));
      args.insertOrAssign(k_SelectedDataGroup_Key, std::make_any<DataPath>(gmtGroupPath));

      auto executeResult = importDataFilter->execute(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
    }

    DataPath gmt1ArrayPath = gmtGroupPath.createChildPath(k_GMT1);
    DataPath gmt2ArrayPath = gmtGroupPath.createChildPath(k_GMT2);
    DataPath gmt3ArrayPath = gmtGroupPath.createChildPath(k_GMT3);
    DataPath exemplarGmt1ArrayPath = gmtGroupPath.createChildPath(k_ExemplarGMT1);
    DataPath exemplarGmt2ArrayPath = gmtGroupPath.createChildPath(k_ExemplarGMT2);
    DataPath exemplarGmt3ArrayPath = gmtGroupPath.createChildPath(k_ExemplarGMT3);
    REQUIRE(dataStructure.getDataAs<Float32Array>(gmt1ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(gmt2ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(gmt3ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(exemplarGmt1ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(exemplarGmt2ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(exemplarGmt3ArrayPath) != nullptr);

    UnitTest::CompareFloatArraysWithNans<float32>(dataStructure, exemplarGmt1ArrayPath, gmt1ArrayPath);
    UnitTest::CompareFloatArraysWithNans<float32>(dataStructure, exemplarGmt2ArrayPath, gmt2ArrayPath);
    UnitTest::CompareFloatArraysWithNans<float32>(dataStructure, exemplarGmt3ArrayPath, gmt3ArrayPath);
  }

  SECTION("Pole Figures for Sigma 11  50.5@[110]")
  {
    // Create Pole Figure
    auto outputFile = fs::path(fmt::format("{}/small_in100_sigma_11_1.dat", unit_test::k_BinaryTestOutputDir));

    {
      VisualizeGBCDGMTFilter filter;
      Arguments args;

      args.insertOrAssign(VisualizeGBCDGMTFilter::k_PhaseOfInterest_Key, std::make_any<Int32Parameter::ValueType>(1));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_MisorientationRotation_Key, std::make_any<VectorFloat32Parameter::ValueType>({50.5F, 1.0F, 1.0F, 0.0F}));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_OutputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(outputFile));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_GBCDArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(gbcdArrayPath));
      args.insertOrAssign(VisualizeGBCDGMTFilter::k_CrystalStructuresArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(crystalStructurePath));

      // Preflight the filter and check result
      auto preflightResult = filter.preflight(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

      // Execute the filter and check the result
      auto executeResult = filter.execute(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
    }

    // Compare the Output Pole Figure
    auto importDataFilter = filterList->createFilter(k_ImportCSVDataFilterHandle);
    REQUIRE(nullptr != importDataFilter);

    {
      Arguments args;
      CSVWizardData data;
      data.inputFilePath = fmt::format("{}/TestFiles/6_6_Small_IN100_GBCD_Sigma11_1.dat", unit_test::k_DREAM3DDataDir);
      data.dataHeaders = {k_ExemplarGMT1, k_ExemplarGMT2, k_ExemplarGMT3};
      data.dataTypes = {DataType::float32, DataType::float32, DataType::float32};
      data.beginIndex = 2;
      data.spaceAsDelimiter = true;
      data.delimiters = {' '};
      data.numberOfLines = 3752;

      args.insertOrAssign(k_WizardData_Key, std::make_any<CSVWizardData>(data));
      args.insertOrAssign(k_TupleDims_Key, std::make_any<DynamicTableParameter::ValueType>(DynamicTableInfo::TableDataType{{static_cast<float64>(3751)}}));
      args.insertOrAssign(k_UseExistingGroup_Key, std::make_any<bool>(false));
      args.insertOrAssign(k_CreatedDataGroup_Key, std::make_any<DataPath>(gmtGroupPath));
      args.insertOrAssign(k_SelectedDataGroup_Key, std::make_any<DataPath>(faceEnsemblePath));

      auto executeResult = importDataFilter->execute(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
    }

    {
      Arguments args;
      CSVWizardData data;
      data.inputFilePath = outputFile.string();
      data.dataHeaders = {k_GMT1, k_GMT2, k_GMT3};
      data.dataTypes = {DataType::float32, DataType::float32, DataType::float32};
      data.beginIndex = 2;
      data.spaceAsDelimiter = true;
      data.delimiters = {' '};
      data.numberOfLines = 3752;

      args.insertOrAssign(k_WizardData_Key, std::make_any<CSVWizardData>(data));
      args.insertOrAssign(k_TupleDims_Key, std::make_any<DynamicTableParameter::ValueType>(DynamicTableInfo::TableDataType{{static_cast<float64>(3751)}}));
      args.insertOrAssign(k_UseExistingGroup_Key, std::make_any<bool>(true));
      args.insertOrAssign(k_CreatedDataGroup_Key, std::make_any<DataPath>(gmtGroupPath));
      args.insertOrAssign(k_SelectedDataGroup_Key, std::make_any<DataPath>(gmtGroupPath));

      auto executeResult = importDataFilter->execute(dataStructure, args);
      COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
    }

    DataPath gmt1ArrayPath = gmtGroupPath.createChildPath(k_GMT1);
    DataPath gmt2ArrayPath = gmtGroupPath.createChildPath(k_GMT2);
    DataPath gmt3ArrayPath = gmtGroupPath.createChildPath(k_GMT3);
    DataPath exemplarGmt1ArrayPath = gmtGroupPath.createChildPath(k_ExemplarGMT1);
    DataPath exemplarGmt2ArrayPath = gmtGroupPath.createChildPath(k_ExemplarGMT2);
    DataPath exemplarGmt3ArrayPath = gmtGroupPath.createChildPath(k_ExemplarGMT3);
    REQUIRE(dataStructure.getDataAs<Float32Array>(gmt1ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(gmt2ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(gmt3ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(exemplarGmt1ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(exemplarGmt2ArrayPath) != nullptr);
    REQUIRE(dataStructure.getDataAs<Float32Array>(exemplarGmt3ArrayPath) != nullptr);

    UnitTest::CompareFloatArraysWithNans<float32>(dataStructure, exemplarGmt1ArrayPath, gmt1ArrayPath);
    UnitTest::CompareFloatArraysWithNans<float32>(dataStructure, exemplarGmt2ArrayPath, gmt2ArrayPath);
    UnitTest::CompareFloatArraysWithNans<float32>(dataStructure, exemplarGmt3ArrayPath, gmt3ArrayPath);
  }

  WriteTestDataStructure(dataStructure, fs::path(fmt::format("{}/gbcd_gmt.dream3d", unit_test::k_BinaryTestOutputDir)));
}