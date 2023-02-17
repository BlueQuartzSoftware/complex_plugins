#include "RemoveFlaggedFeatures.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataStore.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/Utilities/DataGroupUtilities.hpp"

using namespace complex;

namespace
{
template <bool UseRemove, bool UseExtract>
struct AlgorithmTypeOptions
{
  static inline constexpr bool UsingRemove = UseRemove;
  static inline constexpr bool UsingExtract = UseExtract;
};

using Remove = AlgorithmTypeOptions<true, false>;
using Extract = AlgorithmTypeOptions<false, true>;

template <class AlgorithmTypeOptions = Remove>
bool assign_bad_points(ImageGeom& imageGeom, Int32Array& featureIds, BoolArray& flaggedfeatures, std::vector<int32>& neighbors)
{
  const usize totalPoints = featureIds.getNumberOfTuples();
  SizeVec3 uDims = imageGeom.getDimensions();

  int64 dims[3] = {static_cast<int64>(uDims[0]), static_cast<int64>(uDims[1]), static_cast<int64>(uDims[2])};

  const int64 neighborPoints[6] = {(-dims[0] * dims[1]), (-dims[0]), -1, 1, dims[0], (dims[0] * dims[1])};

  usize shouldLoop = false;
  int64 kStride = 0, jStride = 0;
  int32 featureName, feature, neighbor;
  shouldLoop = false;
  for(int64 k = 0; k < dims[2]; k++)
  {
    kStride = dims[0] * dims[1] * k;
    for(int64 j = 0; j < dims[1]; j++)
    {
      jStride = dims[0] * j;
      for(int64 i = 0; i < dims[0]; i++)
      {
        int64 count = kStride + jStride + i;
        featureName = featureIds[count];
        if constexpr(AlgorithmTypeOptions::UsingRemove)
        {
          if(featureName > 0)
          {
            continue;
          }
          shouldLoop = true;
          int32 current = 0;
          int32 most = 0;
          std::vector<int32> numHits(6, 0);
          std::vector<int32> discoveredFeatures = {};
          discoveredFeatures.reserve(6);
          for(int8 l = 0; l < 6; l++)
          {
            if(l == 5 && k == (dims[2] - 1))
            {
              continue;
            }
            if(l == 1 && j == 0)
            {
              continue;
            }
            if(l == 4 && j == (dims[1] - 1))
            {
              continue;
            }
            if(l == 2 && i == 0)
            {
              continue;
            }
            if(l == 3 && i == (dims[0] - 1))
            {
              continue;
            }
            if(l == 0 && k == 0)
            {
              continue;
            }

            int64 neighborPoint = count + neighborPoints[l];
            feature = featureIds[neighborPoint];
            if(feature >= 0)
            {
              bool found = false;
              for(usize featIndex = 0; featIndex < discoveredFeatures.size(); featIndex++)
              {
                if(discoveredFeatures[featIndex] == feature)
                {
                  found = true;
                  numHits[featIndex]++;
                  current = numHits[featIndex];
                  if(current > most)
                  {
                    most = current;
                    neighbors[count] = neighborPoint;
                  }
                  break;
                }
              }
              if(!found)
              {
                discoveredFeatures.push_back(feature);
              }
            }
          }
        }
        else if constexpr(AlgorithmTypeOptions::UsingRemove)
        {
          int64 indices[3] = {i, j, k}; // Sequence dependent DO NOT REORDER
          int64 featureShift = featureName * 6;
          for(int8 l = 0; l < 6; l++)
          {
            int64 current = indices[l / 2];
            if(neighbors[featureShift + l] != -1)
            {
              if(l % 2 == 0)
              {
                if(neighbors[featureShift + l] <= current)
                {
                  continue;
                }
              }
              else
              {
                if(neighbors[featureShift + l] >= current)
                {
                  continue;
                }
              }
            }
            neighbors[featureShift + l] = current;
          }
        }
      }
    }
  }
  return shouldLoop;
}

std::vector<bool> remove_flagged_features(Int32Array& featureIds, BoolArray& flaggedFeatures, const bool fillRemovedFeatures)
{
  bool good = false;
  usize totalPoints = featureIds.getNumberOfTuples();
  usize totalFeatures = flaggedFeatures.getNumberOfTuples();
  std::vector<bool> activeObjects(totalFeatures, true);
  for(usize i = 1; i < totalFeatures; i++)
  {
    if(!flaggedFeatures[i])
    {
      good = true;
    }
    else
    {
      activeObjects[i] = false;
    }
  }
  if(!good)
  {
    return {};
  }
  for(usize i = 0; i < totalPoints; i++)
  {
    if(activeObjects[featureIds[i]])
    {
      continue;
    }

    if(!fillRemovedFeatures)
    {
      featureIds[i] = 0;
    }
    else
    {
      featureIds[i] = -1;
    }
  }
  return activeObjects;
}

void FindVoxelArrays(const Int32Array& featureIds, const std::vector<int32>& neighbors, std::vector<std::shared_ptr<IDataArray>>& voxelArrays)
{
  const usize totalPoints = featureIds.getNumberOfTuples();

  int32 featureName, neighbor;
  for(usize j = 0; j < totalPoints; j++)
  {
    featureName = featureIds[j];
    neighbor = neighbors[j];
    if(neighbor >= 0)
    {
      if(featureName < 0 && featureIds[neighbor] >= 0)
      {
        for(const auto& voxelArray : voxelArrays)
        {
          voxelArray->copyTuple(neighbor, j);
        }
      }
    }
  }
}

} // namespace

// -----------------------------------------------------------------------------
RemoveFlaggedFeatures::RemoveFlaggedFeatures(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                             RemoveFlaggedFeaturesInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
RemoveFlaggedFeatures::~RemoveFlaggedFeatures() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& RemoveFlaggedFeatures::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> RemoveFlaggedFeatures::operator()()
{
  auto& featureIds = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureIdsArrayPath);
  auto& flaggedFeatures = m_DataStructure.getDataRefAs<BoolArray>(m_InputValues->FlaggedFeaturesArrayPath);
  auto& imageGeom = m_DataStructure.getDataRefAs<ImageGeom>(m_InputValues->ImageGeometryPath);

  std::vector<bool> activeObjects = remove_flagged_features(featureIds, flaggedFeatures, m_InputValues->FillRemovedFeatures); // review
  if(activeObjects.empty())
  {
    return MakeErrorResult(-45430, "All Features were flagged and would all be removed. The filter has quit.");
  }

  if(m_InputValues->FillRemovedFeatures)
  {
    std::vector<int32> neighbors((featureIds.getNumberOfTuples() * featureIds.getNumberOfComponents()), -1);
    bool shouldLoop = false;
    do
    {
      shouldLoop = assign_bad_points(imageGeom, featureIds, flaggedFeatures, neighbors);

      std::vector<std::shared_ptr<IDataArray>> voxelArrays = GenerateDataArrayList(m_DataStructure, m_InputValues->FeatureIdsArrayPath, m_InputValues->IgnoredDataArrayPaths);
      FindVoxelArrays(featureIds, neighbors, voxelArrays);
    } while(shouldLoop);
  }

  DataPath featureGroupPath = m_InputValues->FlaggedFeaturesArrayPath.getParent();
  RemoveInactiveObjects(m_DataStructure, featureGroupPath, activeObjects, featureIds, flaggedFeatures.getNumberOfTuples());

  return {};
}
