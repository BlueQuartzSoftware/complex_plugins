#include "SharedFeatureFace.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/Geometry/TriangleGeom.hpp"
#include "complex/Utilities/DataArrayUtilities.hpp"
#include "complex/Utilities/ParallelDataAlgorithm.hpp"

#include <map>
#include <vector>

using namespace complex;

namespace
{
//------------------------------------------------------------------------------
__attribute__((noinline)) uint64_t ConvertToUInt64(uint32_t highWord, uint32_t lowWord)
{
  return ((uint64_t)highWord << 32) | lowWord;
}

//------------------------------------------------------------------------------
class SharedFeatureFaceImpl
{
public:
  SharedFeatureFaceImpl(const std::vector<std::pair<int32_t, int32_t>>& faceLabelVector, Int32Array& surfaceMeshFeatureFaceLabels, Int32Array& surfaceMeshFeatureFaceNumTriangles,
                        std::map<uint64_t, int32_t>& faceSizeMap, const std::atomic_bool& shouldCancel)
  : m_FaceLabelVector(faceLabelVector)
  , m_SurfaceMeshFeatureFaceLabels(surfaceMeshFeatureFaceLabels)
  , m_SurfaceMeshFeatureFaceNumTriangles(surfaceMeshFeatureFaceNumTriangles)
  , m_FaceSizeMap(faceSizeMap)
  , m_ShouldCancel(shouldCancel)
  {
  }
  ~SharedFeatureFaceImpl() = default;

  void operator()(const Range& range) const
  {
    for(size_t i = range.min(); i < range.max(); i++)
    {
      const auto& faceLabelMapEntry = m_FaceLabelVector[i];
      // get feature face labels
      m_SurfaceMeshFeatureFaceLabels[2 * i + 0] = faceLabelMapEntry.first;
      m_SurfaceMeshFeatureFaceLabels[2 * i + 1] = faceLabelMapEntry.second;

      // get feature triangle count
      uint64 faceId64 = ConvertToUInt64(faceLabelMapEntry.first, faceLabelMapEntry.second);
      m_SurfaceMeshFeatureFaceNumTriangles[i] = m_FaceSizeMap[faceId64];
      if(m_ShouldCancel)
      {
        break;
      }
    }
  }

private:
  const std::vector<std::pair<int32_t, int32_t>>& m_FaceLabelVector;
  Int32Array& m_SurfaceMeshFeatureFaceLabels;
  Int32Array& m_SurfaceMeshFeatureFaceNumTriangles;
  std::map<uint64_t, int32_t>& m_FaceSizeMap;
  const std::atomic_bool& m_ShouldCancel;
};
} // namespace

// -----------------------------------------------------------------------------
SharedFeatureFace::SharedFeatureFace(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, SharedFeatureFaceInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
SharedFeatureFace::~SharedFeatureFace() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& SharedFeatureFace::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> SharedFeatureFace::operator()()
{

  const auto& triangleGeom = m_DataStructure.getDataRefAs<TriangleGeom>(m_InputValues->TriangleGeometryPath);
  usize totalPoints = triangleGeom.getNumberOfFaces();

  const Int32Array& surfaceMeshFaceLabels = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FaceLabelsArrayPath);

  auto& surfaceMeshFeatureFaceIds = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureFaceIdsArrayPath);

  std::map<uint64_t, int32_t> faceSizeMap;
  std::map<uint64_t, int32_t> faceIdMap; // This maps a unique 64-bit integer to an increasing 32-bit integer
  int32_t index = 1;

  std::vector<std::pair<int32_t, int32_t>> faceLabelVector;
  faceLabelVector.emplace_back(std::pair<int32_t, int32_t>(0, 0));

  // Loop through all the Triangles and figure out how many triangles we have in each one.
  for(usize t = 0; t < totalPoints; ++t)
  {
    uint32 high = 0;
    uint32 low = 0;
    int32_t fl0 = surfaceMeshFaceLabels[t * 2];
    int32_t fl1 = surfaceMeshFaceLabels[t * 2 + 1];
    if(fl0 < fl1)
    {
      high = fl0;
      low = fl1;
    }
    else
    {
      high = fl1;
      low = fl0;
    }
    uint64 faceId64 = ConvertToUInt64(high, low);
    if(faceSizeMap.find(faceId64) == faceSizeMap.end())
    {
      faceSizeMap[faceId64] = 1;
      faceIdMap[faceId64] = index;
      surfaceMeshFeatureFaceIds[t] = index;
      faceLabelVector.emplace_back(std::pair<int32_t, int32_t>(high, low));
      ++index;
    }
    else
    {
      faceSizeMap[faceId64]++;
      surfaceMeshFeatureFaceIds[t] = faceIdMap[faceId64];
    }
  }
  if(m_ShouldCancel)
  {
    return {};
  }
  // resize + update pointers
  // Grain Boundary Attribute Matrix
  auto& faceFeatureAttrMat = m_DataStructure.getDataRefAs<AttributeMatrix>(m_InputValues->GrainBoundaryAttributeMatrixPath);
  std::vector<size_t> tDims = {static_cast<size_t>(index)};
  faceFeatureAttrMat.setShape(tDims);
  ResizeAttributeMatrix(faceFeatureAttrMat, tDims);

  auto& surfaceMeshFeatureFaceLabels = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureFaceLabelsArrayPath);
  auto& surfaceMeshFeatureFaceNumTriangles = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureFaceNumTrianglesArrayPath);
  surfaceMeshFeatureFaceLabels.getDataStore()->reshapeTuples(tDims);
  surfaceMeshFeatureFaceNumTriangles.getDataStore()->reshapeTuples(tDims);

  // For smaller data sets having data parallelization ON actually runs slower due to
  // all the overhead of the threads. We are just going to turn this off for
  // now until we hit a large data set that is better suited for parallelization
  // Allow data-based parallelization
  ParallelDataAlgorithm dataAlg;
  dataAlg.setParallelizationEnabled(true);
  dataAlg.setRange(0, index);
  dataAlg.execute(SharedFeatureFaceImpl(faceLabelVector, surfaceMeshFeatureFaceLabels, surfaceMeshFeatureFaceNumTriangles, faceSizeMap, m_ShouldCancel));

  return {};
}
