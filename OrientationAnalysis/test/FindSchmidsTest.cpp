/**
 * This file is auto generated from the original OrientationAnalysis/FindSchmidsFilter
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
 * When you start working on this unit test remove "[FindSchmidsFilter][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

#include "OrientationAnalysis/Filters/FindSchmidsFilter.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"

using namespace complex;

TEST_CASE("OrientationAnalysis::FindSchmidsFilter: Instantiation and Parameter Check", "[OrientationAnalysis][FindSchmidsFilter]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  FindSchmidsFilter filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(FindSchmidsFilter::k_LoadingDirection_Key, std::make_any<VectorFloat32Parameter::ValueType>(std::vector<float32>(3)));
  args.insertOrAssign(FindSchmidsFilter::k_StoreAngleComponents_Key, std::make_any<bool>(false));
  args.insertOrAssign(FindSchmidsFilter::k_OverrideSystem_Key, std::make_any<bool>(false));
  args.insertOrAssign(FindSchmidsFilter::k_SlipPlane_Key, std::make_any<VectorFloat32Parameter::ValueType>(std::vector<float32>(3)));
  args.insertOrAssign(FindSchmidsFilter::k_SlipDirection_Key, std::make_any<VectorFloat32Parameter::ValueType>(std::vector<float32>(3)));
  args.insertOrAssign(FindSchmidsFilter::k_FeaturePhasesArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindSchmidsFilter::k_AvgQuatsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindSchmidsFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindSchmidsFilter::k_SchmidsArrayName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindSchmidsFilter::k_SlipSystemsArrayName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindSchmidsFilter::k_PolesArrayName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindSchmidsFilter::k_PhisArrayName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindSchmidsFilter::k_LambdasArrayName_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("OrientationAnalysis::FindSchmidsFilter: Valid filter execution")
//{
//
//}

// TEST_CASE("OrientationAnalysis::FindSchmidsFilter: InValid filter execution")
//{
//
//}