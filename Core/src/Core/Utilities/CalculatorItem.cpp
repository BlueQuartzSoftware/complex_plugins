#include "Core/Utilities/CalculatorItem.h"

#include "Core/Utilities/ICalculatorArray.h"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculatorItem::CalculatorItem() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculatorItem::~CalculatorItem() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string CalculatorItem::getInfixToken()
{
  return m_InfixToken;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorItem::setInfixToken(const std::string& token)
{
  m_InfixToken = token;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CalculatorItem::isICalculatorArray()
{
  return (nullptr != dynamic_cast<ICalculatorArray*>(this));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CalculatorItem::isArray()
{
  ICalculatorArray* calculatorArray = dynamic_cast<ICalculatorArray*>(this);
  if(calculatorArray != nullptr)
  {
    return (calculatorArray->getType() == ICalculatorArray::ValueType::Array);
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CalculatorItem::isNumber()
{
  ICalculatorArray* calculatorArray = dynamic_cast<ICalculatorArray*>(this);
  if(calculatorArray != nullptr)
  {
    return (calculatorArray->getType() == ICalculatorArray::ValueType::Number);
  }

  return false;
}

// -----------------------------------------------------------------------------
CalculatorItem::Pointer CalculatorItem::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
