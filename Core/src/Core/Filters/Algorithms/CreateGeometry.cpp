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
  // copy over the edge data in the case of edge geometries
  if(m_InputValues->GeometryType == 3)
  {
    const DataPath destEdgeListPath = m_InputValues->GeometryPath.createChildPath(m_InputValues->SharedEdgeListArrayPath.getTargetName());
    const auto& edgesList = m_DataStructure.getDataRefAs<UInt64Array>(destEdgeListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::uint64, m_DataStructure, m_InputValues->SharedEdgeListArrayPath, destEdgeListPath))
    {
      const auto& srcEdgeList = m_DataStructure.getDataRefAs<UInt64Array>(m_InputValues->SharedEdgeListArrayPath);
      return MakeErrorResult(-8341, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", m_InputValues->SharedEdgeListArrayPath.toString(),
                                                srcEdgeList.getSize(), destEdgeListPath.toString(), edgesList.getSize()));
    }
  }
  // copy over the face data in the case of triangle and quad geometries
  if(m_InputValues->GeometryType == 4 || m_InputValues->GeometryType == 5)
  {
    const DataPath destFaceListPath = m_InputValues->GeometryPath.createChildPath(m_InputValues->SharedFaceListArrayPath.getTargetName());
    const auto& faceList = m_DataStructure.getDataRefAs<UInt64Array>(destFaceListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::uint64, m_DataStructure, m_InputValues->SharedFaceListArrayPath, destFaceListPath))
    {
      const auto& srcFaceList = m_DataStructure.getDataRefAs<UInt64Array>(m_InputValues->SharedFaceListArrayPath);
      return MakeErrorResult(-8342, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", m_InputValues->SharedFaceListArrayPath.toString(),
                                                srcFaceList.getSize(), destFaceListPath.toString(), faceList.getSize()));
    }
  }
  // copy over the cell data in the case of tetrahedral and hexahedral geometries
  if(m_InputValues->GeometryType == 6 || m_InputValues->GeometryType == 7)
  {
    const DataPath destCellListPath = m_InputValues->GeometryPath.createChildPath(m_InputValues->SharedCellListArrayPath.getTargetName());
    const auto& cellList = m_DataStructure.getDataRefAs<UInt64Array>(destCellListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::uint64, m_DataStructure, m_InputValues->SharedCellListArrayPath, destCellListPath))
    {
      const auto& srcCellList = m_DataStructure.getDataRefAs<UInt64Array>(m_InputValues->SharedCellListArrayPath);
      return MakeErrorResult(-8343, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", m_InputValues->SharedCellListArrayPath.toString(),
                                                srcCellList.getSize(), destCellListPath.toString(), cellList.getSize()));
    }
  }
  return warningResults;
}
