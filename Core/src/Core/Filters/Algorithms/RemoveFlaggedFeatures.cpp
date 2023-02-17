#include "RemoveFlaggedFeatures.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataStore.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/Utilities/DataGroupUtilities.hpp"

using namespace complex;

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
  std::vector<bool> activeObjects = remove_flagged_features();
  if(activeObjects.empty())
  {
    return MakeErrorResult(-45430, "All Features were flagged and would all be removed. The filter has quit.");
  }

  if(m_InputValues->FillRemovedFeatures)
  {
    assign_bad_points();
  }

  auto& featureIds = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureIdsArrayPath);
  auto& flaggedFeatures = m_DataStructure.getDataRefAs<BoolArray>(m_InputValues->FlaggedFeaturesArrayPath);
  DataPath featureGroupPath = m_InputValues->FlaggedFeaturesArrayPath.getParent();
  RemoveInactiveObjects(m_DataStructure, featureGroupPath, activeObjects, featureIds, flaggedFeatures.getNumberOfTuples());

  return {};
}

// -----------------------------------------------------------------------------
void RemoveFlaggedFeatures::assign_bad_points()
{
  auto& featureIds = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureIdsArrayPath);
  auto& flaggedFeatures = m_DataStructure.getDataRefAs<BoolArray>(m_InputValues->FlaggedFeaturesArrayPath);
  auto* imageGeom = m_DataStructure.getDataAs<ImageGeom>(m_InputValues->ImageGeometryPath);

  usize totalPoints = featureIds.getNumberOfTuples();
  SizeVec3 uDims = imageGeom->getDimensions();

  int64 dims[3] = {
      static_cast<int64>(uDims[0]),
      static_cast<int64>(uDims[1]),
      static_cast<int64>(uDims[2]),
  };

  std::vector<int32> neighbors((totalPoints * featureIds.getNumberOfComponents()), -1);

  int64 neighborPoints[6] = {(-dims[0] * dims[1]), (-dims[0]), -1, 1, dims[0],(dims[0] * dims[1])};
  int64 neighborPoint = 0;

  usize shouldLoop = false;
  int64 kStride = 0, jStride = 0;
  int32 featureName, feature, neighbor;
  do
  {
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
          if(featureName < 0)
          {
            shouldLoop = true;
            int32 current = 0;
            int32 most = 0;
            std::vector<int32> n(flaggedFeatures.getNumberOfTuples(), 0);
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

              neighborPoint = count + neighborPoints[l];
              feature = featureIds[neighborPoint];
              if(feature >= 0)
              {
                n[feature]++;
                current = n[feature];
                if(current > most)
                {
                  most = current;
                  neighbors[count] = neighborPoint;
                }
              }
            }
          }
        }
      }
    }

    std::vector<std::shared_ptr<IDataArray>> voxelArrays = GenerateDataArrayList(m_DataStructure, m_InputValues->FeatureIdsArrayPath, m_InputValues->IgnoredDataArrayPaths);
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
  } while(shouldLoop);
}

// -----------------------------------------------------------------------------
std::vector<bool> RemoveFlaggedFeatures::remove_flagged_features()
{
  auto& featureIds = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureIdsArrayPath);
  auto& flaggedFeatures = m_DataStructure.getDataRefAs<BoolArray>(m_InputValues->FlaggedFeaturesArrayPath);

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

    if(!m_InputValues->FillRemovedFeatures)
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
