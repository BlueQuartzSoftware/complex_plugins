#include "Core/Utilities/RootOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RootOperator::RootOperator()
{
  setNumberOfArguments(2);
  setInfixToken("root");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RootOperator::~RootOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RootOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath,
                             std::stack<ICalculatorArray::Pointer>& executionStack){CREATE_NEW_ARRAY_TWO_ARGUMENTS(dataStructure, units, calculatedArrayPath, executionStack, root)}

// -----------------------------------------------------------------------------
RootOperator::Pointer RootOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
