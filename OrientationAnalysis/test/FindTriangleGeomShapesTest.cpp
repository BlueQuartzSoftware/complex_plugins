
#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/DataGroupSelectionParameter.hpp"

#include "OrientationAnalysis/Filters/FindTriangleGeomShapesFilter.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"
#include "complex_plugins/Utilities/TestUtilities.hpp"


using namespace complex;

TEST_CASE("Core::FindTriangleGeomShapes: Instantiation and Parameter Check", "[Core][FindTriangleGeomShapes]")
{
    // Read Exemplar DREAM3D File Filter
  auto exemplarFilePath = fs::path(fmt::format("{}/TestFiles/12_IN625_GBCD/12_IN625_GBCD.dream3d", unit_test::k_DREAM3DDataDir));
  DataStructure exemplarDataStructure = complex::LoadDataStructure(exemplarFilePath);

  // Instantiate the filter, a DataStructure object and an Arguments Object
  FindTriangleGeomShapesFilter filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(FindTriangleGeomShapesFilter::k_FaceLabelsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindTriangleGeomShapesFilter::k_FeatureAttributeMatrixName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindTriangleGeomShapesFilter::k_CentroidsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindTriangleGeomShapesFilter::k_VolumesArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindTriangleGeomShapesFilter::k_Omega3sArrayName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindTriangleGeomShapesFilter::k_AxisLengthsArrayName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindTriangleGeomShapesFilter::k_AxisEulerAnglesArrayName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(FindTriangleGeomShapesFilter::k_AspectRatiosArrayName_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}
