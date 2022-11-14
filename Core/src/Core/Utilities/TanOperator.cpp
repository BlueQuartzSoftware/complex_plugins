#include "Core/Utilities/TanOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TanOperator::TanOperator()
{
  setNumberOfArguments(1);
  setInfixToken("tan");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TanOperator::~TanOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TanOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath,
                            std::stack<ICalculatorArray::Pointer>& executionStack){CREATE_NEW_ARRAY_TRIG(dataStructure, units, calculatedArrayPath, executionStack, tan)}

// -----------------------------------------------------------------------------
TanOperator::Pointer TanOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
