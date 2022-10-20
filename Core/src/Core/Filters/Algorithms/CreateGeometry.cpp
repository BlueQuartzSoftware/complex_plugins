#include "CreateGeometry.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/Geometry/EdgeGeom.hpp"
#include "complex/DataStructure/Geometry/HexahedralGeom.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/DataStructure/Geometry/QuadGeom.hpp"
#include "complex/DataStructure/Geometry/TetrahedralGeom.hpp"
#include "complex/DataStructure/Geometry/TriangleGeom.hpp"
#include "complex/DataStructure/Geometry/VertexGeom.hpp"
#include "complex/Utilities/FilterUtilities.hpp"

using namespace complex;

namespace
{
Result<> checkGeometryArraysCompatible(const Float32Array& vertices, const UInt64Array& cells, bool treatWarningsAsErrors, const std::string& cellType)
{
  Result<> warningResults;
  usize numVertices = vertices.getNumberOfTuples();
  uint64 idx = 0;
  for(usize i = 0; i < cells.getSize(); i++)
  {
    if(cells[i] > idx)
    {
      idx = cells[i];
    }
  }
  if((idx + 1) > numVertices)
  {
    std::string msg =
        fmt::format("Supplied {} list contains a vertex index larger than the total length of the supplied shared vertex list\nIndex Value: {}\nNumber of Vertices: {}", cellType, idx, numVertices);
    if(treatWarningsAsErrors)
    {
      return {nonstd::make_unexpected(std::vector<Error>{Error{-9844, msg}})};
    }
    warningResults.warnings().push_back(Warning{-9844, msg});
  }
  return warningResults;
}

Result<> checkGridBoundsResolution(const Float32Array& bounds, bool treatWarningsAsErrors, const std::string& boundType)
{
  Result<> warningResults;
  float val = bounds[0];
  for(size_t i = 1; i < bounds.getNumberOfTuples(); i++)
  {
    if(val > bounds[i])
    {
      std::string msg =
          fmt::format("Supplied {} Bounds array is not strictly increasing; this results in negative resolutions\nIndex {} Value: {}\nIndex {} Value: {}", boundType, (i - 1), val, i, bounds[i]);
      if(treatWarningsAsErrors)
      {
        return MakeErrorResult(-8344, msg);
      }
      warningResults.warnings().push_back(Warning{-8344, msg});
    }
    val = bounds[i];
  }
  return warningResults;
}
} // namespace

// -----------------------------------------------------------------------------
CreateGeometry::CreateGeometry(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, CreateGeometryInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
CreateGeometry::~CreateGeometry() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& CreateGeometry::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> CreateGeometry::operator()()
{
  Result<> warningResults;
  // copy over the vertex data in the case of vertex, edge, triangle, quad, tet, and hex geometries
  if(m_InputValues->GeometryType == 2 || m_InputValues->GeometryType == 3 || m_InputValues->GeometryType == 4 || m_InputValues->GeometryType == 5 || m_InputValues->GeometryType == 6 ||
     m_InputValues->GeometryType == 7)
  {
    const DataPath destVertexListPath = m_InputValues->GeometryPath.createChildPath(m_InputValues->SharedVertexListArrayPath.getTargetName());
    const auto& vertexList = m_DataStructure.getDataRefAs<Float32Array>(destVertexListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::float32, m_DataStructure, m_InputValues->SharedVertexListArrayPath, destVertexListPath))
    {
      const auto& srcVertexList = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->SharedVertexListArrayPath);
      return MakeErrorResult(-8340, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", m_InputValues->SharedVertexListArrayPath.toString(),
                                                srcVertexList.getSize(), destVertexListPath.toString(), vertexList.getSize()));
    }
  }
  // edge geometry
  if(m_InputValues->GeometryType == 3)
  {
    // copy over the edge data
    const DataPath destEdgeListPath = m_InputValues->GeometryPath.createChildPath(m_InputValues->SharedEdgeListArrayPath.getTargetName());
    const auto& edgesList = m_DataStructure.getDataRefAs<UInt64Array>(destEdgeListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::uint64, m_DataStructure, m_InputValues->SharedEdgeListArrayPath, destEdgeListPath))
    {
      const auto& srcEdgeList = m_DataStructure.getDataRefAs<UInt64Array>(m_InputValues->SharedEdgeListArrayPath);
      return MakeErrorResult(-8341, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", m_InputValues->SharedEdgeListArrayPath.toString(),
                                                srcEdgeList.getSize(), destEdgeListPath.toString(), edgesList.getSize()));
    }

    // This check must be done in execute since we are accessing the array values!
    const auto& vertexList = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->GeometryPath.createChildPath(m_InputValues->SharedVertexListArrayPath.getTargetName()));
    auto results = checkGeometryArraysCompatible(vertexList, edgesList, m_InputValues->TreatWarningsAsErrors, "edge");
    if(results.invalid())
    {
      return results;
    }
    warningResults.warnings().insert(warningResults.warnings().end(), results.warnings().begin(), results.warnings().end());
  }
  // triangle and quad geometries
  if(m_InputValues->GeometryType == 4 || m_InputValues->GeometryType == 5)
  {
    // copy over the face data
    const DataPath destFaceListPath = m_InputValues->GeometryPath.createChildPath(m_InputValues->SharedFaceListArrayPath.getTargetName());
    const auto& faceList = m_DataStructure.getDataRefAs<UInt64Array>(destFaceListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::uint64, m_DataStructure, m_InputValues->SharedFaceListArrayPath, destFaceListPath))
    {
      const auto& srcFaceList = m_DataStructure.getDataRefAs<UInt64Array>(m_InputValues->SharedFaceListArrayPath);
      return MakeErrorResult(-8342, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", m_InputValues->SharedFaceListArrayPath.toString(),
                                                srcFaceList.getSize(), destFaceListPath.toString(), faceList.getSize()));
    }

    // This check must be done in execute since we are accessing the array values!
    const auto& vertexList = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->GeometryPath.createChildPath(m_InputValues->SharedVertexListArrayPath.getTargetName()));
    auto results = checkGeometryArraysCompatible(vertexList, faceList, m_InputValues->TreatWarningsAsErrors, (m_InputValues->GeometryType == 4 ? "triangle" : "quadrilateral"));
    if(results.invalid())
    {
      return results;
    }
    warningResults.warnings().insert(warningResults.warnings().end(), results.warnings().begin(), results.warnings().end());
  }
  // tetrahedral and hexahedral geometries
  if(m_InputValues->GeometryType == 6 || m_InputValues->GeometryType == 7)
  {
    // copy over the cell data
    const DataPath destCellListPath = m_InputValues->GeometryPath.createChildPath(m_InputValues->SharedCellListArrayPath.getTargetName());
    const auto& cellList = m_DataStructure.getDataRefAs<UInt64Array>(destCellListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::uint64, m_DataStructure, m_InputValues->SharedCellListArrayPath, destCellListPath))
    {
      const auto& srcCellList = m_DataStructure.getDataRefAs<UInt64Array>(m_InputValues->SharedCellListArrayPath);
      return MakeErrorResult(-8343, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", m_InputValues->SharedCellListArrayPath.toString(),
                                                srcCellList.getSize(), destCellListPath.toString(), cellList.getSize()));
    }

    // This check must be done in execute since we are accessing the array values!
    const auto& vertexList = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->GeometryPath.createChildPath(m_InputValues->SharedVertexListArrayPath.getTargetName()));
    auto results = checkGeometryArraysCompatible(vertexList, cellList, m_InputValues->TreatWarningsAsErrors, (m_InputValues->GeometryType == 6 ? "tetrahedral" : "hexahedral"));
    if(results.invalid())
    {
      return results;
    }
    warningResults.warnings().insert(warningResults.warnings().end(), results.warnings().begin(), results.warnings().end());
  }
  // This check must be done in execute since we are accessing the array values!
  if(m_InputValues->GeometryType == 1) // RectilinearGridGeometry
  {
    const auto& xBounds = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->XBoundsArrayPath);
    const auto& yBounds = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->YBoundsArrayPath);
    const auto& zBounds = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->ZBoundsArrayPath);
    auto xResults = checkGridBoundsResolution(xBounds, m_InputValues->TreatWarningsAsErrors, "X");
    auto yResults = checkGridBoundsResolution(yBounds, m_InputValues->TreatWarningsAsErrors, "Y");
    auto zResults = checkGridBoundsResolution(zBounds, m_InputValues->TreatWarningsAsErrors, "Z");
    auto results = MergeResults(MergeResults(xResults, yResults), zResults);
    if(results.invalid())
    {
      return xResults;
    }
    warningResults.warnings().insert(warningResults.warnings().end(), results.warnings().begin(), results.warnings().end());
  }
  if(m_InputValues->GeometryType == 3 || m_InputValues->GeometryType == 4 || m_InputValues->GeometryType == 5 || m_InputValues->GeometryType == 6 || m_InputValues->GeometryType == 7)
  {
  }
  return warningResults;
}
