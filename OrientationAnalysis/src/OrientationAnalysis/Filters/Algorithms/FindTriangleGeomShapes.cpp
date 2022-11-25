#include "FindTriangleGeomShapes.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/Geometry/IGeometry.hpp"
#include "complex/DataStructure/Geometry/TriangleGeom.hpp"

#include "EbsdLib/Core/Orientation.hpp"
#include "EbsdLib/Core/OrientationTransformation.hpp"

using namespace complex;

namespace
{
constexpr double k_ScaleFactor = 1.0;
constexpr usize k_00 = 0;
constexpr usize k_01 = 1;
constexpr usize k_02 = 2;
constexpr usize k_10 = 3;
constexpr usize k_11 = 4;
constexpr usize k_12 = 5;
constexpr usize k_20 = 6;
constexpr usize k_21 = 7;
constexpr usize k_22 = 8;

// -----------------------------------------------------------------------------
template <typename T>
void FindTetrahedronInfo(const std::array<usize, 3>& vertIds, const DataArray<T>& vertPtr, const std::array<T, 3>& centroid, std::array<T, 32>& tetInfo)
{
  std::array<double, 30> coords = {vertPtr[3 * vertIds[0] + 0],
                                   vertPtr[3 * vertIds[0] + 1],
                                   vertPtr[3 * vertIds[0] + 2],
                                   vertPtr[3 * vertIds[1] + 0],
                                   vertPtr[3 * vertIds[1] + 1],
                                   vertPtr[3 * vertIds[1] + 2],
                                   vertPtr[3 * vertIds[2] + 0],
                                   vertPtr[3 * vertIds[2] + 1],
                                   vertPtr[3 * vertIds[2] + 2],
                                   centroid[0],
                                   centroid[1],
                                   centroid[2],
                                   0.5 * (vertPtr[3 * vertIds[0] + 0] + centroid[0]),
                                   0.5 * (vertPtr[3 * vertIds[0] + 1] + centroid[1]),
                                   0.5 * (vertPtr[3 * vertIds[0] + 2] + centroid[2]),
                                   0.5 * (vertPtr[3 * vertIds[1] + 0] + centroid[0]),
                                   0.5 * (vertPtr[3 * vertIds[1] + 1] + centroid[1]),
                                   0.5 * (vertPtr[3 * vertIds[1] + 2] + centroid[2]),
                                   0.5 * (vertPtr[3 * vertIds[2] + 0] + centroid[0]),
                                   0.5 * (vertPtr[3 * vertIds[2] + 1] + centroid[1]),
                                   0.5 * (vertPtr[3 * vertIds[2] + 2] + centroid[2]),
                                   0.5 * (vertPtr[3 * vertIds[0] + 0] + vertPtr[3 * vertIds[1] + 0]),
                                   0.5 * (vertPtr[3 * vertIds[0] + 1] + vertPtr[3 * vertIds[1] + 1]),
                                   0.5 * (vertPtr[3 * vertIds[0] + 2] + vertPtr[3 * vertIds[1] + 2]),
                                   0.5 * (vertPtr[3 * vertIds[1] + 0] + vertPtr[3 * vertIds[2] + 0]),
                                   0.5 * (vertPtr[3 * vertIds[1] + 1] + vertPtr[3 * vertIds[2] + 1]),
                                   0.5 * (vertPtr[3 * vertIds[1] + 2] + vertPtr[3 * vertIds[2] + 2]),
                                   0.5 * (vertPtr[3 * vertIds[2] + 0] + vertPtr[3 * vertIds[0] + 0]),
                                   0.5 * (vertPtr[3 * vertIds[2] + 1] + vertPtr[3 * vertIds[0] + 1]),
                                   0.5 * (vertPtr[3 * vertIds[2] + 2] + vertPtr[3 * vertIds[0] + 2])};
  // clang-format off
  std::array<int32_t, 32> tets = {
       4, 5,  6, 3,
       0, 7, 9, 4,
       1, 8, 7, 5,
       2, 9, 8, 6,
       7, 5, 6, 4,
       6, 9, 7, 4,
       6, 5, 7, 8,
       7, 9, 6, 8
      };
  // clang-format on

  for(size_t iter = 0; iter < 8; iter++)
  {
    T ax = coords[3 * tets[4 * iter + 0] + 0];
    T ay = coords[3 * tets[4 * iter + 0] + 1];
    T az = coords[3 * tets[4 * iter + 0] + 2];
    T bx = coords[3 * tets[4 * iter + 1] + 0];
    T by = coords[3 * tets[4 * iter + 1] + 1];
    T bz = coords[3 * tets[4 * iter + 1] + 2];
    T cx = coords[3 * tets[4 * iter + 2] + 0];
    T cy = coords[3 * tets[4 * iter + 2] + 1];
    T cz = coords[3 * tets[4 * iter + 2] + 2];
    T dx = coords[3 * tets[4 * iter + 3] + 0];
    T dy = coords[3 * tets[4 * iter + 3] + 1];
    T dz = coords[3 * tets[4 * iter + 3] + 2];

    std::array<T, 9> volumeMatrix = {bx - ax, cx - ax, dx - ax, by - ay, cy - ay, dy - ay, bz - az, cz - az, dz - az};

    T determinant = (volumeMatrix[k_00] * (volumeMatrix[k_11] * volumeMatrix[k_22] - volumeMatrix[k_12] * volumeMatrix[k_21])) -
                    (volumeMatrix[k_01] * (volumeMatrix[k_10] * volumeMatrix[k_22] - volumeMatrix[k_12] * volumeMatrix[k_20])) +
                    (volumeMatrix[k_02] * (volumeMatrix[k_10] * volumeMatrix[k_21] - volumeMatrix[k_11] * volumeMatrix[k_20]));

    tetInfo[4 * iter + 0] = (determinant / 6.0f);
    tetInfo[4 * iter + 1] = 0.25 * (ax + bx + cx + dx);
    tetInfo[4 * iter + 2] = 0.25 * (ay + by + cy + dy);
    tetInfo[4 * iter + 3] = 0.25 * (az + bz + cz + dz);
  }
}

} // namespace

// -----------------------------------------------------------------------------
void FindTriangleGeomShapes::findMoments()
{
  using MeshIndexType = IGeometry::MeshIndexType;
  using SharedVertexListType = IGeometry::SharedVertexList;

  const auto& triangles = m_DataStructure.getDataRefAs<TriangleGeom>(m_InputValues->TriangleGeometryPath);
  const SharedVertexListType& vertPtr = triangles.getVerticesRef();
  const Int32Array& faceLabels = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FaceLabelsArrayPath);
  const Float32Array& centroids = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->CentroidsArrayPath);
  const Float32Array& volumes = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->VolumesArrayPath);

  auto& omega3S = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->Omega3sArrayPath);

  size_t numFaces = faceLabels.getNumberOfTuples();
  size_t numFeatures = centroids.getNumberOfTuples();
  m_FeatureMoments.resize(numFeatures * 6, 0.0);

  std::array<float, 3> centroid = {0.0F, 0.0F, 0.0F};
  std::array<float, 32> tetInfo = {0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
                                   0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};
  std::array<usize, 3> vertIds = {0, 0, 0};

  for(size_t i = 0; i < numFaces; i++)
  {
    triangles.getFacePointIds(i, vertIds);
    for(size_t j = 0; j < 2; j++)
    {
      if(j == 1)
      {
        std::swap(vertIds[2], vertIds[1]);
      }
      int32_t gnum = faceLabels[2 * i + j];
      if(gnum > 0)
      {
        centroid[0] = centroids[3 * gnum + 0];
        centroid[1] = centroids[3 * gnum + 1];
        centroid[2] = centroids[3 * gnum + 2];
        FindTetrahedronInfo<float32>(vertIds, vertPtr, centroid, tetInfo);
        for(size_t iter = 0; iter < 8; iter++)
        {
          double xdist = (tetInfo[4 * iter + 1] - centroids[gnum * 3 + 0]);
          double ydist = (tetInfo[4 * iter + 2] - centroids[gnum * 3 + 1]);
          double zdist = (tetInfo[4 * iter + 3] - centroids[gnum * 3 + 2]);

          auto xx = static_cast<float>((ydist * ydist) + (zdist * zdist));
          auto yy = static_cast<float>((xdist * xdist) + (zdist * zdist));
          auto zz = static_cast<float>((xdist * xdist) + (ydist * ydist));
          auto xy = static_cast<float>(xdist * ydist);
          auto yz = static_cast<float>(ydist * zdist);
          auto xz = static_cast<float>(xdist * zdist);

          m_FeatureMoments[gnum * 6 + 0] = m_FeatureMoments[gnum * 6 + 0] + (xx * tetInfo[4 * iter + 0]);
          m_FeatureMoments[gnum * 6 + 1] = m_FeatureMoments[gnum * 6 + 1] + (yy * tetInfo[4 * iter + 0]);
          m_FeatureMoments[gnum * 6 + 2] = m_FeatureMoments[gnum * 6 + 2] + (zz * tetInfo[4 * iter + 0]);
          m_FeatureMoments[gnum * 6 + 3] = m_FeatureMoments[gnum * 6 + 3] + (xy * tetInfo[4 * iter + 0]);
          m_FeatureMoments[gnum * 6 + 4] = m_FeatureMoments[gnum * 6 + 4] + (yz * tetInfo[4 * iter + 0]);
          m_FeatureMoments[gnum * 6 + 5] = m_FeatureMoments[gnum * 6 + 5] + (xz * tetInfo[4 * iter + 0]);
        }
      }
    }
  }

  const double k_Sphere = (2000.0 * M_PI * M_PI) / 9.0;
  for(size_t i = 1; i < numFeatures; i++)
  {
    double vol5 = pow(volumes[i], 5.0);
    m_FeatureMoments[i * 6 + 3] = -m_FeatureMoments[i * 6 + 3];
    m_FeatureMoments[i * 6 + 4] = -m_FeatureMoments[i * 6 + 4];
    m_FeatureMoments[i * 6 + 5] = -m_FeatureMoments[i * 6 + 5];
    auto u200 = static_cast<float>((m_FeatureMoments[i * 6 + 1] + m_FeatureMoments[i * 6 + 2] - m_FeatureMoments[i * 6 + 0]) / 2.0f);
    auto u020 = static_cast<float>((m_FeatureMoments[i * 6 + 0] + m_FeatureMoments[i * 6 + 2] - m_FeatureMoments[i * 6 + 1]) / 2.0f);
    auto u002 = static_cast<float>((m_FeatureMoments[i * 6 + 0] + m_FeatureMoments[i * 6 + 1] - m_FeatureMoments[i * 6 + 2]) / 2.0f);
    auto u110 = static_cast<float>(-m_FeatureMoments[i * 6 + 3]);
    auto u011 = static_cast<float>(-m_FeatureMoments[i * 6 + 4]);
    auto u101 = static_cast<float>(-m_FeatureMoments[i * 6 + 5]);
    auto o3 = static_cast<double>((u200 * u020 * u002) + (2.0f * u110 * u101 * u011) - (u200 * u011 * u011) - (u020 * u101 * u101) - (u002 * u110 * u110));
    double omega3 = vol5 / o3;
    omega3 = omega3 / k_Sphere;
    if(omega3 > 1)
    {
      omega3 = 1.0;
    }
    if(vol5 == 0.0)
    {
      omega3 = 0.0;
    }
    omega3S[i] = static_cast<float>(omega3);
  }
}

// -----------------------------------------------------------------------------
void FindTriangleGeomShapes::findAxes()
{
  //  double I1 = 0.0, I2 = 0.0, I3 = 0.0;
  //  double a = 0.0, b = 0.0, c = 0.0, d = 0.0, f = 0.0, g = 0.0, h = 0.0;
  //  double rsquare = 0.0, r = 0.0, theta = 0.0;
  //  double A = 0.0, B = 0.0, C = 0.0;
  //  double r1 = 0.0, r2 = 0.0, r3 = 0.0;
  //  float bovera = 0.0f, covera = 0.0f;
  //  double value = 0.0;

  const Float32Array& centroids = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->CentroidsArrayPath);

  auto& axisLengths = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->AxisLengthsArrayPath);
  auto& aspectRatios = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->AspectRatiosArrayPath);

  size_t numFeatures = centroids.getNumberOfTuples();

  m_FeatureEigenVals.resize(numFeatures * 3);

  for(size_t i = 1; i < numFeatures; i++)
  {
    double ixx = m_FeatureMoments[i * 6 + 0];
    double iyy = m_FeatureMoments[i * 6 + 1];
    double izz = m_FeatureMoments[i * 6 + 2];

    double ixy = m_FeatureMoments[i * 6 + 3];
    double iyz = m_FeatureMoments[i * 6 + 4];
    double ixz = m_FeatureMoments[i * 6 + 5];

    double a = 1.0;
    double b = (-ixx - iyy - izz);
    double c = ((ixx * izz) + (ixx * iyy) + (iyy * izz) - (ixz * ixz) - (ixy * ixy) - (iyz * iyz));
    double d = ((ixz * iyy * ixz) + (ixy * izz * ixy) + (iyz * ixx * iyz) - (ixx * iyy * izz) - (ixy * iyz * ixz) - (ixy * iyz * ixz));

    // f and g are the p and q values when reducing the cubic equation to t^3 + pt + q = 0
    double f = ((3.0 * c / a) - ((b / a) * (b / a))) / 3.0;
    double g = ((2.0 * (b / a) * (b / a) * (b / a)) - (9.0 * b * c / (a * a)) + (27.0 * (d / a))) / 27.0;
    double h = (g * g / 4.0) + (f * f * f / 27.0);
    double rSquare = (g * g / 4.0) - h;
    double r = sqrt(rSquare);
    if(rSquare < 0.0)
    {
      r = 0.0;
    }
    double theta = 0;
    if(r == 0)
    {
      theta = 0;
    }
    if(r != 0)
    {
      double value = -g / (2.0 * r);
      if(value > 1)
      {
        value = 1.0;
      }
      if(value < -1)
      {
        value = -1.0;
      }
      theta = acos(value);
    }
    double const1 = pow(r, 0.33333333333);
    double const2 = cos(theta / 3.0);
    double const3 = b / (3.0 * a);
    double const4 = 1.7320508 * sin(theta / 3.0);

    double r1 = 2 * const1 * const2 - (const3);
    double r2 = -const1 * (const2 - (const4)) - const3;
    double r3 = -const1 * (const2 + (const4)) - const3;
    m_FeatureEigenVals[3 * i] = r1;
    m_FeatureEigenVals[3 * i + 1] = r2;
    m_FeatureEigenVals[3 * i + 2] = r3;

    double i1 = (15.0 * r1) / (4.0 * M_PI);
    double i2 = (15.0 * r2) / (4.0 * M_PI);
    double i3 = (15.0 * r3) / (4.0 * M_PI);
    double aRatio = (i1 + i2 - i3) / 2.0f;
    double bRatio = (i1 + i3 - i2) / 2.0f;
    double cRatio = (i2 + i3 - i1) / 2.0f;
    a = (aRatio * aRatio * aRatio * aRatio) / (bRatio * cRatio);
    a = pow(a, 0.1);
    b = bRatio / aRatio;
    b = sqrt(b) * a;
    c = aRatio / (a * a * a * b);

    axisLengths[3 * i] = static_cast<float>(a / k_ScaleFactor);
    axisLengths[3 * i + 1] = static_cast<float>(b / k_ScaleFactor);
    axisLengths[3 * i + 2] = static_cast<float>(c / k_ScaleFactor);
    auto bOverA = static_cast<float>(b / a);
    auto cOverA = static_cast<float>(c / a);
    if(aRatio == 0 || bRatio == 0 || cRatio == 0)
    {
      bOverA = 0.0f, cOverA = 0.0f;
    }
    aspectRatios[2 * i] = bOverA;
    aspectRatios[2 * i + 1] = cOverA;
  }
}

// -----------------------------------------------------------------------------
void FindTriangleGeomShapes::findAxisEulers()
{
  const Float32Array& centroids = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->CentroidsArrayPath);
  size_t numFeatures = centroids.getNumberOfTuples();

  auto& axisEulerAngles = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->AxisEulerAnglesArrayPath);

  for(size_t i = 1; i < numFeatures; i++)
  {
    double ixx = m_FeatureMoments[i * 6 + 0];
    double iyy = m_FeatureMoments[i * 6 + 1];
    double izz = m_FeatureMoments[i * 6 + 2];
    double ixy = m_FeatureMoments[i * 6 + 3];
    double iyz = m_FeatureMoments[i * 6 + 4];
    double ixz = m_FeatureMoments[i * 6 + 5];
    double radius1 = m_FeatureEigenVals[3 * i];
    double radius2 = m_FeatureEigenVals[3 * i + 1];
    double radius3 = m_FeatureEigenVals[3 * i + 2];

    double e[3][1];
    double vect[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    e[0][0] = radius1;
    e[1][0] = radius2;
    e[2][0] = radius3;
    double uber[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    double bMatrix[3];
    bMatrix[0] = 0.0000001;
    bMatrix[1] = 0.0000001;
    bMatrix[2] = 0.0000001;

    for(int32_t j = 0; j < 3; j++)
    {
      uber[0][0] = ixx - e[j][0];
      uber[0][1] = ixy;
      uber[0][2] = ixz;
      uber[1][0] = ixy;
      uber[1][1] = iyy - e[j][0];
      uber[1][2] = iyz;
      uber[2][0] = ixz;
      uber[2][1] = iyz;
      uber[2][2] = izz - e[j][0];
      std::array<std::array<double, 3>, 3> uberelim{};
      std::array<std::array<double, 1>, 3> uberbelim{};
      int32_t elimCount = 0;

      for(int32_t a = 0; a < 3; a++)
      {
        for(int32_t b = 0; b < 3; b++)
        {
          uberelim[elimCount][b] = uber[a][b];
        }
        uberbelim[elimCount][0] = bMatrix[a];
        elimCount++;
      }
      for(int32_t k = 0; k < elimCount - 1; k++)
      {
        for(int32_t l = k + 1; l < elimCount; l++)
        {
          double c = uberelim[l][k] / uberelim[k][k];
          for(int32_t r = k + 1; r < elimCount; r++)
          {
            uberelim[l][r] = uberelim[l][r] - c * uberelim[k][r];
          }
          uberbelim[l][0] = uberbelim[l][0] - c * uberbelim[k][0];
        }
      }
      uberbelim[elimCount - 1][0] = uberbelim[elimCount - 1][0] / uberelim[elimCount - 1][elimCount - 1];
      for(int32_t l = 1; l < elimCount; l++)
      {
        int32_t r = (elimCount - 1) - l;
        double sum = 0.0;
        for(int32_t n = r + 1; n < elimCount; n++)
        {
          sum = sum + (uberelim[r][n] * uberbelim[n][0]);
        }
        uberbelim[r][0] = (uberbelim[r][0] - sum) / uberelim[r][r];
      }
      for(int32_t p = 0; p < elimCount; p++)
      {
        vect[j][p] = uberbelim[p][0];
      }
    }

    double n1X = vect[0][0];
    double n1Y = vect[0][1];
    double n1Z = vect[0][2];
    double n2X = vect[1][0];
    double n2Y = vect[1][1];
    double n2Z = vect[1][2];
    double n3X = vect[2][0];
    double n3Y = vect[2][1];
    double n3Z = vect[2][2];
    double norm1 = sqrt(((n1X * n1X) + (n1Y * n1Y) + (n1Z * n1Z)));
    double norm2 = sqrt(((n2X * n2X) + (n2Y * n2Y) + (n2Z * n2Z)));
    double norm3 = sqrt(((n3X * n3X) + (n3Y * n3Y) + (n3Z * n3Z)));
    n1X = n1X / norm1;
    n1Y = n1Y / norm1;
    n1Z = n1Z / norm1;
    n2X = n2X / norm2;
    n2Y = n2Y / norm2;
    n2Z = n2Z / norm2;
    n3X = n3X / norm3;
    n3Y = n3Y / norm3;
    n3Z = n3Z / norm3;

    // insert principal unit vectors into rotation matrix representing Feature reference frame within the sample reference frame
    //(Note that the 3 direction is actually the long axis and the 1 direction is actually the short axis)
    float g[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    g[0][0] = static_cast<float>(n3X);
    g[0][1] = static_cast<float>(n3Y);
    g[0][2] = static_cast<float>(n3Z);
    g[1][0] = static_cast<float>(n2X);
    g[1][1] = static_cast<float>(n2Y);
    g[1][2] = static_cast<float>(n2Z);
    g[2][0] = static_cast<float>(n1X);
    g[2][1] = static_cast<float>(n1Y);
    g[2][2] = static_cast<float>(n1Z);

    // check for right-handedness
    OrientationTransformation::ResultType result = OrientationTransformation::om_check(OrientationF(g));
    if(result.result == 0)
    {
      g[2][0] *= -1.0f;
      g[2][1] *= -1.0f;
      g[2][2] *= -1.0f;
    }

    auto euler = OrientationTransformation::om2eu<OrientationF, OrientationF>(OrientationF(g));

    axisEulerAngles[3 * i] = euler[0];
    axisEulerAngles[3 * i + 1] = euler[1];
    axisEulerAngles[3 * i + 2] = euler[2];
  }
}

// -----------------------------------------------------------------------------
FindTriangleGeomShapes::FindTriangleGeomShapes(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                               FindTriangleGeomShapesInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
FindTriangleGeomShapes::~FindTriangleGeomShapes() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& FindTriangleGeomShapes::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> FindTriangleGeomShapes::operator()()
{
  findMoments();
  findAxes();
  findAxisEulers();

  return {};
}
