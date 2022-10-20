#include "CreateGeometryFilter.hpp"

#include "Core/Filters/Algorithms/CreateGeometry.hpp"
#include "complex/Common/TypesUtility.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/Geometry/EdgeGeom.hpp"
#include "complex/DataStructure/Geometry/HexahedralGeom.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/DataStructure/Geometry/QuadGeom.hpp"
#include "complex/DataStructure/Geometry/TetrahedralGeom.hpp"
#include "complex/DataStructure/Geometry/TriangleGeom.hpp"
#include "complex/DataStructure/Geometry/VertexGeom.hpp"
#include "complex/Filter/Actions/CreateGeometry1DAction.hpp"
#include "complex/Filter/Actions/CreateGeometry2DAction.hpp"
#include "complex/Filter/Actions/CreateGeometry3DAction.hpp"
#include "complex/Filter/Actions/CreateImageGeometryAction.hpp"
#include "complex/Filter/Actions/CreateVertexGeometryAction.hpp"
#include "complex/Filter/Actions/DeleteDataAction.hpp"
#include "complex/Filter/Actions/MoveDataAction.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/DataGroupCreationParameter.hpp"
#include "complex/Parameters/DataObjectNameParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

#include <sstream>
#include <string>

using namespace std::string_literals;

using namespace complex;

namespace complex
{
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
  params.insert(std::make_unique<ChoicesParameter>(k_ArrayHandling_Key, "Array Handling", "", 0, ChoicesParameter::Choices{"Copy Array", "Move Array "}));

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
    if(xBounds->getNumberOfTuples() < 2 || yBounds->getNumberOfTuples() < 2 || zBounds->getNumberOfTuples() < 2)
    {
      fmt::format("One of the bounds arrays has a size less than two; all sizes must be at least two\nX Bounds Size: {}\nY Bounds Size: {}\nZ Bounds Size: {}\n", xBounds->getNumberOfTuples(),
                  yBounds->getNumberOfTuples(), zBounds->getNumberOfTuples());
      return {nonstd::make_unexpected(
          std::vector<Error>{Error{-9847, fmt::format("One of the bounds arrays has a size less than two; all sizes must be at least two\nX Bounds Size: {}\nY Bounds Size: {}\nZ Bounds Size: {}\n",
                                                      xBounds->getNumberOfTuples(), yBounds->getNumberOfTuples(), zBounds->getNumberOfTuples())}})};
    }

    // TODO : create rect grid geom action
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
  CreateGeometryInputValues inputValues;
  inputValues.GeometryPath = filterArgs.value<DataPath>(k_GeometryName_Key);
  inputValues.GeometryType = filterArgs.value<ChoicesParameter::ValueType>(k_GeometryType_Key);
  inputValues.TreatWarningsAsErrors = filterArgs.value<bool>(k_WarningsAsErrors_Key);
  inputValues.MoveArrays = filterArgs.value<ChoicesParameter::ValueType>(k_ArrayHandling_Key) == 1;

  if(inputValues.GeometryType == 0) // ImageGeom
  {
    inputValues.Dimensions = filterArgs.value<VectorUInt64Parameter::ValueType>(k_Dimensions_Key);
    inputValues.Origin = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Origin_Key);
    inputValues.Spacing = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Spacing_Key);
    inputValues.CellAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_CellAttributeMatrixName_Key));
  }
  if(inputValues.GeometryType == 1) // RectGridGeom
  {
    inputValues.XBoundsArrayPath = filterArgs.value<DataPath>(k_XBounds_Key);
    inputValues.YBoundsArrayPath = filterArgs.value<DataPath>(k_YBounds_Key);
    inputValues.ZBoundsArrayPath = filterArgs.value<DataPath>(k_ZBounds_Key);
    inputValues.CellAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_CellAttributeMatrixName_Key));
  }
  if(inputValues.GeometryType == 2) // VertexGeom
  {
    inputValues.SharedVertexListArrayPath = filterArgs.value<DataPath>(k_VertexListName_Key);
    inputValues.VertexAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_VertexAttributeMatrixName_Key));
  }
  if(inputValues.GeometryType == 3) // EdgeGeom
  {
    inputValues.SharedVertexListArrayPath = filterArgs.value<DataPath>(k_VertexListName_Key);
    inputValues.VertexAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_VertexAttributeMatrixName_Key));
    inputValues.SharedEdgeListArrayPath = filterArgs.value<DataPath>(k_EdgeListName_Key);
    inputValues.EdgeAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_EdgeAttributeMatrixName_Key));
  }
  if(inputValues.GeometryType == 4) // TriangleGeom
  {
    inputValues.SharedVertexListArrayPath = filterArgs.value<DataPath>(k_VertexListName_Key);
    inputValues.VertexAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_VertexAttributeMatrixName_Key));
    inputValues.SharedFaceListArrayPath = filterArgs.value<DataPath>(k_TriangleListName_Key);
    inputValues.FaceAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_FaceAttributeMatrixName_Key));
  }
  if(inputValues.GeometryType == 5) // QuadGeom
  {
    inputValues.SharedVertexListArrayPath = filterArgs.value<DataPath>(k_VertexListName_Key);
    inputValues.VertexAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_VertexAttributeMatrixName_Key));
    inputValues.SharedFaceListArrayPath = filterArgs.value<DataPath>(k_QuadrilateralListName_Key);
    inputValues.FaceAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_FaceAttributeMatrixName_Key));
  }
  if(inputValues.GeometryType == 6) // TetrahedralGeom
  {
    inputValues.SharedVertexListArrayPath = filterArgs.value<DataPath>(k_VertexListName_Key);
    inputValues.VertexAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_VertexAttributeMatrixName_Key));
    inputValues.SharedCellListArrayPath = filterArgs.value<DataPath>(k_TetrahedralListName_Key);
    inputValues.CellAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_CellAttributeMatrixName_Key));
  }
  if(inputValues.GeometryType == 7) // HexahedralGeom
  {
    inputValues.SharedVertexListArrayPath = filterArgs.value<DataPath>(k_VertexListName_Key);
    inputValues.VertexAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_VertexAttributeMatrixName_Key));
    inputValues.SharedCellListArrayPath = filterArgs.value<DataPath>(k_HexahedralListName_Key);
    inputValues.CellAttributeMatrixPath = inputValues.GeometryPath.createChildPath(filterArgs.value<std::string>(k_CellAttributeMatrixName_Key));
  }

  return CreateGeometry(data, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex
