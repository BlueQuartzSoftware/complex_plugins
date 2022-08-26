#pragma once

#include "Core/Core_export.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{

struct CORE_EXPORT CalculateArrayHistogramInputValues
{
  int32 NumberOfBins;
  bool UserDefinedRange;
  float64 MinRange;
  float64 MaxRange;
  std::vector<DataPath> SelectedArrayPaths;
  DataPath NewDataGroupName;
  DataPath NewDataArrayName;
  std::vector<DataPath> OutputArrayPaths;
};

/**
 * @class CalculateArrayHistogram
 * @brief This filter calculates a Histogram according to user specification and stores it accordingly
 */

class CORE_EXPORT CalculateArrayHistogram
{
public:
  CalculateArrayHistogram(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, CalculateArrayHistogramInputValues* inputValues);
  ~CalculateArrayHistogram() noexcept;

  CalculateArrayHistogram(const CalculateArrayHistogram&) = delete;
  CalculateArrayHistogram(CalculateArrayHistogram&&) noexcept = delete;
  CalculateArrayHistogram& operator=(const CalculateArrayHistogram&) = delete;
  CalculateArrayHistogram& operator=(CalculateArrayHistogram&&) noexcept = delete;

  Result<> operator()();

  const std::atomic_bool& getCancel();

private:
  DataStructure& m_DataStructure;
  const CalculateArrayHistogramInputValues* m_InputValues = nullptr;
  const std::atomic_bool& m_ShouldCancel;
  const IFilter::MessageHandler& m_MessageHandler;
};

} // namespace complex
