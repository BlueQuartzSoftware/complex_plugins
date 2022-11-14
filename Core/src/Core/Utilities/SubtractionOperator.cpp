#include "Core/Utilities/SubtractionOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SubtractionOperator::SubtractionOperator()
{
  setPrecedence(A_Precedence);
  setInfixToken("-");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SubtractionOperator::~SubtractionOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubtractionOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath,
                                    std::stack<ICalculatorArray::Pointer>& executionStack){CREATE_NEW_ARRAY_STANDARD_BINARY(dataStructure, units, calculatedArrayPath, executionStack, -)}

// -----------------------------------------------------------------------------
SubtractionOperator::Pointer SubtractionOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
