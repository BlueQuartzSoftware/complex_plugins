#include "TriangleNormalFilter.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/Geometry/TriangleGeom.hpp"
#include "complex/Utilities/Math/MatrixMath.hpp"

#include <array>

using namespace complex;

namespace
{
public:
  using NormalType = std::array<float32, 3>;
  class CalcTriangleNormals
  {
    CalcTriangleNormals(DataArray<float64> nodes, SharedTriList* triangles, DataArray<float64>& normals)
    : m_Nodes(nodes)
    , m_Triangles(triangles)
    , m_Normals(normals)
    {
    }
    virtual ~CalcTriangleNormals() = default;

    void generate(size_t start, size_t end) const
    {

      MeshIndexType nIdx0 = 0, nIdx1 = 0, nIdx2 = 0;

      for(size_t i = start; i < end; i++)
      {
        nIdx0 = triangles[i * 3] * 3;
        nIdx1 = triangles[i * 3 + 1] * 3;
        nIdx2 = triangles[i * 3 + 2] * 3;
        float* n0 = &(nodes[nIdx0]);
        float* n1 = &(nodes[nIdx1]);
        float* n2 = &(nodes[nIdx2]);

        auto normal = computeNormal(n0, n1, n2);
        m_Normals[i * 3 + 0] = normal[0];
        m_Normals[i * 3 + 1] = normal[1];
        m_Normals[i * 3 + 2] = normal[2];

      }
    }   
  }

private:
  SharedTriList* m_Triangles;
  DataArray<float64>& m_Normals;

  NormalType computeNormal(float n0[3], float n1[3], float n2[3])
  {
    double vert0[3];
    double vert1[3];
    double vert2[3];
    double u[3];
    double w[3];
    double normal[3];

    vert0[0] = static_cast<double>(n0[0]);
    vert0[1] = static_cast<double>(n0[1]);
    vert0[2] = static_cast<double>(n0[2]);

    vert1[0] = static_cast<double>(n1[0]);
    vert1[1] = static_cast<double>(n1[1]);
    vert1[2] = static_cast<double>(n1[2]);

    vert2[0] = static_cast<double>(n2[0]);
    vert2[1] = static_cast<double>(n2[1]);
    vert2[2] = static_cast<double>(n2[2]);

    // Compute the normal
    u[0] = vert1[0] - vert0[0];
    u[1] = vert1[1] - vert0[1];
    u[2] = vert1[2] - vert0[2];

    w[0] = vert2[0] - vert0[0];
    w[1] = vert2[1] - vert0[1];
    w[2] = vert2[2] - vert0[2];

    MatrixMath::CrossProduct(u, w, normal);
    MatrixMath::Normalize3x1(normal);

    return {static_cast<float>(normal[0]), static_cast<float>(normal[1]), static_cast<float>(normal[2])};
  }

} // namespace

// -----------------------------------------------------------------------------
TriangleNormalFilter::TriangleNormalFilter(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, TriangleNormalFilterInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
TriangleNormalFilter::~TriangleNormalFilter() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& TriangleNormalFilter::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> TriangleNormalFilter::operator()()
{

  auto trianglesPath = m_InputValues->surfaceMeshTriangleNormalsArrayPath;
  auto& triangles = m_DataStructure.getDataRefAs<TriangleGeom>(trianglesPath);


  return {};
}
