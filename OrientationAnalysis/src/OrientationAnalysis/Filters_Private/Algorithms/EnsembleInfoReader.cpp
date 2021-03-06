#include "EnsembleInfoReader.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
EnsembleInfoReader::EnsembleInfoReader(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, EnsembleInfoReaderInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
EnsembleInfoReader::~EnsembleInfoReader() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& EnsembleInfoReader::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> EnsembleInfoReader::operator()()
{

  return {};
}
