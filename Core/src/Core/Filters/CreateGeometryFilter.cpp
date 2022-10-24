#include "CreateGeometryFilter.hpp"

#include "complex/Common/TypesUtility.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/Filter/Actions/CreateGeometry1DAction.hpp"
#include "complex/Filter/Actions/CreateGeometry2DAction.hpp"
#include "complex/Filter/Actions/CreateGeometry3DAction.hpp"
#include "complex/Filter/Actions/CreateImageGeometryAction.hpp"
#include "complex/Filter/Actions/CreateRectGridGeometryAction.hpp"
#include "complex/Filter/Actions/CreateVertexGeometryAction.hpp"
#include "complex/Filter/Actions/DeleteDataAction.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/DataObjectNameParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"
#include "complex/Utilities/FilterUtilities.hpp"

#include <sstream>
#include <string>

using namespace std::string_literals;

using namespace complex;

namespace complex
{

struct CopyDataArrayFunctor
{
  template <typename T>
  bool operator()(DataStructure& dataStruct, const DataPath& inDataRef, const DataPath& outDataRef)
  {
    const DataArray<T>& inputDataArray = dataStruct.getDataRefAs<DataArray<T>>(inDataRef);
    const auto& inputData = inputDataArray.getDataStoreRef();
    auto& outputDataArray = dataStruct.getDataRefAs<DataArray<T>>(outDataRef);
    auto& outputData = outputDataArray.getDataStoreRef();
    if(inputData.getSize() == outputData.getSize())
    {
      for(usize i = 0; i < inputData.getSize(); ++i)
      {
        outputData[i] = inputData[i];
      }
    }
    else
    {
      return false;
    }
    return true;
  }
};

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

//------------------------------------------------------------------------------
std::string CreateGeometryFilter::name() const
{
  return FilterTraits<CreateGeometryFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string CreateGeometryFilter::className() const
{
  return FilterTraits<CreateGeometryFilter>::className;
}

//------------------------------------------------------------------------------
Uuid CreateGeometryFilter::uuid() const
{
  return FilterTraits<CreateGeometryFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string CreateGeometryFilter::humanName() const
{
  return "Create Geometry";
}

//------------------------------------------------------------------------------
std::vector<std::string> CreateGeometryFilter::defaultTags() const
{
  return {"#Core", "#Generation",
          "#Geometry"
          "#Create Geometry"};
}

//------------------------------------------------------------------------------
Parameters CreateGeometryFilter::parameters() const
{
  Parameters params;

  params.insertSeparator(Parameters::Separator{"Input Parameters"});
  params.insertLinkableParameter(std::make_unique<ChoicesParameter>(k_GeometryType_Key, "Geometry Type", "", 0, GetAllGeometryTypesAsStrings()));
  params.insert(std::make_unique<BoolParameter>(k_WarningsAsErrors_Key, "Treat Geometry Warnings as Errors", "", false));
  params.insert(std::make_unique<ChoicesParameter>(k_ArrayHandling_Key, "Array Handling", "", 0, ChoicesParameter::Choices{"Copy Array", "Move Array"}));

  params.insert(std::make_unique<VectorUInt64Parameter>(k_Dimensions_Key, "Dimensions", "The number of cells in each of the X, Y, Z directions", std::vector<uint64_t>{20ULL, 60ULL, 200ULL},
                                                        std::vector<std::string>{"X"s, "Y"s, "Z"s}));
  params.insert(
      std::make_unique<VectorFloat32Parameter>(k_Origin_Key, "Origin", "The origin of each of the axes in X, Y, Z order", std::vector<float32>(3), std::vector<std::string>{"X"s, "Y"s, "Z"s}));
  params.insert(
      std::make_unique<VectorFloat32Parameter>(k_Spacing_Key, "Spacing", "The length scale of each voxel/pixel", std::vector<float32>{1.0F, 1.0F, 1.0F}, std::vector<std::string>{"X"s, "Y"s, "Z"s}));

  params.insert(std::make_unique<ArraySelectionParameter>(k_XBounds_Key, "X Bounds", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::float32}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_YBounds_Key, "Y Bounds", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::float32}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_ZBounds_Key, "Z Bounds", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::float32}));

  params.insert(std::make_unique<ArraySelectionParameter>(k_VertexListName_Key, "Shared Vertex List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::float32}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_EdgeListName_Key, "Edge List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::uint64}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_TriangleListName_Key, "Triangle List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::uint64}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_QuadrilateralListName_Key, "Quadrilateral List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::uint64}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_TetrahedralListName_Key, "Tetrahedral List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::uint64}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_HexahedralListName_Key, "Hexahedral List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::uint64}));

  params.insertSeparator(Parameters::Separator{"Created Data Objects"});
  params.insert(std::make_unique<DataObjectNameParameter>(k_VertexAttributeMatrixName_Key, "Vertex Attribute Matrix", "", INodeGeometry0D::k_VertexDataName));
  params.insert(std::make_unique<DataObjectNameParameter>(k_EdgeAttributeMatrixName_Key, "Edge Attribute Matrix", "", INodeGeometry1D::k_EdgeDataName));
  params.insert(std::make_unique<DataObjectNameParameter>(k_FaceAttributeMatrixName_Key, "Face Attribute Matrix", "", INodeGeometry2D::k_FaceDataName));
  params.insert(std::make_unique<DataObjectNameParameter>(k_CellAttributeMatrixName_Key, "Cell Attribute Matrix", "", IGridGeometry::k_CellDataName));
  params.insert(std::make_unique<DataGroupCreationParameter>(k_GeometryName_Key, "Geometry Name", "", DataPath({"[Geometry]"})));

  // setup linked parameters
  // image
  params.linkParameters(k_GeometryType_Key, k_Dimensions_Key, std::make_any<ChoicesParameter::ValueType>(0));
  params.linkParameters(k_GeometryType_Key, k_Origin_Key, std::make_any<ChoicesParameter::ValueType>(0));
  params.linkParameters(k_GeometryType_Key, k_Spacing_Key, std::make_any<ChoicesParameter::ValueType>(0));
  params.linkParameters(k_GeometryType_Key, k_CellAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(0));
  // rect grid
  params.linkParameters(k_GeometryType_Key, k_XBounds_Key, std::make_any<ChoicesParameter::ValueType>(1));
  params.linkParameters(k_GeometryType_Key, k_YBounds_Key, std::make_any<ChoicesParameter::ValueType>(1));
  params.linkParameters(k_GeometryType_Key, k_ZBounds_Key, std::make_any<ChoicesParameter::ValueType>(1));
  params.linkParameters(k_GeometryType_Key, k_CellAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(1));
  // vertex
  params.linkParameters(k_GeometryType_Key, k_VertexListName_Key, std::make_any<ChoicesParameter::ValueType>(2));
  params.linkParameters(k_GeometryType_Key, k_VertexAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(2));
  // edge
  params.linkParameters(k_GeometryType_Key, k_VertexListName_Key, std::make_any<ChoicesParameter::ValueType>(3));
  params.linkParameters(k_GeometryType_Key, k_EdgeListName_Key, std::make_any<ChoicesParameter::ValueType>(3));
  params.linkParameters(k_GeometryType_Key, k_VertexAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(3));
  params.linkParameters(k_GeometryType_Key, k_EdgeAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(3));
  // triangle
  params.linkParameters(k_GeometryType_Key, k_VertexListName_Key, std::make_any<ChoicesParameter::ValueType>(4));
  params.linkParameters(k_GeometryType_Key, k_TriangleListName_Key, std::make_any<ChoicesParameter::ValueType>(4));
  params.linkParameters(k_GeometryType_Key, k_VertexAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(4));
  params.linkParameters(k_GeometryType_Key, k_FaceAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(4));
  // quad
  params.linkParameters(k_GeometryType_Key, k_VertexListName_Key, std::make_any<ChoicesParameter::ValueType>(5));
  params.linkParameters(k_GeometryType_Key, k_QuadrilateralListName_Key, std::make_any<ChoicesParameter::ValueType>(5));
  params.linkParameters(k_GeometryType_Key, k_VertexAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(5));
  params.linkParameters(k_GeometryType_Key, k_FaceAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(5));
  // tet
  params.linkParameters(k_GeometryType_Key, k_VertexListName_Key, std::make_any<ChoicesParameter::ValueType>(6));
  params.linkParameters(k_GeometryType_Key, k_TetrahedralListName_Key, std::make_any<ChoicesParameter::ValueType>(6));
  params.linkParameters(k_GeometryType_Key, k_VertexAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(6));
  params.linkParameters(k_GeometryType_Key, k_CellAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(6));
  // hex
  params.linkParameters(k_GeometryType_Key, k_VertexListName_Key, std::make_any<ChoicesParameter::ValueType>(7));
  params.linkParameters(k_GeometryType_Key, k_HexahedralListName_Key, std::make_any<ChoicesParameter::ValueType>(7));
  params.linkParameters(k_GeometryType_Key, k_VertexAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(7));
  params.linkParameters(k_GeometryType_Key, k_CellAttributeMatrixName_Key, std::make_any<ChoicesParameter::ValueType>(7));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer CreateGeometryFilter::clone() const
{
  return std::make_unique<CreateGeometryFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult CreateGeometryFilter::preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const
{
  auto pGeometryPath = filterArgs.value<DataPath>(k_GeometryName_Key);
  auto pGeometryType = filterArgs.value<ChoicesParameter::ValueType>(k_GeometryType_Key);
  auto pWarningsAsErrors = filterArgs.value<bool>(k_WarningsAsErrors_Key);
  auto pMoveArrays = filterArgs.value<ChoicesParameter::ValueType>(k_ArrayHandling_Key) == 1;

  complex::Result<OutputActions> resultOutputActions;
  std::vector<PreflightValue> preflightUpdatedValues;

  // collect arguments common to multiple geometry types all at once
  DataPath pVertexListPath;
  std::string pVertexAMName;
  std::string pFaceAMName;
  std::string pCellAMName;
  if(pGeometryType == 2 || pGeometryType == 3 || pGeometryType == 4 || pGeometryType == 5 || pGeometryType == 6 || pGeometryType == 7)
  {
    pVertexListPath = filterArgs.value<DataPath>(k_VertexListName_Key);
    pVertexAMName = filterArgs.value<std::string>(k_VertexAttributeMatrixName_Key);

    if(ds.getDataAs<Float32Array>(pVertexListPath) == nullptr)
    {
      return {nonstd::make_unexpected(std::vector<Error>{Error{-9840, fmt::format("Cannot find selected vertex list at path '{}'", pVertexListPath.toString())}})};
    }
  }
  if(pGeometryType == 4 || pGeometryType == 5)
  {
    pFaceAMName = filterArgs.value<std::string>(k_FaceAttributeMatrixName_Key);
  }
  if(pGeometryType == 0 || pGeometryType == 1 || pGeometryType == 6 || pGeometryType == 7)
  {
    pCellAMName = filterArgs.value<std::string>(k_CellAttributeMatrixName_Key);
  }

  // create geometry actions & deferred delete data actions if move arrays option selected
  if(pGeometryType == 0) // ImageGeom
  {
    auto pDimensionsValue = filterArgs.value<VectorUInt64Parameter::ValueType>(k_Dimensions_Key);
    auto pOriginValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Origin_Key);
    auto pSpacingValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Spacing_Key);

    std::stringstream ss;
    std::array<float, 3> halfRes = {pSpacingValue[0] * 0.5F, pSpacingValue[1] * 0.5F, pSpacingValue[2] * 0.5F};
    ss << "Extents:\n"
       << "X Extent: 0 to " << pDimensionsValue[0] - 1 << " (dimension: " << pDimensionsValue[0] << ")\n"
       << "Y Extent: 0 to " << pDimensionsValue[1] - 1 << " (dimension: " << pDimensionsValue[1] << ")\n"
       << "Z Extent: 0 to " << pDimensionsValue[2] - 1 << " (dimension: " << pDimensionsValue[2] << ")\n"
       << "Bounds:\n"
       << "X Range: " << (pOriginValue[0] - halfRes[0]) << " to " << (pOriginValue[0] - halfRes[0] + static_cast<float>(pDimensionsValue[0]) * pSpacingValue[0])
       << " (delta: " << (static_cast<float>(pDimensionsValue[0]) * pSpacingValue[0]) << ")\n"
       << "Y Range: " << (pOriginValue[1] - halfRes[1]) << " to " << (pOriginValue[1] - halfRes[1] + static_cast<float>(pDimensionsValue[1]) * pSpacingValue[1])
       << " (delta: " << (static_cast<float>(pDimensionsValue[1]) * pSpacingValue[1]) << ")\n"
       << "Z Range: " << (pOriginValue[2] - halfRes[2]) << " to " << (pOriginValue[2] - halfRes[2] + static_cast<float>(pDimensionsValue[2]) * pSpacingValue[2])
       << " (delta: " << (static_cast<float>(pDimensionsValue[2]) * pSpacingValue[2]) << ")\n";
    std::string boxDimensions = ss.str();

    auto createImageGeometryAction = std::make_unique<CreateImageGeometryAction>(
        pGeometryPath, CreateImageGeometryAction::DimensionType({pDimensionsValue[0], pDimensionsValue[1], pDimensionsValue[2]}), pOriginValue, pSpacingValue, pCellAMName);

    resultOutputActions.value().actions.push_back(std::move(createImageGeometryAction));
    preflightUpdatedValues.push_back({"BoxDimensions", boxDimensions});
  }
  if(pGeometryType == 1) // RectGridGeom
  {
    auto pXBoundsPath = filterArgs.value<DataPath>(k_XBounds_Key);
    auto pYBoundsPath = filterArgs.value<DataPath>(k_YBounds_Key);
    auto pZBoundsPath = filterArgs.value<DataPath>(k_ZBounds_Key);
    const auto xBounds = ds.getDataAs<Float32Array>(pXBoundsPath);
    if(xBounds == nullptr)
    {
      return {nonstd::make_unexpected(std::vector<Error>{Error{-9844, fmt::format("Cannot find selected quadrilateral list at path '{}'", pXBoundsPath.toString())}})};
    }
    const auto yBounds = ds.getDataAs<Float32Array>(pYBoundsPath);
    if(yBounds == nullptr)
    {
      return {nonstd::make_unexpected(std::vector<Error>{Error{-9845, fmt::format("Cannot find selected quadrilateral list at path '{}'", pYBoundsPath.toString())}})};
    }
    const auto zBounds = ds.getDataAs<Float32Array>(pZBoundsPath);
    if(zBounds == nullptr)
    {
      return {nonstd::make_unexpected(std::vector<Error>{Error{-9846, fmt::format("Cannot find selected quadrilateral list at path '{}'", pZBoundsPath.toString())}})};
    }
    usize xTuples = xBounds->getNumberOfTuples();
    usize yTuples = yBounds->getNumberOfTuples();
    usize zTuples = zBounds->getNumberOfTuples();
    if(xTuples < 2 || yTuples < 2 || zTuples < 2)
    {
      fmt::format("One of the bounds arrays has a size less than two; all sizes must be at least two\nX Bounds Size: {}\nY Bounds Size: {}\nZ Bounds Size: {}\n", xTuples, yTuples, zTuples);
      return {nonstd::make_unexpected(
          std::vector<Error>{Error{-9847, fmt::format("One of the bounds arrays has a size less than two; all sizes must be at least two\nX Bounds Size: {}\nY Bounds Size: {}\nZ Bounds Size: {}\n",
                                                      xBounds->getNumberOfTuples(), yBounds->getNumberOfTuples(), zBounds->getNumberOfTuples())}})};
    }

    auto createRectGridGeometryAction =
        std::make_unique<CreateRectGridGeometryAction>(pGeometryPath, xTuples, yTuples, zTuples, pCellAMName, xBounds->getName(), yBounds->getName(), zBounds->getName());
    resultOutputActions.value().actions.push_back(std::move(createRectGridGeometryAction));

    if(pMoveArrays) // copy over the data and delete later instead of actually moving so the geometry action can create & set the bounds lists
    {
      auto deleteXBoundsListAction = std::make_unique<DeleteDataAction>(pXBoundsPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteXBoundsListAction));
      auto deleteYBoundsListAction = std::make_unique<DeleteDataAction>(pYBoundsPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteYBoundsListAction));
      auto deleteZBoundsListAction = std::make_unique<DeleteDataAction>(pZBoundsPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteZBoundsListAction));
    }
  }
  if(pGeometryType == 2) // VertexGeom
  {
    const auto vertexList = ds.getDataAs<Float32Array>(pVertexListPath);

    auto createVertexGeomAction = std::make_unique<CreateVertexGeometryAction>(pGeometryPath, vertexList->getNumberOfTuples(), pVertexAMName, pVertexListPath.getTargetName());
    resultOutputActions.value().actions.push_back(std::move(createVertexGeomAction));

    if(pMoveArrays) // copy over the data and delete later instead of actually moving so the geometry action can create & set the vertices list
    {
      auto deleteSrcVertexListAction = std::make_unique<DeleteDataAction>(pVertexListPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteSrcVertexListAction));
    }
  }
  if(pGeometryType == 3) // EdgeGeom
  {
    auto pEdgeListPath = filterArgs.value<DataPath>(k_EdgeListName_Key);
    auto pEdgeAMName = filterArgs.value<std::string>(k_EdgeAttributeMatrixName_Key);
    const auto vertexList = ds.getDataAs<Float32Array>(pVertexListPath);
    const auto edgeList = ds.getDataAs<UInt64Array>(pEdgeListPath);
    if(edgeList == nullptr)
    {
      return {nonstd::make_unexpected(std::vector<Error>{Error{-9841, fmt::format("Cannot find selected edge list at path '{}'", pEdgeListPath.toString())}})};
    }

    auto createEdgeGeomAction = std::make_unique<CreateEdgeGeometryAction>(pGeometryPath, edgeList->getNumberOfTuples(), vertexList->getNumberOfTuples(), pVertexAMName, pEdgeAMName,
                                                                           pVertexListPath.getTargetName(), pEdgeListPath.getTargetName());
    resultOutputActions.value().actions.push_back(std::move(createEdgeGeomAction));

    if(pMoveArrays) // copy over the data and delete later instead of actually moving so the geometry action can create & set the vertices list(s)
    {
      auto deleteSrcVertexListAction = std::make_unique<DeleteDataAction>(pVertexListPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteSrcVertexListAction));
      auto deleteSrcEdgesListAction = std::make_unique<DeleteDataAction>(pEdgeListPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteSrcEdgesListAction));
    }
  }
  if(pGeometryType == 4) // TriangleGeom
  {
    auto pTriangleListPath = filterArgs.value<DataPath>(k_TriangleListName_Key);
    const auto vertexList = ds.getDataAs<Float32Array>(pVertexListPath);
    const auto triangleList = ds.getDataAs<UInt64Array>(pTriangleListPath);
    if(triangleList == nullptr)
    {
      return {nonstd::make_unexpected(std::vector<Error>{Error{-9843, fmt::format("Cannot find selected triangle list at path '{}'", pTriangleListPath.toString())}})};
    }

    auto createTriangleGeomAction = std::make_unique<CreateTriangleGeometryAction>(pGeometryPath, triangleList->getNumberOfTuples(), vertexList->getNumberOfTuples(), pVertexAMName, pFaceAMName,
                                                                                   pVertexListPath.getTargetName(), pTriangleListPath.getTargetName());
    resultOutputActions.value().actions.push_back(std::move(createTriangleGeomAction));

    if(pMoveArrays) // copy over the data and delete later instead of actually moving so the geometry action can create & set the list(s)
    {
      auto deleteSrcVertexListAction = std::make_unique<DeleteDataAction>(pVertexListPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteSrcVertexListAction));
      auto deleteSrcTrianglesListAction = std::make_unique<DeleteDataAction>(pTriangleListPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteSrcTrianglesListAction));
    }
  }
  if(pGeometryType == 5) // QuadGeom
  {
    auto pQuadListPath = filterArgs.value<DataPath>(k_QuadrilateralListName_Key);
    const auto vertexList = ds.getDataAs<Float32Array>(pVertexListPath);
    const auto quadList = ds.getDataAs<UInt64Array>(pQuadListPath);
    if(quadList == nullptr)
    {
      return {nonstd::make_unexpected(std::vector<Error>{Error{-9845, fmt::format("Cannot find selected quadrilateral list at path '{}'", pQuadListPath.toString())}})};
    }

    auto createQuadGeomAction = std::make_unique<CreateQuadGeometryAction>(pGeometryPath, quadList->getNumberOfTuples(), vertexList->getNumberOfTuples(), pVertexAMName, pFaceAMName,
                                                                           pVertexListPath.getTargetName(), pQuadListPath.getTargetName());
    resultOutputActions.value().actions.push_back(std::move(createQuadGeomAction));

    if(pMoveArrays) // copy over the data and delete later instead of actually moving so the geometry action can create & set the list(s)
    {
      auto deleteSrcVertexListAction = std::make_unique<DeleteDataAction>(pVertexListPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteSrcVertexListAction));
      auto deleteSrcQuadsListAction = std::make_unique<DeleteDataAction>(pQuadListPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteSrcQuadsListAction));
    }
  }
  if(pGeometryType == 6) // TetrahedralGeom
  {
    auto pTetListPath = filterArgs.value<DataPath>(k_TetrahedralListName_Key);
    const auto vertexList = ds.getDataAs<Float32Array>(pVertexListPath);
    const auto tetList = ds.getDataAs<UInt64Array>(pTetListPath);
    if(tetList == nullptr)
    {
      return {nonstd::make_unexpected(std::vector<Error>{Error{-9847, fmt::format("Cannot find selected quadrilateral list at path '{}'", pTetListPath.toString())}})};
    }

    auto createTetGeomAction = std::make_unique<CreateTetrahedralGeometryAction>(pGeometryPath, tetList->getNumberOfTuples(), vertexList->getNumberOfTuples(), pVertexAMName, pCellAMName,
                                                                                 pVertexListPath.getTargetName(), pTetListPath.getTargetName());
    resultOutputActions.value().actions.push_back(std::move(createTetGeomAction));

    if(pMoveArrays) // copy over the data and delete later instead of actually moving so the geometry action can create & set the list(s)
    {
      auto deleteSrcVertexListAction = std::make_unique<DeleteDataAction>(pVertexListPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteSrcVertexListAction));
      auto deleteSrcTetListAction = std::make_unique<DeleteDataAction>(pTetListPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteSrcTetListAction));
    }
  }
  if(pGeometryType == 7) // HexahedralGeom
  {
    auto pHexListPath = filterArgs.value<DataPath>(k_HexahedralListName_Key);
    const auto vertexList = ds.getDataAs<Float32Array>(pVertexListPath);
    const auto hexList = ds.getDataAs<UInt64Array>(pHexListPath);
    if(hexList == nullptr)
    {
      return {nonstd::make_unexpected(std::vector<Error>{Error{-9845, fmt::format("Cannot find selected quadrilateral list at path '{}'", pHexListPath.toString())}})};
    }

    auto createHexGeomAction = std::make_unique<CreateHexahedralGeometryAction>(pGeometryPath, hexList->getNumberOfTuples(), vertexList->getNumberOfTuples(), pVertexAMName, pCellAMName,
                                                                                pVertexListPath.getTargetName(), pHexListPath.getTargetName());
    resultOutputActions.value().actions.push_back(std::move(createHexGeomAction));

    if(pMoveArrays) // copy over the data and delete later instead of actually moving so the geometry action can create & set the list(s)
    {
      auto deleteSrcVertexListAction = std::make_unique<DeleteDataAction>(pVertexListPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteSrcVertexListAction));
      auto deleteSrcHexListAction = std::make_unique<DeleteDataAction>(pHexListPath);
      resultOutputActions.value().deferredActions.push_back(std::move(deleteSrcHexListAction));
    }
  }

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> CreateGeometryFilter::executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                           const std::atomic_bool& shouldCancel) const
{
  auto geometryPath = filterArgs.value<DataPath>(k_GeometryName_Key);
  auto geometryType = filterArgs.value<ChoicesParameter::ValueType>(k_GeometryType_Key);
  auto treatWarningsAsErrors = filterArgs.value<bool>(k_WarningsAsErrors_Key);
  auto moveArrays = filterArgs.value<ChoicesParameter::ValueType>(k_ArrayHandling_Key) == 1;

  DataPath sharedVertexListArrayPath;
  DataPath sharedFaceListArrayPath;
  DataPath sharedCellListArrayPath;

  if(geometryType == 2 || geometryType == 3 || geometryType == 4 || geometryType == 5 || geometryType == 6 || geometryType == 7)
  {
    sharedVertexListArrayPath = filterArgs.value<DataPath>(k_VertexListName_Key);
  }
  if(geometryType == 4)
  {
    sharedFaceListArrayPath = filterArgs.value<DataPath>(k_TriangleListName_Key);
  }
  if(geometryType == 5)
  {
    sharedFaceListArrayPath = filterArgs.value<DataPath>(k_QuadrilateralListName_Key);
  }
  if(geometryType == 6)
  {
    sharedCellListArrayPath = filterArgs.value<DataPath>(k_TetrahedralListName_Key);
  }
  if(geometryType == 7)
  {
    sharedCellListArrayPath = filterArgs.value<DataPath>(k_HexahedralListName_Key);
  }

  Result<> warningResults;
  // copy over the vertex data in the case of vertex, edge, triangle, quad, tet, and hex geometries
  if(geometryType == 2 || geometryType == 3 || geometryType == 4 || geometryType == 5 || geometryType == 6 || geometryType == 7)
  {
    const DataPath destVertexListPath = geometryPath.createChildPath(sharedVertexListArrayPath.getTargetName());
    const auto& vertexList = data.getDataRefAs<Float32Array>(destVertexListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::float32, data, sharedVertexListArrayPath, destVertexListPath))
    {
      const auto& srcVertexList = data.getDataRefAs<Float32Array>(sharedVertexListArrayPath);
      return MakeErrorResult(-8340, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", sharedVertexListArrayPath.toString(),
                                                srcVertexList.getSize(), destVertexListPath.toString(), vertexList.getSize()));
    }
  }
  // edge geometry
  if(geometryType == 3)
  {
    auto sharedEdgeListArrayPath = filterArgs.value<DataPath>(k_EdgeListName_Key);

    // copy over the edge data
    const DataPath destEdgeListPath = geometryPath.createChildPath(sharedEdgeListArrayPath.getTargetName());
    const auto& edgesList = data.getDataRefAs<UInt64Array>(destEdgeListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::uint64, data, sharedEdgeListArrayPath, destEdgeListPath))
    {
      const auto& srcEdgeList = data.getDataRefAs<UInt64Array>(sharedEdgeListArrayPath);
      return MakeErrorResult(-8341, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", sharedEdgeListArrayPath.toString(),
                                                srcEdgeList.getSize(), destEdgeListPath.toString(), edgesList.getSize()));
    }

    // This check must be done in execute since we are accessing the array values!
    const auto& vertexList = data.getDataRefAs<Float32Array>(geometryPath.createChildPath(sharedVertexListArrayPath.getTargetName()));
    auto results = checkGeometryArraysCompatible(vertexList, edgesList, treatWarningsAsErrors, "edge");
    if(results.invalid())
    {
      return results;
    }
    warningResults.warnings().insert(warningResults.warnings().end(), results.warnings().begin(), results.warnings().end());
  }
  // triangle and quad geometries
  if(geometryType == 4 || geometryType == 5)
  {
    // copy over the face data
    const DataPath destFaceListPath = geometryPath.createChildPath(sharedFaceListArrayPath.getTargetName());
    const auto& faceList = data.getDataRefAs<UInt64Array>(destFaceListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::uint64, data, sharedFaceListArrayPath, destFaceListPath))
    {
      const auto& srcFaceList = data.getDataRefAs<UInt64Array>(sharedFaceListArrayPath);
      return MakeErrorResult(-8342, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", sharedFaceListArrayPath.toString(),
                                                srcFaceList.getSize(), destFaceListPath.toString(), faceList.getSize()));
    }

    // This check must be done in execute since we are accessing the array values!
    const auto& vertexList = data.getDataRefAs<Float32Array>(geometryPath.createChildPath(sharedVertexListArrayPath.getTargetName()));
    auto results = checkGeometryArraysCompatible(vertexList, faceList, treatWarningsAsErrors, (geometryType == 4 ? "triangle" : "quadrilateral"));
    if(results.invalid())
    {
      return results;
    }
    warningResults.warnings().insert(warningResults.warnings().end(), results.warnings().begin(), results.warnings().end());
  }
  // tetrahedral and hexahedral geometries
  if(geometryType == 6 || geometryType == 7)
  {
    // copy over the cell data
    const DataPath destCellListPath = geometryPath.createChildPath(sharedCellListArrayPath.getTargetName());
    const auto& cellList = data.getDataRefAs<UInt64Array>(destCellListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::uint64, data, sharedCellListArrayPath, destCellListPath))
    {
      const auto& srcCellList = data.getDataRefAs<UInt64Array>(sharedCellListArrayPath);
      return MakeErrorResult(-8343, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", sharedCellListArrayPath.toString(),
                                                srcCellList.getSize(), destCellListPath.toString(), cellList.getSize()));
    }

    // This check must be done in execute since we are accessing the array values!
    const auto& vertexList = data.getDataRefAs<Float32Array>(geometryPath.createChildPath(sharedVertexListArrayPath.getTargetName()));
    auto results = checkGeometryArraysCompatible(vertexList, cellList, treatWarningsAsErrors, (geometryType == 6 ? "tetrahedral" : "hexahedral"));
    if(results.invalid())
    {
      return results;
    }
    warningResults.warnings().insert(warningResults.warnings().end(), results.warnings().begin(), results.warnings().end());
  }
  // rectilinear grid geometry
  if(geometryType == 1)
  {
    auto xBoundsArrayPath = filterArgs.value<DataPath>(k_XBounds_Key);
    auto yBoundsArrayPath = filterArgs.value<DataPath>(k_YBounds_Key);
    auto zBoundsArrayPath = filterArgs.value<DataPath>(k_ZBounds_Key);
    auto cellAttributeMatrixPath = geometryPath.createChildPath(filterArgs.value<std::string>(k_CellAttributeMatrixName_Key));

    // This check must be done in execute since we are accessing the array values!
    const auto& srcXBounds = data.getDataRefAs<Float32Array>(xBoundsArrayPath);
    const auto& srcYBounds = data.getDataRefAs<Float32Array>(yBoundsArrayPath);
    const auto& srcZBounds = data.getDataRefAs<Float32Array>(zBoundsArrayPath);
    auto xResults = checkGridBoundsResolution(srcXBounds, treatWarningsAsErrors, "X");
    auto yResults = checkGridBoundsResolution(srcYBounds, treatWarningsAsErrors, "Y");
    auto zResults = checkGridBoundsResolution(srcZBounds, treatWarningsAsErrors, "Z");
    auto results = MergeResults(MergeResults(xResults, yResults), zResults);
    if(results.invalid())
    {
      return results;
    }
    warningResults.warnings().insert(warningResults.warnings().end(), results.warnings().begin(), results.warnings().end());

    // copy over the bounds data
    const DataPath destXBoundsListPath = geometryPath.createChildPath(xBoundsArrayPath.getTargetName());
    const auto& xBounds = data.getDataRefAs<Float32Array>(destXBoundsListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::float32, data, xBoundsArrayPath, destXBoundsListPath))
    {
      return MakeErrorResult(-8340, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", xBoundsArrayPath.toString(), srcXBounds.getSize(),
                                                destXBoundsListPath.toString(), xBounds.getSize()));
    }

    const DataPath destYBoundsListPath = geometryPath.createChildPath(yBoundsArrayPath.getTargetName());
    const auto& yBounds = data.getDataRefAs<Float32Array>(destYBoundsListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::float32, data, yBoundsArrayPath, destYBoundsListPath))
    {
      return MakeErrorResult(-8340, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", yBoundsArrayPath.toString(), srcYBounds.getSize(),
                                                destYBoundsListPath.toString(), yBounds.getSize()));
    }

    const DataPath destZBoundsListPath = geometryPath.createChildPath(zBoundsArrayPath.getTargetName());
    const auto& zBounds = data.getDataRefAs<Float32Array>(destZBoundsListPath);
    if(!ExecuteDataFunction(CopyDataArrayFunctor{}, DataType::float32, data, zBoundsArrayPath, destZBoundsListPath))
    {
      return MakeErrorResult(-8340, fmt::format("Could not copy data array at path '{}' with size {} to data array at path '{}' with size {}.", zBoundsArrayPath.toString(), srcYBounds.getSize(),
                                                destZBoundsListPath.toString(), zBounds.getSize()));
    }
  }
  return warningResults;
}
} // namespace complex
