#pragma once

#include "Core/Core_export.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{

struct CORE_EXPORT CreateGeometryInputValues
{
  ChoicesParameter::ValueType GeometryType;
  bool TreatWarningsAsErrors;
  ChoicesParameter::ValueType ArrayHandling;
  DataPath DataContainerName;
  VectorInt32Parameter::ValueType Dimensions;
  VectorFloat32Parameter::ValueType Origin;
  VectorFloat32Parameter::ValueType Spacing;
  DataPath ImageCellAttributeMatrixName;
  DataPath XBoundsArrayPath;
  DataPath YBoundsArrayPath;
  DataPath ZBoundsArrayPath;
  DataPath RectGridCellAttributeMatrixName;
  <<<NOT_IMPLEMENTED>>> BoxDimensions;
  DataPath SharedVertexListArrayPath0;
  DataPath VertexAttributeMatrixName0;
  DataPath SharedVertexListArrayPath1;
  DataPath SharedEdgeListArrayPath;
  DataPath VertexAttributeMatrixName1;
  DataPath EdgeAttributeMatrixName;
  DataPath SharedVertexListArrayPath2;
  DataPath SharedTriListArrayPath;
  DataPath VertexAttributeMatrixName2;
  DataPath FaceAttributeMatrixName0;
  DataPath SharedVertexListArrayPath3;
  DataPath SharedQuadListArrayPath;
  DataPath VertexAttributeMatrixName3;
  DataPath FaceAttributeMatrixName1;
  DataPath SharedVertexListArrayPath4;
  DataPath SharedTetListArrayPath;
  DataPath VertexAttributeMatrixName4;
  DataPath TetCellAttributeMatrixName;
  DataPath SharedVertexListArrayPath5;
  DataPath SharedHexListArrayPath;
  DataPath VertexAttributeMatrixName5;
  DataPath HexCellAttributeMatrixName;
};

/**
 * @class ConditionalSetValue
 * @brief This filter replaces values in the target array with a user specified value
 * where a bool mask array specifies.
 */

class CORE_EXPORT CreateGeometry
{
public:
  CreateGeometry(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, CreateGeometryInputValues* inputValues);
  ~CreateGeometry() noexcept;

  CreateGeometry(const CreateGeometry&) = delete;
  CreateGeometry(CreateGeometry&&) noexcept = delete;
  CreateGeometry& operator=(const CreateGeometry&) = delete;
  CreateGeometry& operator=(CreateGeometry&&) noexcept = delete;

  Result<> operator()();

  const std::atomic_bool& getCancel();

private:
  DataStructure& m_DataStructure;
  const CreateGeometryInputValues* m_InputValues = nullptr;
  const std::atomic_bool& m_ShouldCancel;
  const IFilter::MessageHandler& m_MessageHandler;
};

} // namespace complex
