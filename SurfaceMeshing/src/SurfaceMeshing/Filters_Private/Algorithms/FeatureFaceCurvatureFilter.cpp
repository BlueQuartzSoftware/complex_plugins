#include "FeatureFaceCurvatureFilter.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
FeatureFaceCurvatureFilter::FeatureFaceCurvatureFilter(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                                       FeatureFaceCurvatureFilterInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
FeatureFaceCurvatureFilter::~FeatureFaceCurvatureFilter() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& FeatureFaceCurvatureFilter::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> FeatureFaceCurvatureFilter::operator()()
{

  return {};
}
