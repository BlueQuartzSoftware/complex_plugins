#include "Core/Utilities/ATanOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ATanOperator::ATanOperator()
{
  setNumberOfArguments(1);
  setInfixToken("atan");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ATanOperator::~ATanOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ATanOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath,
                             std::stack<ICalculatorArray::Pointer>& executionStack){CREATE_NEW_ARRAY_ARCTRIG(dataStructure, units, calculatedArrayPath, executionStack, atan)}

// -----------------------------------------------------------------------------
ATanOperator::Pointer ATanOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
