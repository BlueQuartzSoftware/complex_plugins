#include "QuickSurfaceMesh.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
QuickSurfaceMesh::QuickSurfaceMesh(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, QuickSurfaceMeshInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
QuickSurfaceMesh::~QuickSurfaceMesh() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& QuickSurfaceMesh::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> QuickSurfaceMesh::operator()()
{

  return {};
}
