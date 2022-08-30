#include "CalculateArrayHistogram.hpp"

#include "Core/Filters/CalculateArrayHistogramFilter.hpp"

#include "complex/Common/ComplexRange.hpp"
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
  using type = T;
  GenerateHistogramFromData(CalculateArrayHistogram* filter, const int32 numBins, const float64 minRange, const float64 maxRange, const DataArray<typename type>& inputArray, Float32Array& histogram,
                            usize& overflow)
  : m_Filter(filter)
  , m_NumBins(numBins)
  , m_Min(minRange)
  , m_Max(maxRange)
  , m_InputArray(inputArray)
  , m_Histogram(histogram)
  , m_Overflow(overflow)
  {
  }
  GenerateHistogramFromData(CalculateArrayHistogram* filter, const int32 numBins, const DataArray<typename type>& inputArray, Float32Array& histogram, usize& overflow)
  : m_Filter(filter)
  , m_NumBins(numBins)
  , m_InputArray(inputArray)
  , m_Histogram(histogram)
  , m_Overflow(overflow)
  {
    float64 min = std::numeric_limits<float>::max();
    float64 max = -1.0 * std::numeric_limits<float>::max();
    for(size_t i = 0; i < m_InputArray.getNumberOfTuples(); i++) // min and max in the input array
    {
      if(static_cast<float>(m_InputArray[i]) > max)
      {
        max = static_cast<float>(m_InputArray[i]);
      }
      if(static_cast<float>(m_InputArray[i]) < min)
      {
        min = static_cast<float>(m_InputArray[i]);
      }
    }
    m_Max = max;
    m_Min = min;
  }
  ~GenerateHistogramFromData() = default;

  void generate(const usize start, const usize end) const
  {
    auto beginning = std::chrono::steady_clock::now();
    const float32 increment = (m_Max - m_Min) / m_NumBins;
    if(m_NumBins == 1)
    {
      m_Histogram[0] = m_Max;
      m_Histogram[1] = end;
    }
    else
    {
      for(usize i = start; i < end; i++)
      {
        auto now = std::chrono::steady_clock::now();
        // send updates every 1 second
        if(std::chrono::duration_cast<std::chrono::milliseconds>(now - beginning).count() > 1000)
        {
          std::string message = fmt::format("Processing {}: {}% completed", m_InputArray.getName(), static_cast<int32>(100 * (static_cast<float>(i) / static_cast<float>(end))));
          m_Filter->updateProgress(message);
          beginning = std::chrono::steady_clock::now();
        }
        if(m_Filter->getCancel())
        {
          return;
        }
        const auto bin = (m_InputArray.at(i) - m_Min) / increment;
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

    for(int32 i = 0; i < m_NumBins; i++)
    {
      m_Histogram[i * 2] = m_Min + increment * (i + 1);
    }
  }

  void operator()(const ComplexRange& range) const
  {
    generate(range.min(), range.max());
  }

private:
  CalculateArrayHistogram* m_Filter = nullptr;
  const int32 m_NumBins = 1;
  float64 m_Min = 0.0;
  float64 m_Max = 0.0;
  const DataArray<T>& m_InputArray;
  Float32Array& m_Histogram;
  usize& m_Overflow = 0;
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
const std::atomic_bool& CalculateArrayHistogram::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> CalculateArrayHistogram::operator()()
{
  const auto numBins = m_InputValues->NumberOfBins;
  const auto selectedArrayPaths = m_InputValues->SelectedArrayPaths;

  for(int32 i = 0; i < selectedArrayPaths.size(); i++)
  {
    const auto& selectedArrayPath = selectedArrayPaths.at(i);
    const auto& inputData = m_DataStructure.getDataRefAs<IDataArray>(selectedArrayPath);
    auto type = inputData.getDataType();
    auto& histogramPath = m_InputValues->CreatedHistogramDataPaths.at(i);
    auto& histogram = m_DataStructure.getDataRefAs<DataArray<float32>>(histogramPath);
    const auto numTup = inputData.getNumberOfTuples();
    ParallelDataAlgorithm dataAlg;
    dataAlg.setRange(0, numTup);
    usize overflow = 0;
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
            GenerateHistogramFromData(this, numBins, m_InputValues->MinRange, m_InputValues->MaxRange, m_DataStructure.getDataRefAs<DataArray<int8>>(selectedArrayPath), histogram, overflow));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(this, numBins, m_DataStructure.getDataRefAs<DataArray<int8>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::int16: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(this, numBins, m_InputValues->MinRange, m_InputValues->MaxRange, m_DataStructure.getDataRefAs<DataArray<int16>>(selectedArrayPath), histogram, overflow));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(this, numBins, m_DataStructure.getDataRefAs<DataArray<int16>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::int32: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(this, numBins, m_InputValues->MinRange, m_InputValues->MaxRange, m_DataStructure.getDataRefAs<DataArray<int32>>(selectedArrayPath), histogram, overflow));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(this, numBins, m_DataStructure.getDataRefAs<DataArray<int32>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::int64: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(this, numBins, m_InputValues->MinRange, m_InputValues->MaxRange, m_DataStructure.getDataRefAs<DataArray<int64>>(selectedArrayPath), histogram, overflow));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(this, numBins, m_DataStructure.getDataRefAs<DataArray<int64>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::uint8: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(this, numBins, m_InputValues->MinRange, m_InputValues->MaxRange, m_DataStructure.getDataRefAs<DataArray<uint8>>(selectedArrayPath), histogram, overflow));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(this, numBins, m_DataStructure.getDataRefAs<DataArray<uint8>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::uint16: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(this, numBins, m_InputValues->MinRange, m_InputValues->MaxRange, m_DataStructure.getDataRefAs<DataArray<uint16>>(selectedArrayPath), histogram, overflow));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(this, numBins, m_DataStructure.getDataRefAs<DataArray<uint16>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::uint32: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(this, numBins, m_InputValues->MinRange, m_InputValues->MaxRange, m_DataStructure.getDataRefAs<DataArray<uint32>>(selectedArrayPath), histogram, overflow));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(this, numBins, m_DataStructure.getDataRefAs<DataArray<uint32>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::uint64: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(this, numBins, m_InputValues->MinRange, m_InputValues->MaxRange, m_DataStructure.getDataRefAs<DataArray<uint64>>(selectedArrayPath), histogram, overflow));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(this, numBins, m_DataStructure.getDataRefAs<DataArray<uint64>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::float32: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(this, numBins, m_InputValues->MinRange, m_InputValues->MaxRange, m_DataStructure.getDataRefAs<DataArray<float32>>(selectedArrayPath), histogram, overflow));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(this, numBins, m_DataStructure.getDataRefAs<DataArray<float32>>(selectedArrayPath), histogram, overflow));
      }
      break;
    }
    case DataType::float64: {
      if(m_InputValues->UserDefinedRange)
      {
        dataAlg.execute(
            GenerateHistogramFromData(this, numBins, m_InputValues->MinRange, m_InputValues->MaxRange, m_DataStructure.getDataRefAs<DataArray<float64>>(selectedArrayPath), histogram, overflow));
      }
      else
      {
        dataAlg.execute(GenerateHistogramFromData(this, numBins, m_DataStructure.getDataRefAs<DataArray<float64>>(selectedArrayPath), histogram, overflow));
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
