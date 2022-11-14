#include "Core/Utilities/CosOperator.h"

#include <cmath>

#include "Core/Utilities/CalculatorArray.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CosOperator::CosOperator()
{
  setNumberOfArguments(1);
  setInfixToken("cos");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CosOperator::~CosOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CosOperator::calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath,
                            std::stack<ICalculatorArray::Pointer>& executionStack){CREATE_NEW_ARRAY_TRIG(dataStructure, units, calculatedArrayPath, executionStack, cos)}

// -----------------------------------------------------------------------------
CosOperator::Pointer CosOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
