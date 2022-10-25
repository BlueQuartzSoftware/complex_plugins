#pragma once

#include "Core/Core_export.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Filter/IFilter.hpp"

/**
* This is example code to put in the Execute Method of the filter.

*/

namespace complex
{

struct CORE_EXPORT FindTriangleGeomShapesInputValues
{
  DataPath FaceLabelsArrayPath;
  DataPath FeatureAttributeMatrixName;
  DataPath CentroidsArrayPath;
  DataPath VolumesArrayPath;
  DataPath Omega3sArrayName;
  DataPath AxisLengthsArrayName;
  DataPath AxisEulerAnglesArrayName;
  DataPath AspectRatiosArrayName;
};

/**
 * @class ConditionalSetValue
 * @brief This filter replaces values in the target array with a user specified value
 * where a bool mask array specifies.
 */

class CORE_EXPORT FindTriangleGeomShapes
{
public:
  FindTriangleGeomShapes(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, FindTriangleGeomShapesInputValues* inputValues);
  ~FindTriangleGeomShapes() noexcept;

  FindTriangleGeomShapes(const FindTriangleGeomShapes&) = delete;
  FindTriangleGeomShapes(FindTriangleGeomShapes&&) noexcept = delete;
  FindTriangleGeomShapes& operator=(const FindTriangleGeomShapes&) = delete;
  FindTriangleGeomShapes& operator=(FindTriangleGeomShapes&&) noexcept = delete;

  Result<> operator()();

  const std::atomic_bool& getCancel();

private:
  DataStructure& m_DataStructure;
  const FindTriangleGeomShapesInputValues* m_InputValues = nullptr;
  const std::atomic_bool& m_ShouldCancel;
  const IFilter::MessageHandler& m_MessageHandler;
};

} // namespace complex
