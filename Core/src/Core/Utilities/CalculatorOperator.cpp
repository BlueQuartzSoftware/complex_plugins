#include "Core/Utilities/CalculatorOperator.h"

#include "complex/Common/Numbers.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculatorOperator::CalculatorOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculatorOperator::~CalculatorOperator() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CalculatorOperator::hasHigherPrecedence(const CalculatorOperator::Pointer other)
{
  return m_Precedence > other->m_Precedence;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculatorOperator::Precedence CalculatorOperator::getPrecedence()
{
  return m_Precedence;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorOperator::setPrecedence(Precedence precedence)
{
  m_Precedence = precedence;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculatorOperator::OperatorType CalculatorOperator::getOperatorType()
{
  return m_OperatorType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculatorOperator::setOperatorType(OperatorType type)
{
  m_OperatorType = type;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double CalculatorOperator::toDegrees(double radians)
{
  return radians * (180.0 / numbers::pi);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double CalculatorOperator::toRadians(double degrees)
{
  return degrees * (numbers::pi / 180.0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double CalculatorOperator::root(double base, double root)
{
  if(root == 0)
  {
    return std::numeric_limits<double>::infinity();
  }

  return pow(base, 1 / root);
}

// -----------------------------------------------------------------------------
CalculatorOperator::Pointer CalculatorOperator::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}
