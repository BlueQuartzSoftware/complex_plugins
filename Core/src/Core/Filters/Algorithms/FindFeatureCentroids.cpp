#include "FindFeatureCentroids.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
FindFeatureCentroids::FindFeatureCentroids(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, FindFeatureCentroidsInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
FindFeatureCentroids::~FindFeatureCentroids() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& FindFeatureCentroids::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> FindFeatureCentroids::operator()()
{

  const auto& featureIds = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureIdsArrayPath);
  const auto& imageGeom = m_DataStructure.getDataRefAs<ImageGeom>(m_InputValues->ImageGeometryPath);
  auto& centroids = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->CentroidsArrayPath);


  size_t totalFeatures = centroids.getNumberOfTuples();

  size_t xPoints = imageGeom.getNumXPoints();
  size_t yPoints = imageGeom.getNumYPoints();
  size_t zPoints = imageGeom.getNumZPoints();

  //Copy the Coords into an array and then compute the sum

  std::vector<double> sum(totalFeatures * 3, 0.0);
  std::vector<double> center(totalFeatures * 3, 0.0);
  std::vector<size_t> count(totalFeatures * 3, 0.0);

  for(size_t i = 0; i < zPoints; i++)
  {
    size_t zStride = i * xPoints * yPoints;
    for(size_t j = 0; j < yPoints; j++)
    {
      size_t yStride = j * xPoints;
      for(size_t k = 0; k < xPoints; k++)
      {
        int32_t featureId = featureIds[zStride + yStride + k]; // Get the current FeatureId

        complex::Point3Dd voxel_center = imageGeom.getCoords(k, j, i); // Get the voxel center based on XYZ index from Image Geom

        //Kahan Sum for X Coord
        size_t featureId_idx = featureId * 3ULL;
        auto componentValue = static_cast<double>(voxel_center[0] - center[featureId_idx]);
        double temp = sum[featureId_idx] + componentValue;
        center[featureId_idx] = (temp - sum[featureId_idx]) - componentValue;
        sum[featureId_idx] = temp;
        count[featureId_idx]++;

        //Kahan Sum for Y Coord
        featureId_idx  = featureId * 3ULL + 1;
        componentValue = static_cast<double>(voxel_center[1] - center[featureId_idx]);
        temp = sum[featureId_idx] + componentValue;
        center[featureId_idx] = (temp - sum[featureId_idx]) - componentValue;
        sum[featureId_idx] = temp;
        count[featureId_idx]++;

        //Kahan Sum for Z Coord
        featureId_idx = featureId * 3ULL + 2;
        componentValue = static_cast<double>(voxel_center[2] - center[featureId_idx]);
        temp = sum[featureId_idx] + componentValue;
        center[featureId_idx] = (temp - sum[featureId_idx]) - componentValue;
        sum[featureId_idx] = temp;
        count[featureId_idx]++;
      }
    }
  }

  for(size_t featureId = 0; featureId < totalFeatures; featureId++)
  {
    auto featureId_idx = static_cast<size_t>(featureId * 3);
    if(static_cast<float>(count[featureId_idx]) > 0.0f)
    {
      centroids[featureId_idx] = static_cast<float>(sum[featureId_idx] / static_cast<double>(count[featureId_idx]));
    }

    featureId_idx = static_cast<size_t>(featureId * 3 + 1);
    if(static_cast<float>(count[featureId_idx]) > 0.0f)
    {
      centroids[featureId_idx] = static_cast<float>(sum[featureId_idx] / static_cast<double>(count[featureId_idx]));
    }

    featureId_idx = static_cast<size_t>(featureId * 3 + 2);
    if(static_cast<float>(count[featureId_idx]) > 0.0f)
    {
      centroids[featureId_idx] = static_cast<float>(sum[featureId_idx] / static_cast<double>(count[featureId_idx]));
    }
  }

  return {};
}
