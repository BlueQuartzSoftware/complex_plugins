#include <catch2/catch.hpp>

#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"

#include "complex_plugins/Utilities/SmallIN100Utilities.hpp"

#include "OrientationAnalysis/Filters/AlignSectionsMutualInformationFilter.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace complex;

TEST_CASE("OrientationAnalysis::AlignSectionsMutualInformationFilter: Valid filter execution")
{
  std::shared_ptr<UnitTest::make_shared_enabler> app = std::make_shared<UnitTest::make_shared_enabler>();
  app->loadPlugins(unit_test::k_BuildDir.view(), true);
  auto* filterList = Application::Instance()->getFilterList();

  // Read Exemplar DREAM3D File Filter
  auto exemplarFilePath = fs::path(fmt::format("{}/6_6_align_sections_mutual_information/6_6_align_sections_mutual_information.dream3d", unit_test::k_TestFilesDir));
  DataStructure exemplarDataStructure = UnitTest::LoadDataStructure(exemplarFilePath);

  // Read the Small IN100 Data set
  auto baseDataFilePath = fs::path(fmt::format("{}/Small_IN100.dream3d", unit_test::k_TestFilesDir));
  DataStructure dataStructure = UnitTest::LoadDataStructure(baseDataFilePath);

  // MultiThreshold Objects Filter (From ComplexCore Plugin)
  SmallIn100::ExecuteMultiThresholdObjects(dataStructure, *filterList);

  // Convert Orientation Representation Filter (From OrientationAnalysis Plugin)
  SmallIn100::ExecuteConvertOrientations(dataStructure, *filterList);

  // Align Sections Mutual Information Filter
  {
    // Instantiate the filter, a DataStructure object and an Arguments Object
    AlignSectionsMutualInformationFilter filter;
    Arguments args;

    // Create valid Parameters for the filter.
    args.insertOrAssign(AlignSectionsMutualInformationFilter::k_WriteAlignmentShifts_Key, std::make_any<bool>(true));
    args.insertOrAssign(AlignSectionsMutualInformationFilter::k_AlignmentShiftFileName_Key,
                        std::make_any<FileSystemPathParameter::ValueType>(fs::path(fmt::format("{}/AlignSectionsMutualInformation_1.txt", unit_test::k_BinaryDir))));
    args.insertOrAssign(AlignSectionsMutualInformationFilter::k_MisorientationTolerance_Key, std::make_any<float32>(5.0f));
    args.insertOrAssign(AlignSectionsMutualInformationFilter::k_UseGoodVoxels_Key, std::make_any<bool>(true));
    args.insertOrAssign(AlignSectionsMutualInformationFilter::k_SelectedImageGeometry_Key, std::make_any<DataPath>(Constants::k_DataContainerPath));
    args.insertOrAssign(AlignSectionsMutualInformationFilter::k_QuatsArrayPath_Key, std::make_any<DataPath>(Constants::k_QuatsArrayPath));
    args.insertOrAssign(AlignSectionsMutualInformationFilter::k_CellPhasesArrayPath_Key, std::make_any<DataPath>(Constants::k_PhasesArrayPath));
    args.insertOrAssign(AlignSectionsMutualInformationFilter::k_GoodVoxelsArrayPath_Key, std::make_any<DataPath>(Constants::k_MaskArrayPath));
    args.insertOrAssign(AlignSectionsMutualInformationFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(Constants::k_CrystalStructuresArrayPath));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  UnitTest::CompareExemplarToGeneratedData(dataStructure, exemplarDataStructure, Constants::k_CellAttributeMatrix, Constants::k_SmallIN100);
}

TEST_CASE("OrientationAnalysis::AlignSectionsMutualInformationFilter: InValid filter execution")
{
  // Read the Small IN100 Data set
  auto baseDataFilePath = fs::path(fmt::format("{}/6_6_stats_test.dream3d", unit_test::k_TestFilesDir));
  DataStructure dataStructure = UnitTest::LoadDataStructure(baseDataFilePath);
  // Instantiate the filter and an Arguments Object
  AlignSectionsMutualInformationFilter filter;
  Arguments args;
  args.insertOrAssign(AlignSectionsMutualInformationFilter::k_WriteAlignmentShifts_Key, std::make_any<bool>(true));
  args.insertOrAssign(AlignSectionsMutualInformationFilter::k_AlignmentShiftFileName_Key,
                      std::make_any<FileSystemPathParameter::ValueType>(fs::path(fmt::format("{}/AlignSectionsMutualInformation_2.txt", unit_test::k_BinaryDir))));
  args.insertOrAssign(AlignSectionsMutualInformationFilter::k_MisorientationTolerance_Key, std::make_any<float32>(5.0f));
  args.insertOrAssign(AlignSectionsMutualInformationFilter::k_UseGoodVoxels_Key, std::make_any<bool>(true));
  args.insertOrAssign(AlignSectionsMutualInformationFilter::k_SelectedImageGeometry_Key, std::make_any<DataPath>(Constants::k_DataContainerPath));
  args.insertOrAssign(AlignSectionsMutualInformationFilter::k_QuatsArrayPath_Key, std::make_any<DataPath>(Constants::k_QuatsArrayPath));
  args.insertOrAssign(AlignSectionsMutualInformationFilter::k_CellPhasesArrayPath_Key, std::make_any<DataPath>(Constants::k_PhasesArrayPath));
  args.insertOrAssign(AlignSectionsMutualInformationFilter::k_GoodVoxelsArrayPath_Key,
                      std::make_any<DataPath>(DataPath({Constants::k_DataContainer, Constants::k_CellFeatureData, Constants::k_ActiveName})));
  args.insertOrAssign(AlignSectionsMutualInformationFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(Constants::k_CrystalStructuresArrayPath));

  SECTION("Mismatching cell data tuples")
  {
    args.insertOrAssign(AlignSectionsMutualInformationFilter::k_GoodVoxelsArrayPath_Key,
                        std::make_any<DataPath>(DataPath({Constants::k_DataContainer, Constants::k_CellFeatureData, Constants::k_ActiveName})));
  }

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_INVALID(preflightResult.outputActions);
  REQUIRE(preflightResult.outputActions.errors()[0].code == -3542);
}
