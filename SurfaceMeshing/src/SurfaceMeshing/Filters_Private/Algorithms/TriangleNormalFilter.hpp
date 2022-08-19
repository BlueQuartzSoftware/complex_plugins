#pragma once

#include "SurfaceMeshing/SurfaceMeshing_export.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{

struct SURFACEMESHING_EXPORT TriangleNormalFilterInputValues
{
  DataPath SurfaceMeshTriangleNormalsArrayPath;
};

/**
 * @class TriangleNormalFilter
 * @brief This filter calculates a normal vector of length 1 (normalized) for each Triangle in a Triangle Geometry.
 */

class SURFACEMESHING_EXPORT TriangleNormalFilter
{
public:
  TriangleNormalFilter(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, TriangleNormalFilterInputValues* inputValues);
  ~TriangleNormalFilter() noexcept;

  TriangleNormalFilter(const TriangleNormalFilter&) = delete;
  TriangleNormalFilter(TriangleNormalFilter&&) noexcept = delete;
  TriangleNormalFilter& operator=(const TriangleNormalFilter&) = delete;
  TriangleNormalFilter& operator=(TriangleNormalFilter&&) noexcept = delete;

  Result<> operator()();

  const std::atomic_bool& getCancel();

private:
  DataStructure& m_DataStructure;
  const TriangleNormalFilterInputValues* m_InputValues = nullptr;
  const std::atomic_bool& m_ShouldCancel;
  const IFilter::MessageHandler& m_MessageHandler;
};

} // namespace complex
