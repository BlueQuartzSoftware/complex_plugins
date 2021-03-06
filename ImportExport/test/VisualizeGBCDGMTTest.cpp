/**
 * This file is auto generated from the original ImportExport/VisualizeGBCDGMT
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
 * When you start working on this unit test remove "[VisualizeGBCDGMT][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

#include <filesystem>
namespace fs = std::filesystem;

#include "ImportExport/Filters/VisualizeGBCDGMT.hpp"
#include "ImportExport/ImportExport_test_dirs.hpp"

using namespace complex;

TEST_CASE("ImportExport::VisualizeGBCDGMT: Instantiation and Parameter Check", "[ImportExport][VisualizeGBCDGMT][.][UNIMPLEMENTED][!mayfail]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  VisualizeGBCDGMT filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(VisualizeGBCDGMT::k_PhaseOfInterest_Key, std::make_any<int32>(1234356));
  args.insertOrAssign(VisualizeGBCDGMT::k_MisorientationRotation_Key, std::make_any<VectorFloat32Parameter::ValueType>(std::vector<float32>(4)));
  args.insertOrAssign(VisualizeGBCDGMT::k_OutputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(fs::path("/Path/To/Output/File/To/Write.data")));
  args.insertOrAssign(VisualizeGBCDGMT::k_GBCDArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(VisualizeGBCDGMT::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("ImportExport::VisualizeGBCDGMT: Valid filter execution")
//{
//
//}

// TEST_CASE("ImportExport::VisualizeGBCDGMT: InValid filter execution")
//{
//
//}
