#include "ItkDiscreteGaussianBlur.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
ItkDiscreteGaussianBlur::ItkDiscreteGaussianBlur(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                                 ItkDiscreteGaussianBlurInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
ItkDiscreteGaussianBlur::~ItkDiscreteGaussianBlur() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& ItkDiscreteGaussianBlur::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> ItkDiscreteGaussianBlur::operator()()
{

  return {};
}
