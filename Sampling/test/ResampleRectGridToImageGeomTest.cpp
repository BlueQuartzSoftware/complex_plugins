/**
 * This file is auto generated from the original Sampling/ResampleRectGridToImageGeom
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
 * When you start working on this unit test remove "[ResampleRectGridToImageGeom][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/DataGroupSelectionParameter.hpp"
#include "complex/Parameters/MultiArraySelectionParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

#include "Sampling/Filters/ResampleRectGridToImageGeom.hpp"
#include "Sampling/Sampling_test_dirs.hpp"

using namespace complex;

TEST_CASE("Sampling::ResampleRectGridToImageGeom: Instantiation and Parameter Check", "[Sampling][ResampleRectGridToImageGeom][.][UNIMPLEMENTED][!mayfail]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  ResampleRectGridToImageGeom filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(ResampleRectGridToImageGeom::k_RectilinearGridPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(ResampleRectGridToImageGeom::k_SelectedDataArrayPaths_Key,
                      std::make_any<MultiArraySelectionParameter::ValueType>(MultiArraySelectionParameter::ValueType{DataPath(), DataPath(), DataPath()}));
  args.insertOrAssign(ResampleRectGridToImageGeom::k_Dimensions_Key, std::make_any<VectorInt32Parameter::ValueType>(std::vector<int32>(3)));
  args.insertOrAssign(ResampleRectGridToImageGeom::k_ImageGeometryPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(ResampleRectGridToImageGeom::k_ImageGeomCellAttributeMatrix_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("Sampling::ResampleRectGridToImageGeom: Valid filter execution")
//{
//
//}

// TEST_CASE("Sampling::ResampleRectGridToImageGeom: InValid filter execution")
//{
//
//}
