#include "Core/Utilities/LnOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LnOperator::LnOperator()
{
  setNumberOfArguments(1);
  setInfixToken("ln");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LnOperator::~LnOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LnOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath, std::stack<ICalculatorArray::Pointer>& executionStack)
{
  CREATE_NEW_ARRAY_STANDARD_UNARY(dataStructure, units, calculatedArrayPath, executionStack, log);
}

// -----------------------------------------------------------------------------
LnOperator::Pointer LnOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
