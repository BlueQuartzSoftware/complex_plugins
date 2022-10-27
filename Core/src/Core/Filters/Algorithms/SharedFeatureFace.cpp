#include "SharedFeatureFace.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/Geometry/TriangleGeom.hpp"

#include <map>

using namespace complex;

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

  const Int32Array& m_SurfaceMeshFaceLabels = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FaceLabelsArrayPath);

  auto& m_SurfaceMeshFeatureFaceIds = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureFaceIdsArrayPath);

  std::map<uint64_t, int32_t> faceSizeMap;
  std::map<uint64_t, int32_t> faceIdMap; // This maps a unique 64-bit integer to an increasing 32-bit integer
  int32_t index = 1;
  struct
  {
    int g = 0;
    int r = 0;
  } faceId;
  auto* faceId_64 = reinterpret_cast<uint64_t*>(&faceId);

  std::vector<std::pair<int32_t, int32_t>> faceLabelMap;
  faceLabelMap.emplace_back(std::pair<int32_t, int32_t>(0, 0));

  // Loop through all the Triangles and figure out how many triangles we have in each one.
  for(int64_t t = 0; t < totalPoints; ++t)
  {
    int32_t fl0 = m_SurfaceMeshFaceLabels[t * 2];
    int32_t fl1 = m_SurfaceMeshFaceLabels[t * 2 + 1];
    if(fl0 < fl1)
    {
      faceId.g = fl0;
      faceId.r = fl1;
    }
    else
    {
      faceId.g = fl1;
      faceId.r = fl0;
    }

    if(faceSizeMap.find(*faceId_64) == faceSizeMap.end())
    {
      faceSizeMap[*faceId_64] = 1;
      faceIdMap[*faceId_64] = index;
      m_SurfaceMeshFeatureFaceIds[t] = index;
      faceLabelMap.emplace_back(std::pair<int32_t, int32_t>(faceId.g, faceId.r));
      ++index;
    }
    else
    {
      faceSizeMap[*faceId_64]++;
      m_SurfaceMeshFeatureFaceIds[t] = faceIdMap[*faceId_64];
    }
  }

  // resize + update pointers
  // Grain Boundary Attribute Matrix
  auto& faceFeatureAttrMat = m_DataStructure.getDataRefAs<AttributeMatrix>(m_InputValues->GrainBoundaryAttributeMatrixPath);
  std::vector<size_t> tDims = {static_cast<size_t>(index)};
  faceFeatureAttrMat.setShape(tDims);

  auto& m_SurfaceMeshFeatureFaceLabels = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureFaceLabelsArrayPath);
  auto& m_SurfaceMeshFeatureFaceNumTriangles = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureFaceNumTrianglesArrayPath);
  m_SurfaceMeshFeatureFaceLabels.getDataStore()->reshapeTuples(tDims);
  m_SurfaceMeshFeatureFaceNumTriangles.getDataStore()->reshapeTuples(tDims);

  for(int32_t i = 0; i < index; i++)
  {
    // get feature face labels
    m_SurfaceMeshFeatureFaceLabels[2 * i + 0] = faceLabelMap[i].first;
    m_SurfaceMeshFeatureFaceLabels[2 * i + 1] = faceLabelMap[i].second;

    // get feature triangle count
    faceId.g = faceLabelMap[i].first;
    faceId.r = faceLabelMap[i].second;
    m_SurfaceMeshFeatureFaceNumTriangles[i] = faceSizeMap[*faceId_64];
  }
  return {};
}
