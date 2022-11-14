#include "Core/Utilities/CeilOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CeilOperator::CeilOperator()
{
  setNumberOfArguments(1);
  setInfixToken("ceil");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CeilOperator::~CeilOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CeilOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath, std::stack<ICalculatorArray::Pointer>& executionStack)
{
  CREATE_NEW_ARRAY_STANDARD_UNARY(dataStructure, units, calculatedArrayPath, executionStack, ceil);
}

// -----------------------------------------------------------------------------
CeilOperator::Pointer CeilOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
