#include "Core/Utilities/ASinOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ASinOperator::ASinOperator()
{
  setNumberOfArguments(1);
  setInfixToken("asin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ASinOperator::~ASinOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ASinOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath,
                             std::stack<ICalculatorArray::Pointer>& executionStack){CREATE_NEW_ARRAY_ARCTRIG(dataStructure, units, calculatedArrayPath, executionStack, asin)}

// -----------------------------------------------------------------------------
ASinOperator::Pointer ASinOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
