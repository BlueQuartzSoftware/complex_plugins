#include "WriteASCIIData.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/Common/Types.hpp"

#include <fstream>
#include <string>
#include <chrono>

using namespace complex;
namespace // defining constant expressions for choices parameters
{
// delimiter choices
constexpr int64 k_Comma = 0;
constexpr int64 k_Semicolon = 1;
constexpr int64 k_Space = 2;
constexpr int64 k_Colon = 3;
constexpr int64 k_Tab = 4;

// output type choices
constexpr int64 k_MultipleFiles = 0;
constexpr int64 k_SingleFile = 1;
} // namespace

namespace // once logic has been verified try using buffer flush instead to speed up processing
{
template <typename T>
class WriteOutASCIIData
{
public:
  WriteOutASCIIData(WriteASCIIData* filter, complex::DataArray<T>& inputData, int32 maxValPerLine, char delimiter, std::string filePath)
  : m_Filter(filter)
  , m_InputData(inputData)
  , m_MaxValPerLine(maxValPerLine)
  , m_Delimiter(delimiter)
  , m_FilePath(filePath)
  {
  }
  ~WriteOutASCIIData() = default;

  // function should just print
  void execute()
  {
    int32 recCount = 0;
    int32 count = 0 
    auto start = std::chrono::steady_clock::now();
    std::ofstream fout(filePath, std::ios_base::app);  // open precreated file in append mode
    for(auto tuple : m_InputData)
    {
      auto now = std::chrono::steady_clock::now();
      // Only send updates every 1 second
      if(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > 1000)
      {
        std::string message = fmt::format("Processing {}: {}% completed", m_InputData.getName(), static_cast<int32>(100 * (static_cast<float>(count) / static_cast<float>(m_InputData.getSize()))));
        m_Filter->updateProgress(message);
        start = std::chrono::steady_clock::now();
      }
      if(m_Filter->getCancel())
      {
        return;
      }
      for(auto element : tuple)
      {
        fout << element;

        recCount++;

        if(recCount >= m_MaxValPerLine)
        {
          fout << m_Delimiter;
          recCount = 0;
        }
        else
        {
          fout << m_Delimiter;
        }
      }
      count++;
    }
    fout.close();
  }

  void operator()()const 
  {
    execute()
  }

private:
  WriteASCIIData* m_Filter = nullptr;
  complex::DataArray<T>& m_InputData;
  int32 m_MaxValPerLine = 0;
  char m_Delimiter = ' ';
  std::string m_FilePath = {};
};
} // namespace

// -----------------------------------------------------------------------------
WriteASCIIData::WriteASCIIData(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, WriteASCIIDataInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
WriteASCIIData::~WriteASCIIData() noexcept = default;

// -----------------------------------------------------------------------------
void WriteASCIIData::updateProgress(const std::string& progMessage)
{
  m_MessageHandler({IFilter::Message::Type::Info, progMessage});
}
// -----------------------------------------------------------------------------
const std::atomic_bool& WriteASCIIData::getCancel()
{
  return m_ShouldCancel;
}
// -----------------------------------------------------------------------------
Result<> WriteASCIIData::operator()()
{
  const auto del = m_InputValues->delimiter;
  char delimiter = ' ';
  switch(del)
  {
  case k_Comma: //0
  {
    delimiter = ',';
    break;
  }
  case k_Semicolon: //1
  {
    delimiter = ';';
    break;
  }
  case k_Space: //2
  {
    delimiter = ' ';
    break;
  }
  case k_Colon: //3
  {
    delimiter = ':';
    break;
  }
  case k_Tab: //4
  {
    delimiter = '\t';
    break;
  }
  default: 
  {
    delimiter = ' ';
  }
  }
  
  int32 maxValPerLine = m_InputValues->maxValPerLine;
  if (maxValPerLine < 1)
  {
    MakeErrorResult(-11020, fmt::format("Characters per line must be greater than zero: {}", m_InputValues->maxValPerLine));
    return {};
  }

  auto selectedDataArrayPaths = m_InputValues->selectedDataArrayPaths;
  int32 count = 0; // used to dispaly progress to user (declared here to avoid duplication)
  std::string filePath = "";
  //if multi file
  if (m_InputValues->outputStyle == k_MultipleFiles) // k_MultipleFiles = 0
  {
    for(const auto& selectedArrayPath : selectedDataArrayPaths)
    {
      if(m_ShouldCancel)
      {
        return {};
      }
      auto& oldSelectedArray = m_DataStructure.getDataRefAs<IDataArray>(selectedArrayPath);
      m_MessageHandler(fmt::format("Now Writing: {}", oldSelectedArray.getName())); // progress display
      filePath = getFilePath(oldSelectedArray);
      DataType type = oldSelectedArray.getDataType();
      switch(type)
      {
      case DataType::boolean: {
        WriteOutASCIIData<bool>(this, m_DataStructure.getDataRefAs<DataArray<bool>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::int8: {
        WriteOutASCIIData<int8>(this, m_DataStructure.getDataRefAs<DataArray<int8>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::int16: {
        WriteOutASCIIData<int16>(this, m_DataStructure.getDataRefAs<DataArray<int16>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::int32: {
        WriteOutASCIIData<int32>(this, m_DataStructure.getDataRefAs<DataArray<int32>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::int64: {
        WriteOutASCIIData<int64>(this, m_DataStructure.getDataRefAs<DataArray<int64>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::uint8: {
        WriteOutASCIIData<uint8>(this, m_DataStructure.getDataRefAs<DataArray<uint8>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::uint16: {
        WriteOutASCIIData<uint16>(this, m_DataStructure.getDataRefAs<DataArray<uint16>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::uint32: {
        WriteOutASCIIData<uint32>(this, m_DataStructure.getDataRefAs<DataArray<uint32>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::uint64: {
        WriteOutASCIIData<uint64>(this, m_DataStructure.getDataRefAs<DataArray<uint64>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::float32: {
        WriteOutASCIIData<float32>(this, m_DataStructure.getDataRefAs<DataArray<float32>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::float64: {
        WriteOutASCIIData<float64>(this, m_DataStructure.getDataRefAs<DataArray<float64>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      default: {
        throw std::runtime_error("Invalid DataType");
      }
      }
      count++;
    }
  }
  //if single file
  else if (m_InputValues->outputStyle == k_SingleFile) // k_SingleFile = 1
  {
    for(const auto& selectedArrayPath : selectedDataArrayPaths)
    {
      if(m_ShouldCancel)
      {
        return {};
      }
      m_MessageHandler(fmt::format("Generating Header Number: {}", count + 1)); //progress display
      auto& selectedArrayPtr = m_DataStructure.getDataRefAs<IDataArray>(selectedArrayPath);
      if(count == 0) // initialize file path
      {
        filePath = getFilePath(selectedArrayPtr);
      }
      std::ofstream fout(filePath, std::ios_base::app); // open precreated file in append mode
      fout << selectedArrayPtr.getName() << delimiter;
      count++;
      if(count == selectedArrayPtr.getNumberOfTuples()) 
      {
        fout << "\n";
      }
      fout.close();
    }
    
    count = 0; // reset

    //begin printing arrays
    for(const auto& selectedArrayPath : selectedDataArrayPaths)
    {
      if(m_ShouldCancel)
      {
        return {};
      }
      auto& oldSelectedArray = m_DataStructure.getDataRefAs<IDataArray>(selectedArrayPath);
      m_MessageHandler(fmt::format("Now Writing: {}", oldSelectedArray.getName())); // progress display
      DataType type = oldSelectedArray.getDataType();
      switch(type)
      {
      case DataType::boolean: {
        WriteOutASCIIData<bool>(this, m_DataStructure.getDataRefAs<DataArray<bool>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::int8: {
        WriteOutASCIIData<int8>(this, m_DataStructure.getDataRefAs<DataArray<int8>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::int16: {
        WriteOutASCIIData<int16>(this, m_DataStructure.getDataRefAs<DataArray<int16>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::int32: {
        WriteOutASCIIData<int32>(this, m_DataStructure.getDataRefAs<DataArray<int32>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::int64: {
        WriteOutASCIIData<int64>(this, m_DataStructure.getDataRefAs<DataArray<int64>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::uint8: {
        WriteOutASCIIData<uint8>(this, m_DataStructure.getDataRefAs<DataArray<uint8>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::uint16: {
        WriteOutASCIIData<uint16>(this, m_DataStructure.getDataRefAs<DataArray<uint16>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::uint32: {
        WriteOutASCIIData<uint32>(this, m_DataStructure.getDataRefAs<DataArray<uint32>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::uint64: {
        WriteOutASCIIData<uint64>(this, m_DataStructure.getDataRefAs<DataArray<uint64>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::float32: {
        WriteOutASCIIData<float32>(this, m_DataStructure.getDataRefAs<DataArray<float32>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      case DataType::float64: {
        WriteOutASCIIData<float64>(this, m_DataStructure.getDataRefAs<DataArray<float64>>(selectedArrayPath), maxValPerLine, delimiter, filePath);
        break;
      }
      default: {
        throw std::runtime_error("Invalid DataType");
      }
      }
      count++;
    }
  }
  return {};
}


// -----------------------------------------------------------------------------
std::string WriteASCIIData::getFilePath(const DataObject& selectedArrayPtr)
{
  std::string name = selectedArrayPtr.getName();
  std::string extension = m_InputValues->fileExtension;

  std::string fullPath = m_InputValues->outputPath.string() + name + extension; // this is correct formatting but vector parameters need to be fixed

  std::ofstream fout (fullPath, std::ofstream::out);  // test name resolution and create file
  if(!fout.is_open()) 
  {
    MakeErrorResult(-11025, fmt::format("Error opening path {}", fullPath)); // remember to adjust the error codes correctly
    return "";
  }
  fout.close();
  return fullPath;
}
