/**
 * This file is auto generated from the original SimulationIO/ExportMultiOnScaleTableFile
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
 * When you start working on this unit test remove "[ExportMultiOnScaleTableFile][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

#include <filesystem>
namespace fs = std::filesystem;

#include "SimulationIO/Filters/ExportMultiOnScaleTableFile.hpp"
#include "SimulationIO/SimulationIO_test_dirs.hpp"

using namespace complex;

TEST_CASE("SimulationIO::ExportMultiOnScaleTableFile: Instantiation and Parameter Check", "[SimulationIO][ExportMultiOnScaleTableFile][.][UNIMPLEMENTED][!mayfail]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  ExportMultiOnScaleTableFile filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(ExportMultiOnScaleTableFile::k_OutputPath_Key, std::make_any<FileSystemPathParameter::ValueType>(fs::path("/Path/To/Output/Directory/To/Read")));
  args.insertOrAssign(ExportMultiOnScaleTableFile::k_DataContainerPrefix_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ExportMultiOnScaleTableFile::k_MatrixName_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ExportMultiOnScaleTableFile::k_ArrayName_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ExportMultiOnScaleTableFile::k_NumKeypoints_Key, std::make_any<VectorInt32Parameter::ValueType>(std::vector<int32>(3)));
  args.insertOrAssign(ExportMultiOnScaleTableFile::k_PhaseNamesArrayPath_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("SimulationIO::ExportMultiOnScaleTableFile: Valid filter execution")
//{
//
//}

// TEST_CASE("SimulationIO::ExportMultiOnScaleTableFile: InValid filter execution")
//{
//
//}
