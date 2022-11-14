#include "Core/Utilities/PowOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PowOperator::PowOperator()
{
  setPrecedence(C_Precedence);
  setInfixToken("^");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PowOperator::~PowOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PowOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath,
                            std::stack<ICalculatorArray::Pointer>& executionStack){CREATE_NEW_ARRAY_TWO_ARGUMENTS(dataStructure, units, calculatedArrayPath, executionStack, pow)}

// -----------------------------------------------------------------------------
PowOperator::Pointer PowOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
