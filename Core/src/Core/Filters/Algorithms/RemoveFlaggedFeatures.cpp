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

  size_t totalPoints = featureIds.getNumberOfTuples();
  SizeVec3 uDims = imageGeom->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(uDims[0]),
      static_cast<int64_t>(uDims[1]),
      static_cast<int64_t>(uDims[2]),
  };

  DataStructure internalDataStruct;
  Int32Array* neighbors = Int32Array::CreateWithStore<DataStore<int32>>(internalDataStruct, std::string("_INTERNAL_USE_ONLY_Neighbors"), featureIds.getTupleShape(), featureIds.getComponentShape());
  neighbors->fill(-1);

  int64_t neighpoints[6] = {(-dims[0] * dims[1]), (-dims[0]), -1, 1, dims[0],(dims[0] * dims[1])};
  int64_t neighpoint = 0;

  size_t shouldLoop = false;
  int64_t kStride = 0, jStride = 0;
  int32_t featurename, feature;
  int32_t neighbor;
  do
  {
    shouldLoop = false;
    for(int64_t k = 0; k < dims[2]; k++)
    {
      kStride = dims[0] * dims[1] * k;
      for(int64_t j = 0; j < dims[1]; j++)
      {
        jStride = dims[0] * j;
        for(int64_t i = 0; i < dims[0]; i++)
        {
          int64_t count = kStride + jStride + i;
          featurename = featureIds[count];
          if(featurename < 0)
          {
            shouldLoop = true;
            int32_t current = 0;
            int32_t most = 0;
            std::vector<int32_t> n(flaggedFeatures.getNumberOfTuples(), 0);
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

              neighpoint = count + neighpoints[l];
              feature = featureIds[neighpoint];
              if(feature >= 0)
              {
                n[feature]++;
                current = n[feature];
                if(current > most)
                {
                  most = current;
                  (*neighbors)[count] = neighpoint;
                }
              }
            }
          }
        }
      }
    }

    std::vector<std::shared_ptr<IDataArray>> voxelArrays = GenerateDataArrayList(m_DataStructure, m_InputValues->FeatureIdsArrayPath, m_InputValues->IgnoredDataArrayPaths);
    for(size_t j = 0; j < totalPoints; j++)
    {
      featurename = featureIds[j];
      neighbor = (*neighbors)[j];
      if(neighbor >= 0)
      {
        if(featurename < 0 && featureIds[neighbor] >= 0)
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
  size_t totalPoints = featureIds.getNumberOfTuples();
  size_t totalFeatures = flaggedFeatures.getNumberOfTuples();
  std::vector<bool> activeObjects(totalFeatures, true);
  for(size_t i = 1; i < totalFeatures; i++)
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
  for(size_t i = 0; i < totalPoints; i++)
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
