#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"

#include "OrientationAnalysis/Filters/GenerateFaceMisorientationColoringFilter.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"

#include "complex_plugins/Utilities/TestUtilities.hpp"

using namespace complex;
using namespace complex::UnitTest;

namespace
{
inline constexpr StringLiteral k_FaceMisorientationColors("SurfaceMeshFaceMisorientationColors");
inline constexpr StringLiteral k_NXFaceMisorientationColors("NXFaceMisorientationColors");

DataPath smallIn100Group({complex::Constants::k_SmallIN100});
DataPath featureDataPath = smallIn100Group.createChildPath(complex::Constants::k_Grain_Data);
DataPath avgEulerAnglesPath = featureDataPath.createChildPath(complex::Constants::k_AvgEulerAngles);
DataPath featurePhasesPath = featureDataPath.createChildPath(complex::Constants::k_Phases);
DataPath crystalStructurePath = smallIn100Group.createChildPath(complex::Constants::k_Phase_Data).createChildPath(complex::Constants::k_CrystalStructures);
DataPath avgQuatsPath = featureDataPath.createChildPath("AvgQuats");

DataPath triangleDataContainerPath({complex::Constants::k_TriangleDataContainerName});
DataPath faceDataGroup = triangleDataContainerPath.createChildPath(complex::Constants::k_FaceData);

DataPath faceLabels = faceDataGroup.createChildPath(complex::Constants::k_FaceLabels);
DataPath faceNormals = faceDataGroup.createChildPath(complex::Constants::k_FaceNormals);
DataPath faceAreas = faceDataGroup.createChildPath(complex::Constants::k_FaceAreas);
} // namespace

TEST_CASE("OrientationAnalysis::GenerateFaceMisorientationColoringFilter: Valid filter execution", "[OrientationAnalysis][GenerateFaceMisorientationColoringFilter]")
{
  // Read the Small IN100 Data set
  auto baseDataFilePath = fs::path(fmt::format("{}/TestFiles/GBCD/6_6_Small_IN100_GBCD.dream3d", unit_test::k_DREAM3DDataDir));
  DataStructure dataStructure = UnitTest::LoadDataStructure(baseDataFilePath);

  // Instantiate the filter, a DataStructure object and an Arguments Object
  GenerateFaceMisorientationColoringFilter filter;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(GenerateFaceMisorientationColoringFilter::k_SurfaceMeshFaceLabelsArrayPath_Key, std::make_any<DataPath>(faceLabels));
  args.insertOrAssign(GenerateFaceMisorientationColoringFilter::k_AvgQuatsArrayPath_Key, std::make_any<DataPath>(avgQuatsPath));
  args.insertOrAssign(GenerateFaceMisorientationColoringFilter::k_FeaturePhasesArrayPath_Key, std::make_any<DataPath>(featurePhasesPath));
  args.insertOrAssign(GenerateFaceMisorientationColoringFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(crystalStructurePath));
  args.insertOrAssign(GenerateFaceMisorientationColoringFilter::k_SurfaceMeshFaceMisorientationColorsArrayName_Key, std::make_any<std::string>(::k_NXFaceMisorientationColors));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

  // Execute the filter and check the result
  auto executeResult = filter.execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);

  // compare the resulting face IPF Colors array
  DataPath exemplarPath = faceDataGroup.createChildPath(::k_FaceMisorientationColors);
  DataPath generatedPath = faceDataGroup.createChildPath(::k_NXFaceMisorientationColors);
  CompareArrays<float32>(dataStructure, exemplarPath, generatedPath);
}

TEST_CASE("OrientationAnalysis::GenerateFaceMisorientationColoringFilter: Invalid filter execution", "[OrientationAnalysis][GenerateFaceMisorientationColoringFilter]")
{
  // Read the Small IN100 Data set
  auto baseDataFilePath = fs::path(fmt::format("{}/TestFiles/GBCD/6_6_Small_IN100_GBCD.dream3d", unit_test::k_DREAM3DDataDir));
  DataStructure dataStructure = UnitTest::LoadDataStructure(baseDataFilePath);

  // Instantiate the filter, a DataStructure object and an Arguments Object
  GenerateFaceMisorientationColoringFilter filter;
  Arguments args;

  args.insertOrAssign(GenerateFaceMisorientationColoringFilter::k_SurfaceMeshFaceLabelsArrayPath_Key, std::make_any<DataPath>(faceLabels));
  args.insertOrAssign(GenerateFaceMisorientationColoringFilter::k_AvgQuatsArrayPath_Key, std::make_any<DataPath>(avgQuatsPath));
  args.insertOrAssign(GenerateFaceMisorientationColoringFilter::k_FeaturePhasesArrayPath_Key, std::make_any<DataPath>(faceAreas));
  args.insertOrAssign(GenerateFaceMisorientationColoringFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(crystalStructurePath));
  args.insertOrAssign(GenerateFaceMisorientationColoringFilter::k_SurfaceMeshFaceMisorientationColorsArrayName_Key, std::make_any<std::string>(::k_NXFaceMisorientationColors));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_INVALID(preflightResult.outputActions);

  // Execute the filter and check the result
  auto executeResult = filter.execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_INVALID(executeResult.result);
}
