#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"

#include "OrientationAnalysis/Filters/FindFeatureReferenceMisorientationsFilter.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"

using namespace complex;

TEST_CASE("OrientationAnalysis::FindFeatureReferenceMisorientationsFilter", "[OrientationAnalysis][FindFeatureReferenceMisorientationsFilter][.][UNIMPLEMENTED][!mayfail]")
{
  // Read the Small IN100 Data set
  auto baseDataFilePath = fs::path(fmt::format("{}/TestFiles/6_6_stats_test.dream3d", unit_test::k_DREAM3DDataDir));
  DataStructure dataStructure = UnitTest::LoadDataStructure(baseDataFilePath);
  DataPath smallIn100Group({complex::Constants::k_DataContainer});
  DataPath cellDataPath = smallIn100Group.createChildPath(complex::Constants::k_CellData);

  {
    // Instantiate the filter, a DataStructure object and an Arguments Object
    FindFeatureReferenceMisorientationsFilter filter;
    Arguments args;

    // Create default Parameters for the filter.
    args.insertOrAssign(FindFeatureReferenceMisorientationsFilter::k_FeatureIdsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindFeatureReferenceMisorientationsFilter::k_CellPhasesArrayPath_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindFeatureReferenceMisorientationsFilter::k_QuatsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindFeatureReferenceMisorientationsFilter::k_GBEuclideanDistancesArrayPath_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindFeatureReferenceMisorientationsFilter::k_AvgQuatsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindFeatureReferenceMisorientationsFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindFeatureReferenceMisorientationsFilter::k_FeatureReferenceMisorientationsArrayName_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindFeatureReferenceMisorientationsFilter::k_FeatureAvgMisorientationsArrayName_Key, std::make_any<DataPath>(DataPath{}));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }
}
