/**
 * This file is auto generated from the original ImportExport/VtkRectilinearGridWriter
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
 * When you start working on this unit test remove "[VtkRectilinearGridWriter][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/MultiArraySelectionParameter.hpp"

#include <filesystem>
namespace fs = std::filesystem;

#include "ImportExport/Filters/VtkRectilinearGridWriter.hpp"
#include "ImportExport/ImportExport_test_dirs.hpp"

using namespace complex;

TEST_CASE("ImportExport::VtkRectilinearGridWriter: Instantiation and Parameter Check", "[ImportExport][VtkRectilinearGridWriter][.][UNIMPLEMENTED][!mayfail]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  VtkRectilinearGridWriter filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(VtkRectilinearGridWriter::k_OutputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(fs::path("/Path/To/Output/File/To/Write.data")));
  args.insertOrAssign(VtkRectilinearGridWriter::k_WriteBinaryFile_Key, std::make_any<bool>(false));
  args.insertOrAssign(VtkRectilinearGridWriter::k_SelectedDataArrayPaths_Key,
                      std::make_any<MultiArraySelectionParameter::ValueType>(MultiArraySelectionParameter::ValueType{DataPath(), DataPath(), DataPath()}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("ImportExport::VtkRectilinearGridWriter: Valid filter execution")
//{
//
//}

// TEST_CASE("ImportExport::VtkRectilinearGridWriter: InValid filter execution")
//{
//
//}
