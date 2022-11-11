#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/DataObjectNameParameter.hpp"

#include "complex_plugins/Utilities/TestUtilities.hpp"


#include "Core/Core_test_dirs.hpp"
#include "Core/Filters/SharedFeatureFaceFilter.hpp"

using namespace complex;

TEST_CASE("Core::SharedFeatureFaceFilter: Instantiation and Parameter Check", "[Core][SharedFeatureFaceFilter]")
{
    // Read Exemplar DREAM3D File Filter
  auto exemplarFilePath = fs::path(fmt::format("{}/TestFiles/12_IN625_GBCD/12_IN625_GBCD.dream3d", unit_test::k_DREAM3DDataDir));
  DataStructure exemplarDataStructure = complex::LoadDataStructure(exemplarFilePath);

  // Instantiate the filter, a DataStructure object and an Arguments Object
  SharedFeatureFaceFilter filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(SharedFeatureFaceFilter::k_TriGeometryDataPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(SharedFeatureFaceFilter::k_FaceLabelsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(SharedFeatureFaceFilter::k_FeatureFaceIdsArrayName_Key, std::make_any<DataObjectNameParameter::ValueType>(""));
  args.insertOrAssign(SharedFeatureFaceFilter::k_GrainBoundaryAttributeMatrixName_Key, std::make_any<DataObjectNameParameter::ValueType>(""));
  args.insertOrAssign(SharedFeatureFaceFilter::k_FeatureNumTrianglesArrayName_Key, std::make_any<DataObjectNameParameter::ValueType>(""));
  args.insertOrAssign(SharedFeatureFaceFilter::k_FeatureFaceLabelsArrayName_Key, std::make_any<DataObjectNameParameter::ValueType>(""));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("SurfaceMeshing::SharedFeatureFaceFilter: Valid filter execution")
//{
//
//}

// TEST_CASE("SurfaceMeshing::SharedFeatureFaceFilter: InValid filter execution")
//{
//
//}
