/**
 * This file is auto generated from the original OrientationAnalysis/FindGBPDMetricBased
 * runtime information. These are the steps that need to be taken to utilize this
 * unit test in the proper way.
 *
 * 1: Validate each of the default parameters that gets created.
 * 2: Inspect the actual filter to determine if the filter in its default state
 * would pass or fail BOTH the preflight() and execute() methods
 * 3: UPDATE the ```REQUIRE(result.result.valid());``` code to have the proper
 *
 * 4: Add additional unit tests to actually test each code path within the filter
 *
 * There are some example Catch2 ```TEST_CASE``` sections for your inspiration.
 *
 * NOTE the format of the ```TEST_CASE``` macro. Please stick to this format to
 * allow easier parsing of the unit tests.
 *
 * When you start working on this unit test remove "[FindGBPDMetricBased][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"

#include <filesystem>
namespace fs = std::filesystem;

#include "OrientationAnalysis/Filters/FindGBPDMetricBased.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"

using namespace complex;

TEST_CASE("OrientationAnalysis::FindGBPDMetricBased: Valid Execution", "[OrientationAnalysis][FindGBPDMetricBased][.][UNIMPLEMENTED][!mayfail]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  FindGBPDMetricBased filter;
  DataStructure dataStructure;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(FindGBPDMetricBased::k_PhaseOfInterest_Key, std::make_any<int32>(1234356));
  args.insertOrAssign(FindGBPDMetricBased::k_LimitDist_Key, std::make_any<float32>(1.23345f));
  args.insertOrAssign(FindGBPDMetricBased::k_NumSamplPts_Key, std::make_any<int32>(1234356));
  args.insertOrAssign(FindGBPDMetricBased::k_ExcludeTripleLines_Key, std::make_any<bool>(false));
  args.insertOrAssign(FindGBPDMetricBased::k_DistOutputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(fs::path("/Path/To/Output/File/To/Write.data")));
  args.insertOrAssign(FindGBPDMetricBased::k_ErrOutputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(fs::path("/Path/To/Output/File/To/Write.data")));
  args.insertOrAssign(FindGBPDMetricBased::k_SaveRelativeErr_Key, std::make_any<bool>(false));
  args.insertOrAssign(FindGBPDMetricBased::k_NodeTypesArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindGBPDMetricBased::k_SurfaceMeshFaceLabelsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindGBPDMetricBased::k_SurfaceMeshFaceNormalsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindGBPDMetricBased::k_SurfaceMeshFaceAreasArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindGBPDMetricBased::k_SurfaceMeshFeatureFaceLabelsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindGBPDMetricBased::k_FeatureEulerAnglesArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindGBPDMetricBased::k_FeaturePhasesArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindGBPDMetricBased::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(dataStructure, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(dataStructure, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("OrientationAnalysis::FindGBPDMetricBased: InValid filter execution")
//{
//
//}
