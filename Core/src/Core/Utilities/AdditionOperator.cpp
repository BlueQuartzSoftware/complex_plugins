#include "Core/Utilities/AdditionOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdditionOperator::AdditionOperator()
{
  setPrecedence(A_Precedence);
  setInfixToken("+");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdditionOperator::~AdditionOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdditionOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath,
                                 std::stack<ICalculatorArray::Pointer>& executionStack){CREATE_NEW_ARRAY_STANDARD_BINARY(dataStructure, units, calculatedArrayPath, executionStack, +)}

// -----------------------------------------------------------------------------
AdditionOperator::Pointer AdditionOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
