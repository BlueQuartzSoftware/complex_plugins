
#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"

#include "Core/Core_test_dirs.hpp"
#include "Core/Filters/FindSurfaceAreaToVolumeFilter.hpp"

using namespace complex;

TEST_CASE("Core::FindSurfaceAreaToVolume: Instantiation and Parameter Check", "[Core][FindSurfaceAreaToVolume]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  FindSurfaceAreaToVolumeFilter filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(FindSurfaceAreaToVolumeFilter::k_FeatureIdsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindSurfaceAreaToVolumeFilter::k_NumCellsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindSurfaceAreaToVolumeFilter::k_SurfaceAreaVolumeRatioArrayName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindSurfaceAreaToVolumeFilter::k_CalculateSphericity_Key, std::make_any<bool>(false));
  args.insertOrAssign(FindSurfaceAreaToVolumeFilter::k_SphericityArrayName_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}
