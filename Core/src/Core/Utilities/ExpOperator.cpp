#include "Core/Utilities/ExpOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExpOperator::ExpOperator()
{
  setNumberOfArguments(1);
  setInfixToken("exp");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExpOperator::~ExpOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExpOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath, std::stack<ICalculatorArray::Pointer>& executionStack)
{
  CREATE_NEW_ARRAY_STANDARD_UNARY(dataStructure, units, calculatedArrayPath, executionStack, exp);
}

// -----------------------------------------------------------------------------
ExpOperator::Pointer ExpOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
