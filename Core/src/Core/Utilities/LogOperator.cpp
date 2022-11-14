#include "Core/Utilities/LogOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LogOperator::LogOperator()
{
  setNumberOfArguments(2);
  setInfixToken("log");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LogOperator::~LogOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LogOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath, std::stack<ICalculatorArray::Pointer>& executionStack)
{
  CREATE_NEW_ARRAY_TWO_ARGUMENTS(dataStructure, units, calculatedArrayPath, executionStack, log_arbitrary_base)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double LogOperator::log_arbitrary_base(double base, double value)
{
  return log(value) / log(base);
}

// -----------------------------------------------------------------------------
LogOperator::Pointer LogOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
