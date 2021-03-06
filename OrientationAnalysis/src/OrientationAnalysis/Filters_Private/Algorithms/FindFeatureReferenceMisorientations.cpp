#include "FindFeatureReferenceMisorientations.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
FindFeatureReferenceMisorientations::FindFeatureReferenceMisorientations(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                                                         FindFeatureReferenceMisorientationsInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
FindFeatureReferenceMisorientations::~FindFeatureReferenceMisorientations() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& FindFeatureReferenceMisorientations::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> FindFeatureReferenceMisorientations::operator()()
{

  return {};
}
