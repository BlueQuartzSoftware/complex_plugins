#include "Core/Utilities/ABSOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ABSOperator::ABSOperator()
{
  setNumberOfArguments(1);
  setInfixToken("abs");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ABSOperator::~ABSOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ABSOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath, std::stack<ICalculatorArray::Pointer>& executionStack)
{
  CREATE_NEW_ARRAY_STANDARD_UNARY(dataStructure, units, calculatedArrayPath, executionStack, fabs);
}

// -----------------------------------------------------------------------------
ABSOperator::Pointer ABSOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
