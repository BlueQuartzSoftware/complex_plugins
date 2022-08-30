#include <catch2/catch.hpp>

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/DataGroupSelectionParameter.hpp"
#include "complex/Parameters/MultiArraySelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"

#include "Core/Filters/CalculateArrayHistogramFilter.hpp"

#include <iostream>

using namespace complex;

namespace
{
template <typename T>
void fillArray(DataArray<T>* data, std::vector<T> values)
{
  int32 count = 0;
  for(T value : values)
  {
    data->getDataStore()->setValue(count, value);
    count++;
  }
}
} // namespace

TEST_CASE("Core::CalculateArrayHistogram: Valid Filter Execution", "[Core][CalculateArrayHistogram]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  CalculateArrayHistogramFilter filter;
  DataStructure dataStruct;
  Arguments args;

  // load vector with data paths for test
  ::fillArray(DataArray<float64>::CreateWithStore<DataStore<float64>>(dataStruct, "array0", {static_cast<usize>(4)}, {static_cast<usize>(3)}),
              {0.0, 5.5, 8.5, 9.2, 16.7, 907.3, 5.0, 6.9, 83.7387483, -56.8, 3.7, -4.9});
  ::fillArray(DataArray<int32>::CreateWithStore<DataStore<int32>>(dataStruct, "array1", {static_cast<usize>(4)}, {static_cast<usize>(3)}), {56, 82, 46, 93, 73, 57, 24, 32, -90, -35, 74, -19});
  ::fillArray(DataArray<uint32>::CreateWithStore<DataStore<uint32>>(dataStruct, "array2", {static_cast<usize>(4)}, {static_cast<usize>(3)}), {83, 93, 75, 67, 8977, 56, 48, 92, 57, 34, 34, 34});

  std::vector<DataPath> dataPaths = dataStruct.getAllDataPaths();
  auto dataGPath = *std::move(DataPath::FromString("DataGroup"));

  // Create default Parameters for the filter.
  args.insertOrAssign(CalculateArrayHistogramFilter::k_NumberOfBins_Key, std::make_any<int32>(4));
  args.insertOrAssign(CalculateArrayHistogramFilter::k_UserDefinedRange_Key, std::make_any<bool>(false));
  args.insertOrAssign(CalculateArrayHistogramFilter::k_NewDataGroup_Key, std::make_any<bool>(true));
  args.insertOrAssign(CalculateArrayHistogramFilter::k_SelectedArrayPaths_Key, std::make_any<MultiArraySelectionParameter::ValueType>(dataPaths));
  args.insertOrAssign(CalculateArrayHistogramFilter::k_NewDataGroupName_Key, std::make_any<DataPath>(dataGPath));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(dataStruct, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(dataStruct, args);
  REQUIRE(executeResult.result.valid());

  // load vector with child paths from filter
  std::vector<DataPath> childPaths;
  for(const auto& dataPath : dataPaths)
  {
    childPaths.push_back(*std::move(DataPath::FromString("DataGroup/" + dataPath.toString() + "Histogram")));
  }

  for(const auto& child : childPaths)
  {
    auto& dataArray = dataStruct.getDataRefAs<DataArray<float32>>(child);
    for(int32 index = 0; index < dataArray.getNumberOfTuples(); index++)
    {
      for(int32 i = 0; i < dataArray.getNumberOfComponents(); i++)
      {
        std::cout << dataArray[(index * dataArray.getNumberOfComponents()) + i] << ", ";
      }
      std::cout << "\t";
    }
    std::cout << std::endl;
  }
}
