#include "VtkRectilinearGridWriter.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
VtkRectilinearGridWriter::VtkRectilinearGridWriter(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                                   VtkRectilinearGridWriterInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
VtkRectilinearGridWriter::~VtkRectilinearGridWriter() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& VtkRectilinearGridWriter::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> VtkRectilinearGridWriter::operator()()
{

  return {};
}
