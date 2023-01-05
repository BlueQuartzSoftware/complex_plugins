#include "FillBadData.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/Utilities/DataGroupUtilities.hpp"
#include "complex/Utilities/FilterUtilities.hpp"
#include "complex/Utilities/ParallelTaskAlgorithm.hpp"

using namespace complex;

namespace
{

// struct FillBadDataFunctor
//{
//   template <typename ScalarType>
//   void operator()(const Int32Array& featureIds, IDataArray& output, const std::vector<int32>& neighbors)
//   {
//     DataArray<ScalarType>& outputArray = dynamic_cast<DataArray<ScalarType>>(output);
//     for(size_t tupleIndex = 0; tupleIndex < featureIds.getNumberOfTuples(); tupleIndex++)
//     {
//       const int32 featureName = featureIds[tupleIndex];
//       const int32 neighbor = neighbors[tupleIndex];
//       if(featureName < 0 && neighbor != -1 && featureIds[static_cast<size_t>(neighbor)] > 0)
//       {
//         outputArray.copyTuple(neighbor, tupleIndex);
//       }
//     }
//   }
// };

template <typename T>
class FillBadDataUpdateTuples
{
public:
  FillBadDataUpdateTuples(const Int32Array& featureIds, DataArray<T>& outputArray, const std::vector<int32>& neighbors)
  : m_FeatureIds(featureIds)
  , m_OuputArray(outputArray)
  , m_Neighbors(neighbors)
  {
  }
  ~FillBadDataUpdateTuples() = default;

  FillBadDataUpdateTuples(const FillBadDataUpdateTuples&) = default;           // Copy Constructor Not Implemented
  FillBadDataUpdateTuples(FillBadDataUpdateTuples&&) = delete;                 // Move Constructor Not Implemented
  FillBadDataUpdateTuples& operator=(const FillBadDataUpdateTuples&) = delete; // Copy Assignment Not Implemented
  FillBadDataUpdateTuples& operator=(FillBadDataUpdateTuples&&) = delete;      // Move Assignment Not Implemented

  void convert(size_t start, size_t stop) const
  {
    for(size_t tupleIndex = start; tupleIndex < stop; tupleIndex++)
    {
      const int32 featureName = m_FeatureIds[tupleIndex];
      const int32 neighbor = m_Neighbors[tupleIndex];
      if(featureName < 0 && neighbor != -1 && m_FeatureIds[static_cast<size_t>(neighbor)] > 0)
      {
        m_OuputArray.copyTuple(neighbor, tupleIndex);
      }
    }
  }

  void operator()() const
  {
    convert(0, m_OuputArray.getNumberOfTuples());
  }

private:
  const Int32Array& m_FeatureIds;
  DataArray<T>& m_OuputArray;
  const std::vector<int32>& m_Neighbors;
};
} // namespace

// -----------------------------------------------------------------------------
FillBadData::FillBadData(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, FillBadDataInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
FillBadData::~FillBadData() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& FillBadData::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> FillBadData::operator()()
{
  auto& m_FeatureIds = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->featureIdsArrayPath);
  const size_t totalPoints = m_FeatureIds.getNumberOfTuples();

  std::vector<int32> m_Neighbors(totalPoints, -1);

  std::vector<bool> m_AlreadyChecked(totalPoints, false);

  const auto& selectedImageGeom = m_DataStructure.getDataRefAs<ImageGeom>(m_InputValues->inputImageGeometry);

  const SizeVec3 udims = selectedImageGeom.getDimensions();

  auto* m_CellPhases = m_DataStructure.getDataAs<Int32Array>(m_InputValues->featureIdsArrayPath);

  std::array<int64_t, 3> dims = {
      static_cast<int64_t>(udims[0]),
      static_cast<int64_t>(udims[1]),
      static_cast<int64_t>(udims[2]),
  };

  size_t count = 1;
  int32_t good = 1;
  int64_t neighbor;
  int64_t index = 0;
  float x = 0.0f, y = 0.0f, z = 0.0f;
  int64_t column = 0, row = 0, plane = 0;
  int64_t neighpoint = 0;
  int32_t featurename = 0, feature = 0;
  size_t numfeatures = 0;
  size_t maxPhase = 0;

  for(size_t i = 0; i < totalPoints; i++)
  {
    featurename = m_FeatureIds[i];
    if(featurename > numfeatures)
    {
      numfeatures = featurename;
    }
  }

  if(m_InputValues->storeAsNewPhase)
  {
    for(size_t i = 0; i < totalPoints; i++)
    {
      if((*m_CellPhases)[i] > maxPhase)
      {
        maxPhase = (*m_CellPhases)[i];
      }
    }
  }

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];
  std::vector<int64_t> currentvlist;

  for(size_t iter = 0; iter < totalPoints; iter++)
  {
    m_AlreadyChecked[iter] = false;
    if(m_FeatureIds[iter] != 0)
    {
      m_AlreadyChecked[iter] = true;
    }
  }

  for(size_t i = 0; i < totalPoints; i++)
  {
    if(!m_AlreadyChecked[i] && m_FeatureIds[i] == 0)
    {
      currentvlist.push_back(static_cast<int64_t>(i));
      count = 0;
      while(count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % dims[0];
        row = (index / dims[0]) % dims[1];
        plane = index / (dims[0] * dims[1]);
        for(int32_t j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = index + neighpoints[j];
          if(j == 0 && plane == 0)
          {
            good = 0;
          }
          if(j == 5 && plane == (dims[2] - 1))
          {
            good = 0;
          }
          if(j == 1 && row == 0)
          {
            good = 0;
          }
          if(j == 4 && row == (dims[1] - 1))
          {
            good = 0;
          }
          if(j == 2 && column == 0)
          {
            good = 0;
          }
          if(j == 3 && column == (dims[0] - 1))
          {
            good = 0;
          }
          if(good == 1 && m_FeatureIds[neighbor] == 0 && !m_AlreadyChecked[neighbor])
          {
            currentvlist.push_back(neighbor);
            m_AlreadyChecked[neighbor] = true;
          }
        }
        count++;
      }
      if((int32_t)currentvlist.size() >= m_InputValues->minAllowedDefectSizeValue)
      {
        for(size_t k = 0; k < currentvlist.size(); k++)
        {
          m_FeatureIds[currentvlist[k]] = 0;
          if(m_InputValues->storeAsNewPhase)
          {
            (*m_CellPhases)[currentvlist[k]] = maxPhase + 1;
          }
        }
      }
      if((int32_t)currentvlist.size() < m_InputValues->minAllowedDefectSizeValue)
      {
        for(std::vector<int64_t>::size_type k = 0; k < currentvlist.size(); k++)
        {
          m_FeatureIds[currentvlist[k]] = -1;
        }
      }
      currentvlist.clear();
    }
  }

  int32_t current = 0;
  int32_t most = 0;
  std::vector<int32_t> n(numfeatures + 1, 0);

  while(count != 0)
  {
    count = 0;
    for(size_t i = 0; i < totalPoints; i++)
    {
      featurename = m_FeatureIds[i];
      if(featurename < 0)
      {
        count++;
        current = 0;
        most = 0;
        x = static_cast<float>(i % dims[0]);
        y = static_cast<float>((i / dims[0]) % dims[1]);
        z = static_cast<float>(i / (dims[0] * dims[1]));
        for(int32_t j = 0; j < 6; j++)
        {
          good = 1;
          neighpoint = static_cast<int64_t>(i + neighpoints[j]);
          if(j == 0 && z == 0)
          {
            good = 0;
          }
          if(j == 5 && z == (dims[2] - 1))
          {
            good = 0;
          }
          if(j == 1 && y == 0)
          {
            good = 0;
          }
          if(j == 4 && y == (dims[1] - 1))
          {
            good = 0;
          }
          if(j == 2 && x == 0)
          {
            good = 0;
          }
          if(j == 3 && x == (dims[0] - 1))
          {
            good = 0;
          }
          if(good == 1)
          {
            feature = m_FeatureIds[neighpoint];
            if(feature > 0)
            {
              n[feature]++;
              current = n[feature];
              if(current > most)
              {
                most = current;
                m_Neighbors[i] = neighpoint;
              }
            }
          }
        }
        for(int32_t l = 0; l < 6; l++)
        {
          good = 1;
          neighpoint = i + neighpoints[l];
          if(l == 0 && z == 0)
          {
            good = 0;
          }
          if(l == 5 && z == (dims[2] - 1))
          {
            good = 0;
          }
          if(l == 1 && y == 0)
          {
            good = 0;
          }
          if(l == 4 && y == (dims[1] - 1))
          {
            good = 0;
          }
          if(l == 2 && x == 0)
          {
            good = 0;
          }
          if(l == 3 && x == (dims[0] - 1))
          {
            good = 0;
          }
          if(good == 1)
          {
            feature = m_FeatureIds[neighpoint];
            if(feature > 0)
            {
              n[feature] = 0;
            }
          }
        }
      }
    }

    std::optional<std::vector<DataPath>> allChildArrays = GetAllChildDataPaths(m_DataStructure, selectedImageGeom.getCellDataPath(), DataObject::Type::DataArray, m_InputValues->ignoredDataArrayPaths);
    std::vector<DataPath> voxelArrayNames;
    if(allChildArrays.has_value())
    {
      voxelArrayNames = allChildArrays.value();
    }

    for(const auto& cellArrayPath : voxelArrayNames)
    {
      if(cellArrayPath == m_InputValues->featureIdsArrayPath)
      {
        continue;
      }
      auto& oldCellArray = m_DataStructure.getDataRefAs<IDataArray>(cellArrayPath);
      const DataType type = oldCellArray.getDataType();

      //      ExecuteDataFunction(FillBadDataFunctor{}, type, featureIds, oldCellArray, neighbors);

      switch(type)
      {
      case DataType::boolean: {
        FillBadDataUpdateTuples<bool>(m_FeatureIds, dynamic_cast<BoolArray&>(oldCellArray), m_Neighbors)();
        break;
      }
      case DataType::int8: {
        FillBadDataUpdateTuples<int8>(m_FeatureIds, dynamic_cast<Int8Array&>(oldCellArray), m_Neighbors)();
        break;
      }
      case DataType::int16: {
        FillBadDataUpdateTuples<int16>(m_FeatureIds, dynamic_cast<Int16Array&>(oldCellArray), m_Neighbors)();
        break;
      }
      case DataType::int32: {
        FillBadDataUpdateTuples<int32>(m_FeatureIds, dynamic_cast<Int32Array&>(oldCellArray), m_Neighbors)();
        break;
      }
      case DataType::int64: {
        FillBadDataUpdateTuples<int64>(m_FeatureIds, dynamic_cast<Int64Array&>(oldCellArray), m_Neighbors)();
        break;
      }
      case DataType::uint8: {
        FillBadDataUpdateTuples<uint8>(m_FeatureIds, dynamic_cast<UInt8Array&>(oldCellArray), m_Neighbors)();
        break;
      }
      case DataType::uint16: {
        FillBadDataUpdateTuples<uint16>(m_FeatureIds, dynamic_cast<UInt16Array&>(oldCellArray), m_Neighbors)();
        break;
      }
      case DataType::uint32: {
        FillBadDataUpdateTuples<uint32>(m_FeatureIds, dynamic_cast<UInt32Array&>(oldCellArray), m_Neighbors)();
        break;
      }
      case DataType::uint64: {
        FillBadDataUpdateTuples<uint64>(m_FeatureIds, dynamic_cast<UInt64Array&>(oldCellArray), m_Neighbors)();
        break;
      }
      case DataType::float32: {
        FillBadDataUpdateTuples<float32>(m_FeatureIds, dynamic_cast<Float32Array&>(oldCellArray), m_Neighbors)();
        break;
      }
      case DataType::float64: {
        FillBadDataUpdateTuples<float64>(m_FeatureIds, dynamic_cast<Float64Array&>(oldCellArray), m_Neighbors)();
        break;
      }
      default: {
        throw std::runtime_error("Invalid DataType");
      }
      }
    }

    // We need to update the FeatureIds array _LAST_ since the above operations depend on that values in that array
    FillBadDataUpdateTuples<int32>(m_FeatureIds, dynamic_cast<Int32Array&>(m_FeatureIds), m_Neighbors)();
  }
  return {};
}
