#include "CreateGeometryFilter.hpp"

#include "complex/Common/TypesUtility.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/Geometry/EdgeGeom.hpp"
#include "complex/DataStructure/Geometry/HexahedralGeom.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/DataStructure/Geometry/QuadGeom.hpp"
#include "complex/DataStructure/Geometry/TetrahedralGeom.hpp"
#include "complex/DataStructure/Geometry/TriangleGeom.hpp"
#include "complex/DataStructure/Geometry/VertexGeom.hpp"
#include "complex/Filter/Actions/CreateImageGeometryAction.hpp"
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
  // Create the parameter descriptors that are needed for this filter
  params.insertLinkableParameter(std::make_unique<ChoicesParameter>(k_GeometryType_Key, "Geometry Type", "", 0, GetAllGeometryTypesAsStrings()));
  params.insert(std::make_unique<BoolParameter>(k_WarningsAsErrors_Key, "Treat Geometry Warnings as Errors", "", false));
  params.insert(std::make_unique<ChoicesParameter>(k_ArrayHandling_Key, "Array Handling", "", 0, ChoicesParameter::Choices{"Copy Array, Move Array"}));
  params.insert(std::make_unique<DataGroupCreationParameter>(k_GeometryName_Key, "Geometry Name", "", DataPath({"[Geometry]"})));

  // Image Geometry
  params.insert(std::make_unique<VectorUInt64Parameter>(k_Dimensions_Key, "Dimensions", "The number of cells in each of the X, Y, Z directions", std::vector<uint64_t>{20ULL, 60ULL, 200ULL},
                                                        std::vector<std::string>{"X"s, "Y"s, "Z"s}));
  params.insert(
      std::make_unique<VectorFloat32Parameter>(k_Origin_Key, "Origin", "The origin of each of the axes in X, Y, Z order", std::vector<float32>(3), std::vector<std::string>{"X"s, "Y"s, "Z"s}));
  params.insert(
      std::make_unique<VectorFloat32Parameter>(k_Spacing_Key, "Spacing", "The length scale of each voxel/pixel", std::vector<float32>{1.0F, 1.0F, 1.0F}, std::vector<std::string>{"X"s, "Y"s, "Z"s}));

  // RectilinearGrid Geometry
  params.insert(std::make_unique<ArraySelectionParameter>(k_XBounds_Key, "X Bounds", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::float32}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_YBounds_Key, "Y Bounds", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::float32}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_ZBounds_Key, "Z Bounds", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::float32}));

  // Vertex, Edge, Triangle, Quadrilateral, Tetrahedral, & Hexahedral Geometry
  params.insert(std::make_unique<ArraySelectionParameter>(k_VertexListName_Key, "Shared Vertex List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::float32}));
  params.insert(std::make_unique<DataObjectNameParameter>(k_VertexAttributeMatrixName_Key, "Vertex Attribute Matrix", "", INodeGeometry0D::k_VertexDataName));

  // Edge Geometry
  params.insert(std::make_unique<ArraySelectionParameter>(k_EdgeListName_Key, "Edge List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::uint64}));
  params.insert(std::make_unique<DataObjectNameParameter>(k_EdgeAttributeMatrixName_Key, "Edge Attribute Matrix", "", INodeGeometry1D::k_EdgeDataName));

  // Triangle Geometry
  params.insert(std::make_unique<ArraySelectionParameter>(k_TriangleListName_Key, "Triangle List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::uint64}));

  // Quadrilateral Geometry
  params.insert(std::make_unique<ArraySelectionParameter>(k_QuadrilateralListName_Key, "Quadrilateral List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::uint64}));

  // Triangle & Quadrilateral Geometry
  params.insert(std::make_unique<DataObjectNameParameter>(k_FaceAttributeMatrixName_Key, "Face Attribute Matrix", "", INodeGeometry2D::k_FaceDataName));

  // Tetrahedral Geometry
  params.insert(std::make_unique<ArraySelectionParameter>(k_TetrahedralListName_Key, "Tetrahedral List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::uint64}));

  // Hexahedral Geometry
  params.insert(std::make_unique<ArraySelectionParameter>(k_HexahedralListName_Key, "Hexahedral List", "", DataPath{}, ArraySelectionParameter::AllowedTypes{DataType::uint64}));

  // Image, RectilinearGrid, Tetrahedral, & Hexahedral Geometry
  params.insert(std::make_unique<DataObjectNameParameter>(k_CellAttributeMatrixName_Key, "Cell Attribute Matrix", "", IGridGeometry::k_CellDataName));

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
  /**
   * These are the values that were gathered from the UI or the pipeline file or
   * otherwise passed into the filter. These are here for your convenience. If you
   * do not need some of them remove them.
   */
  auto pImageGeometryPath = filterArgs.value<DataPath>(k_GeometryDataPath_Key);
  auto pDimensionsValue = filterArgs.value<VectorUInt64Parameter::ValueType>(k_Dimensions_Key);
  auto pOriginValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Origin_Key);
  auto pSpacingValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Spacing_Key);
  auto cellDataName = filterArgs.value<DataObjectNameParameter::ValueType>(k_CellDataName_Key);

  // These variables should be updated with the latest data generated for each variable during preflight.
  // These will be returned through the preflightResult variable to the
  // user interface. You could make these member variables instead if needed.
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

  // Define a custom class that generates the changes to the DataStructure.
  auto createImageGeometryAction = std::make_unique<CreateImageGeometryAction>(
      pImageGeometryPath, CreateImageGeometryAction::DimensionType({pDimensionsValue[0], pDimensionsValue[1], pDimensionsValue[2]}), pOriginValue, pSpacingValue, cellDataName);

  // Assign the CreateImageGeometryAction to the Result<OutputActions>::actions vector via a push_back
  complex::Result<OutputActions> resultOutputActions;
  resultOutputActions.value().actions.push_back(std::move(createImageGeometryAction));

  // Declare the preflightResult variable that will be populated with the results
  // of the preflight. The PreflightResult type contains the output Actions and
  // any preflight updated values that you want to be displayed to the user, typically
  // through a user interface (UI).
  IFilter::PreflightResult preflightResult;
  // Assign/Move the resultOutputActions to the preflightResult object

  std::vector<PreflightValue> preflightUpdatedValues = {{"BoxDimensions", boxDimensions}};

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> CreateGeometryFilter::executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                           const std::atomic_bool& shouldCancel) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pImageGeometryPath = filterArgs.value<DataPath>(k_GeometryDataPath_Key);
  auto pDimensionsValue = filterArgs.value<VectorUInt64Parameter::ValueType>(k_Dimensions_Key);
  auto pOriginValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Origin_Key);
  auto pSpacingValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Spacing_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/
  return {};
}
} // namespace complex
