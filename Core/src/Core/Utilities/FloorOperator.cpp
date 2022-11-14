#include "Core/Utilities/FloorOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloorOperator::FloorOperator()
{
  setNumberOfArguments(1);
  setInfixToken("floor");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloorOperator::~FloorOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloorOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath,
                              std::stack<ICalculatorArray::Pointer>& executionStack){CREATE_NEW_ARRAY_STANDARD_UNARY(dataStructure, units, calculatedArrayPath, executionStack, floor)}

// -----------------------------------------------------------------------------
FloorOperator::Pointer FloorOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
