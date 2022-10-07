#include "CalculateArrayHistogram.hpp"

#include "Core/Filters/CalculateArrayHistogramFilter.hpp"

#include "complex/Common/Range.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/Utilities/ParallelDataAlgorithm.hpp"

#include <chrono>

using namespace complex;

namespace
{
template <typename T>
class GenerateHistogramFromData
{
public:
  GenerateHistogramFromData(CalculateArrayHistogram& filter, const int32 numBins, const DataArray<T>& inputArray, Float64Array& histogram, std::atomic<usize>& overflow, float64 minRange = 0.0,
                            float64 maxRange = 0.0)
  : m_Filter(filter)
  , m_NumBins(numBins)
  , m_Min(minRange)
  , m_Max(maxRange)
  , m_InputArray(inputArray)
  , m_Histogram(histogram)
  , m_Overflow(overflow)
  {
  }
  ~GenerateHistogramFromData() = default;

  void generate(const usize start, const usize end) const
  {
    // tuple visulaization: Histogram = {(bin maximum, count), (bin maximum, count), ... }

    float64 min = std::numeric_limits<float>::max();
    float64 max = -1.0 * std::numeric_limits<float>::max();
    if(m_Max == 0.0 && m_Min == 0.0)
    {
      const auto numOfElements = m_InputArray.getNumberOfComponents() * m_InputArray.getNumberOfTuples();
      for(size_t i = 0; i < numOfElements; i++) // min and max in the input array
      {
        if(static_cast<float64>(m_InputArray[i]) > max)
        {
          max = static_cast<float64>(m_InputArray[i]);
        }
        if(static_cast<float64>(m_InputArray[i]) < min)
        {
          min = static_cast<float64>(m_InputArray[i]);
        }
      }
      max = static_cast<float64>(max + 1); // ensure upper limit encapsulates max value
      min = static_cast<float64>(min - 1); // ensure lower limit encapsulates min value
    }
    else
    {
      min = m_Min;
      max = m_Max;
    }

    auto beginning = std::chrono::steady_clock::now();
    const float64 increment = (max - min) / m_NumBins;
    if(m_NumBins == 1) // if one bin, just set the first element to total number of points
    {
      m_Histogram[0] = max;
      m_Histogram[1] = end;
    }
    else
    {
      for(usize i = start; i < end; i++)
      {
        m_Filter.updateThreadSafeProgress();
        if(m_Filter.getCancel())
        {
          return;
        }
        const auto bin = std::floor((m_InputArray.at(i) - min) / increment);
        if((bin >= 0) && (bin < m_NumBins))
        {
          m_Histogram[bin * 2 + 1]++;
        }
        else
        {
          m_Overflow++;
        }
      }
    }

    for(int64 i = 0; i < m_NumBins; i++)
    {
      m_Histogram[(i * 2)] = static_cast<float64>(min + (increment * (i + 1))); // load bin maximum into respective postion {(x, ), (x , ), ...}
    }
  }

  void operator()(const Range& range) const
  {
    generate(range.min(), range.max());
  }

private:
  CalculateArrayHistogram& m_Filter = nullptr;
  const int32 m_NumBins = 1;
  float64 m_Min = 0.0;
  float64 m_Max = 0.0;
  const DataArray<T>& m_InputArray;
  Float64Array& m_Histogram;
  std::atomic<usize>& m_Overflow;
};
} // namespace

// -----------------------------------------------------------------------------
CalculateArrayHistogram::CalculateArrayHistogram(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                                 CalculateArrayHistogramInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
CalculateArrayHistogram::~CalculateArrayHistogram() noexcept = default;

// -----------------------------------------------------------------------------
void CalculateArrayHistogram::updateProgress(const std::string& progMessage)
{
  m_MessageHandler({IFilter::Message::Type::Info, progMessage});
}
// -----------------------------------------------------------------------------
void CalculateArrayHistogram::updateThreadSafeProgress()
{
  std::lock_guard<std::mutex> guard(m_ProgressMessage_Mutex);

  m_ProgressCounter++;
  size_t progressInt = static_cast<size_t>((static_cast<double>(m_ProgressCounter) / m_TotalElements) * 100.0f);

  auto now = std::chrono::steady_clock::now();
  if(std::chrono::duration_cast<std::chrono::milliseconds>(now - m_InitialTime).count() > 1000) // every second update
  {
    std::string progressMessage = "Calculating... ";
    m_MessageHandler(IFilter::ProgressMessage{IFilter::Message::Type::Progress, progressMessage, static_cast<int32_t>(progressInt)});
    m_InitialTime = std::chrono::steady_clock::now();
  }
}

// -----------------------------------------------------------------------------
const std::atomic_bool& CalculateArrayHistogram::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> CalculateArrayHistogram::operator()()
{
  const auto numBins = m_InputValues->NumberOfBins;
  const auto selectedArrayPaths = m_InputValues->SelectedArrayPaths;
  
  for (const auto& arrayPath : selectedArrayPaths)
  {
      m_TotalElements += m_DataStructure.getDataAs<IDataArray>(arrayPath)->getSize();
  }

  for(int32 i = 0; i < selectedArrayPaths.size(); i++)
  {
    const auto& selectedArrayPath = selectedArrayPaths.at(i);
    const auto& inputData = m_DataStructure.getDataRefAs<IDataArray>(selectedArrayPath);
    auto type = inputData.getDataType();
    auto& histogramPath = m_InputValues->CreatedHistogramDataPaths.at(i);
    auto& histogram = m_DataStructure.getDataRefAs<DataArray<float64>>(histogramPath);
    const auto numElements = inputData.getNumberOfTuples() * inputData.getNumberOfComponents();
    ParallelDataAlgorithm dataAlg;
    dataAlg.setRange(0, numElements);
    std::atomic<usize> overflow = 0;
    if(m_ShouldCancel)
    {
      return {};
    }

    switch(type)
    {
    case DataType::int8: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<int8>>(selectedArrayPath), histogram, overflow, m_InputValues->MinRange, m_InputValues->MaxRange));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<int8>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::int16: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<int16>>(selectedArrayPath), histogram, overflow, m_InputValues->MinRange, m_InputValues->MaxRange));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<int16>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::int32: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<int32>>(selectedArrayPath), histogram, overflow, m_InputValues->MinRange, m_InputValues->MaxRange));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<int32>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::int64: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<int64>>(selectedArrayPath), histogram, overflow, m_InputValues->MinRange, m_InputValues->MaxRange));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<int64>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::uint8: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<uint8>>(selectedArrayPath), histogram, overflow, m_InputValues->MinRange, m_InputValues->MaxRange));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<uint8>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::uint16: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<uint16>>(selectedArrayPath), histogram, overflow, m_InputValues->MinRange, m_InputValues->MaxRange));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<uint16>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::uint32: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<uint32>>(selectedArrayPath), histogram, overflow, m_InputValues->MinRange, m_InputValues->MaxRange));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<uint32>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::uint64: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<uint64>>(selectedArrayPath), histogram, overflow, m_InputValues->MinRange, m_InputValues->MaxRange));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<uint64>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::float32: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<float32>>(selectedArrayPath), histogram, overflow, m_InputValues->MinRange, m_InputValues->MaxRange));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<float32>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::float64: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<float64>>(selectedArrayPath), histogram, overflow, m_InputValues->MinRange, m_InputValues->MaxRange));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(*this, numBins, m_DataStructure.getDataRefAs<DataArray<float64>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    default: {
      throw std::runtime_error("Invalid DataType");
    }
    }

    if(overflow > 0)
    {
      const std::string arrayName = inputData.getName();
      CalculateArrayHistogram::updateProgress(fmt::format("{} values not categorized into bin for array {}", overflow, arrayName));
    }
  }

  return {};
}
