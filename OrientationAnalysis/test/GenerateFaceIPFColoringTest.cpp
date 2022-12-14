#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"

#include "OrientationAnalysis/Filters/GenerateFaceIPFColoringFilter.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"

using namespace complex;

TEST_CASE("complex_plugins::GenerateFaceIPFColoringFilter: Instantiation and Parameter Check", "[complex_plugins][GenerateFaceIPFColoringFilter][.][UNIMPLEMENTED][!mayfail]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  GenerateFaceIPFColoringFilter filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(GenerateFaceIPFColoringFilter::k_SurfaceMeshFaceLabelsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(GenerateFaceIPFColoringFilter::k_SurfaceMeshFaceNormalsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(GenerateFaceIPFColoringFilter::k_FeatureEulerAnglesArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(GenerateFaceIPFColoringFilter::k_FeaturePhasesArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(GenerateFaceIPFColoringFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(GenerateFaceIPFColoringFilter::k_SurfaceMeshFaceIPFColorsArrayName_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("complex_plugins::GenerateFaceIPFColoringFilter: Valid filter execution")
//{
//
// }

// TEST_CASE("complex_plugins::GenerateFaceIPFColoringFilter: InValid filter execution")
//{
//
// }
