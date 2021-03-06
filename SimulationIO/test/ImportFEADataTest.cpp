/**
 * This file is auto generated from the original SimulationIO/ImportFEAData
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
 * When you start working on this unit test remove "[ImportFEAData][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"

#include <filesystem>
namespace fs = std::filesystem;

#include "SimulationIO/Filters/ImportFEAData.hpp"
#include "SimulationIO/SimulationIO_test_dirs.hpp"

using namespace complex;

TEST_CASE("SimulationIO::ImportFEAData: Instantiation and Parameter Check", "[SimulationIO][ImportFEAData][.][UNIMPLEMENTED][!mayfail]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  ImportFEAData filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(ImportFEAData::k_odbName_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ImportFEAData::k_odbFilePath_Key, std::make_any<FileSystemPathParameter::ValueType>(fs::path("/Path/To/Output/Directory/To/Read")));
  args.insertOrAssign(ImportFEAData::k_ABQPythonCommand_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ImportFEAData::k_InstanceName_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ImportFEAData::k_Step_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ImportFEAData::k_FrameNumber_Key, std::make_any<int32>(1234356));
  args.insertOrAssign(ImportFEAData::k_BSAMInputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(fs::path("/Path/To/Input/File/To/Read.data")));
  args.insertOrAssign(ImportFEAData::k_DEFORMInputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(fs::path("/Path/To/Input/File/To/Read.data")));
  args.insertOrAssign(ImportFEAData::k_DEFORMPointTrackInputFile_Key, std::make_any<FileSystemPathParameter::ValueType>(fs::path("/Path/To/Input/File/To/Read.data")));
  args.insertOrAssign(ImportFEAData::k_ImportSingleTimeStep_Key, std::make_any<bool>(false));
  args.insertOrAssign(ImportFEAData::k_SingleTimeStepValue_Key, std::make_any<int32>(1234356));
  args.insertOrAssign(ImportFEAData::k_TimeSeriesBundleName_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ImportFEAData::k_DataContainerName_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ImportFEAData::k_VertexAttributeMatrixName_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ImportFEAData::k_CellAttributeMatrixName_Key, std::make_any<StringParameter::ValueType>("SomeString"));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("SimulationIO::ImportFEAData: Valid filter execution")
//{
//
//}

// TEST_CASE("SimulationIO::ImportFEAData: InValid filter execution")
//{
//
//}
