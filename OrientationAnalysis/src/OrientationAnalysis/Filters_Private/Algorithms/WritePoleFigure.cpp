#include "WritePoleFigure.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
WritePoleFigure::WritePoleFigure(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, WritePoleFigureInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
WritePoleFigure::~WritePoleFigure() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& WritePoleFigure::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> WritePoleFigure::operator()()
{

  return {};
}
