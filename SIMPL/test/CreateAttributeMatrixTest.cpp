/**
 * This file is auto generated from the original Core/CreateAttributeMatrix
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
 * When you start working on this unit test remove "[CreateAttributeMatrix][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/DynamicTableFilterParameter.hpp"

#include "SIMPL/Filters/CreateAttributeMatrix.hpp"
#include "SIMPL/SIMPL_test_dirs.hpp"

using namespace complex;

TEST_CASE("Core::CreateAttributeMatrix: Instantiation and Parameter Check", "[Core][CreateAttributeMatrix][.][UNIMPLEMENTED][!mayfail]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  CreateAttributeMatrix filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(CreateAttributeMatrix::k_AttributeMatrixType_Key, std::make_any<ChoicesParameter::ValueType>(0));
  /*[x]*/ args.insertOrAssign(CreateAttributeMatrix::k_TupleDimensions_Key, std::make_any<<<<NOT_IMPLEMENTED>>>>({}));
  args.insertOrAssign(CreateAttributeMatrix::k_CreatedAttributeMatrix_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("Core::CreateAttributeMatrix: Valid filter execution")
//{
//
//}

// TEST_CASE("Core::CreateAttributeMatrix: InValid filter execution")
//{
//
//}
