#include "Core/Utilities/MultiplicationOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiplicationOperator::MultiplicationOperator()
{
  setPrecedence(B_Precedence);
  setInfixToken("*");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiplicationOperator::~MultiplicationOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiplicationOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath,
                                       std::stack<ICalculatorArray::Pointer>& executionStack){CREATE_NEW_ARRAY_STANDARD_BINARY(dataStructure, units, calculatedArrayPath, executionStack, *)}

// -----------------------------------------------------------------------------
MultiplicationOperator::Pointer MultiplicationOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
