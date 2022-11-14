#include "Core/Utilities/Log10Operator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Log10Operator::Log10Operator()
{
  setNumberOfArguments(1);
  setInfixToken("log10");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Log10Operator::~Log10Operator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Log10Operator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath, std::stack<ICalculatorArray::Pointer>& executionStack)
{
  CREATE_NEW_ARRAY_STANDARD_UNARY(dataStructure, units, calculatedArrayPath, executionStack, log10);
}

// -----------------------------------------------------------------------------
Log10Operator::Pointer Log10Operator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
