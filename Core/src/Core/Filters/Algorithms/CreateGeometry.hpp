#pragma once

#include "Core/Core_export.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Filter/IFilter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

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

struct CORE_EXPORT CreateGeometryInputValues
{
  DataPath GeometryPath;
  ChoicesParameter::ValueType GeometryType;
  bool TreatWarningsAsErrors;
  bool MoveArrays; // true = move arrays, false = copy arrays

  VectorUInt64Parameter::ValueType Dimensions;
  VectorFloat32Parameter::ValueType Origin;
  VectorFloat32Parameter::ValueType Spacing;

  DataPath XBoundsArrayPath;
  DataPath YBoundsArrayPath;
  DataPath ZBoundsArrayPath;

  DataPath SharedVertexListArrayPath;
  DataPath VertexAttributeMatrixPath;

  DataPath SharedEdgeListArrayPath;
  DataPath EdgeAttributeMatrixPath;

  DataPath SharedFaceListArrayPath;
  DataPath FaceAttributeMatrixPath;

  DataPath SharedCellListArrayPath;
  DataPath CellAttributeMatrixPath;
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
