#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"

#include "OrientationAnalysis/Filters/FindMisorientationsFilter.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"

using namespace complex;

TEST_CASE("OrientationAnalysis::FindMisorientationsFilter", "[OrientationAnalysis][FindMisorientationsFilter]")
{
  // Read the Small IN100 Data set
  auto baseDataFilePath = fs::path(fmt::format("{}/TestFiles/6_6_stats_test.dream3d", unit_test::k_DREAM3DDataDir));
  DataStructure dataStructure = UnitTest::LoadDataStructure(baseDataFilePath);
  DataPath smallIn100Group({complex::Constants::k_DataContainer});
  DataPath cellDataPath = smallIn100Group.createChildPath(complex::Constants::k_CellData);

  {
    // Instantiate the filter, a DataStructure object and an Arguments Object
    FindMisorientationsFilter filter;
    DataStructure ds;
    Arguments args;

    // Create default Parameters for the filter.
    args.insertOrAssign(FindMisorientationsFilter::k_FindAvgMisors_Key, std::make_any<bool>(false));
    args.insertOrAssign(FindMisorientationsFilter::k_NeighborListArrayPath_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindMisorientationsFilter::k_AvgQuatsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindMisorientationsFilter::k_FeaturePhasesArrayPath_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindMisorientationsFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindMisorientationsFilter::k_MisorientationListArrayName_Key, std::make_any<DataPath>(DataPath{}));
    args.insertOrAssign(FindMisorientationsFilter::k_AvgMisorientationsArrayName_Key, std::make_any<DataPath>(DataPath{}));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);

  }
}

