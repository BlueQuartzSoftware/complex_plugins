
#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/DataObjectNameParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"

#include "OrientationAnalysis/Filters/VisualizeGBCDPoleFigureFilter.hpp"
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

inline constexpr StringLiteral k_PoleFigureSigma3("GBCD Pole Figure [Sigma 3]");
inline constexpr StringLiteral k_PoleFigureSigma9("GBCD Pole Figure [Sigma 9]");
inline constexpr StringLiteral k_PoleFigureSigma11("GBCD Pole Figure [Sigma 11]");
inline constexpr StringLiteral k_ExemplarCellData("Cell Data");
inline constexpr StringLiteral k_Sigma3("Sigma 3");
inline constexpr StringLiteral k_Sigma9("Sigma 9");
inline constexpr StringLiteral k_Sigma11("Sigma 11");
inline constexpr StringLiteral k_MRD("MRD");

inline constexpr int32 k_Dimension = 300;

} // namespace

TEST_CASE("OrientationAnalysis::VisualizeGBCDPoleFigureFilter", "[OrientationAnalysis][VisualizeGBCDPoleFigureFilter]")
{
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

  DataPath nxPoleFigureSigma3({"NX GBCD Pole Figure [Sigma 3]"});
  DataPath nxPoleFigureSigma9({"NX GBCD Pole Figure [Sigma 9]"});
  DataPath nxPoleFigureSigma11({"NX GBCD Pole Figure [Sigma 11]"});

  // Create the Pole Figures for Sigma 3  60@[111]
  {
    VisualizeGBCDPoleFigureFilter filter;
    Arguments args;

    auto outputFile = fs::path(fmt::format("{}/small_in100_sigma_3.vtk", unit_test::k_BinaryTestOutputDir));

    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_PhaseOfInterest_Key, std::make_any<Int32Parameter::ValueType>(1));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_MisorientationRotation_Key, std::make_any<VectorFloat32Parameter::ValueType>({60.0F, 1.0F, 1.0F, 1.0F}));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_OutputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(outputFile));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_OutputImageDimension_Key, std::make_any<int32>(k_Dimension));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_GBCDArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(gbcdArrayPath));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_CrystalStructuresArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(crystalStructurePath));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_ImageGeometryName_Key, std::make_any<DataGroupCreationParameter::ValueType>(nxPoleFigureSigma3));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_CellAttributeMatrixName_Key, std::make_any<DataObjectNameParameter::ValueType>(Constants::k_CellData));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_CellIntensityArrayName_Key, std::make_any<DataObjectNameParameter::ValueType>(k_Sigma3));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Compare the Output Pole Figure for Sigma 3
  {
    const DataPath k_GeneratedDataPath = nxPoleFigureSigma3.createChildPath(Constants::k_CellData).createChildPath(k_Sigma3);
    const DataPath k_ExemplarArrayPath({k_PoleFigureSigma3, k_ExemplarCellData, k_MRD});

    UnitTest::CompareFloatArraysWithNans<float64>(dataStructure, k_ExemplarArrayPath, k_GeneratedDataPath);
  }

  // Create the Pole Figures for Sigma 9  39@[110]
  {
    VisualizeGBCDPoleFigureFilter filter;
    Arguments args;

    auto outputFile = fs::path(fmt::format("{}/small_in100_sigma_9.vtk", unit_test::k_BinaryTestOutputDir));

    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_PhaseOfInterest_Key, std::make_any<Int32Parameter::ValueType>(1));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_MisorientationRotation_Key, std::make_any<VectorFloat32Parameter::ValueType>({39.0F, 1.0F, 1.0F, 0.0F}));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_OutputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(outputFile));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_OutputImageDimension_Key, std::make_any<int32>(k_Dimension));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_GBCDArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(gbcdArrayPath));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_CrystalStructuresArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(crystalStructurePath));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_ImageGeometryName_Key, std::make_any<DataGroupCreationParameter::ValueType>(nxPoleFigureSigma9));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_CellAttributeMatrixName_Key, std::make_any<DataObjectNameParameter::ValueType>(Constants::k_CellData));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_CellIntensityArrayName_Key, std::make_any<DataObjectNameParameter::ValueType>(k_Sigma9));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Compare the Output Pole Figure for Sigma 9
  {
    const DataPath k_GeneratedDataPath = nxPoleFigureSigma9.createChildPath(Constants::k_CellData).createChildPath(k_Sigma9);
    const DataPath k_ExemplarArrayPath({k_PoleFigureSigma9, k_ExemplarCellData, k_MRD});

    UnitTest::CompareFloatArraysWithNans<float64>(dataStructure, k_ExemplarArrayPath, k_GeneratedDataPath);
  }

  // Create the Pole Figures for Sigma 11  50.5@[110]
  {
    VisualizeGBCDPoleFigureFilter filter;
    Arguments args;

    auto outputFile = fs::path(fmt::format("{}/small_in100_sigma_11.vtk", unit_test::k_BinaryTestOutputDir));

    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_PhaseOfInterest_Key, std::make_any<Int32Parameter::ValueType>(1));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_MisorientationRotation_Key, std::make_any<VectorFloat32Parameter::ValueType>({50.5F, 1.0F, 1.0F, 0.0F}));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_OutputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(outputFile));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_OutputImageDimension_Key, std::make_any<int32>(k_Dimension));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_GBCDArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(gbcdArrayPath));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_CrystalStructuresArrayPath_Key, std::make_any<ArraySelectionParameter::ValueType>(crystalStructurePath));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_ImageGeometryName_Key, std::make_any<DataGroupCreationParameter::ValueType>(nxPoleFigureSigma11));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_CellAttributeMatrixName_Key, std::make_any<DataObjectNameParameter::ValueType>(Constants::k_CellData));
    args.insertOrAssign(VisualizeGBCDPoleFigureFilter::k_CellIntensityArrayName_Key, std::make_any<DataObjectNameParameter::ValueType>(k_Sigma11));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Compare the Output Pole Figure for Sigma 11
  {
    const DataPath k_GeneratedDataPath = nxPoleFigureSigma11.createChildPath(Constants::k_CellData).createChildPath(k_Sigma11);
    const DataPath k_ExemplarArrayPath({k_PoleFigureSigma11, k_ExemplarCellData, k_MRD});

    UnitTest::CompareFloatArraysWithNans<float64>(dataStructure, k_ExemplarArrayPath, k_GeneratedDataPath);
  }

  WriteTestDataStructure(dataStructure, fs::path(fmt::format("{}/gbcd_pole_figure.dream3d", unit_test::k_BinaryTestOutputDir)));
}
