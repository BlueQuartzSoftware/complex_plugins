/**
 * This file is auto generated from the original DREAM3DReview/AverageVertexArrayToEdgeFaceCellArray
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
 * When you start working on this unit test remove "[AverageVertexArrayToEdgeFaceCellArray][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"

#include "DREAM3DReview/DREAM3DReview_test_dirs.hpp"
#include "DREAM3DReview/Filters/AverageVertexArrayToEdgeFaceCellArray.hpp"

using namespace complex;

TEST_CASE("DREAM3DReview::AverageVertexArrayToEdgeFaceCellArray: Instantiation and Parameter Check", "[DREAM3DReview][AverageVertexArrayToEdgeFaceCellArray][.][UNIMPLEMENTED][!mayfail]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  AverageVertexArrayToEdgeFaceCellArray filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(AverageVertexArrayToEdgeFaceCellArray::k_WeightedAverage_Key, std::make_any<bool>(false));
  args.insertOrAssign(AverageVertexArrayToEdgeFaceCellArray::k_SelectedArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(AverageVertexArrayToEdgeFaceCellArray::k_AverageCellArrayPath_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("DREAM3DReview::AverageVertexArrayToEdgeFaceCellArray: Valid filter execution")
//{
//
//}

// TEST_CASE("DREAM3DReview::AverageVertexArrayToEdgeFaceCellArray: InValid filter execution")
//{
//
//}
