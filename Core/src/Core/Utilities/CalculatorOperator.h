#pragma once

#include <memory>

#include <stack>

#include "Core/Core_export.hpp"

#include "Core/Utilities/CalculatorArray.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/CalculatorParameter.hpp"

namespace complex
{

class CORE_EXPORT CalculatorOperator : public CalculatorItem
{
public:
  enum OperatorType
  {
    Unary,
    Binary
  };

  using Self = CalculatorOperator;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static double toDegrees(double radians);
  static double toRadians(double degrees);

  ~CalculatorOperator() override;

  bool hasHigherPrecedence(CalculatorOperator::Pointer other);

  virtual void calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath, std::stack<ICalculatorArray::Pointer>& executionStack) = 0;

  OperatorType getOperatorType();

protected:
  CalculatorOperator();

  enum Precedence
  {
    Unknown_Precedence,
    A_Precedence,
    B_Precedence,
    C_Precedence,
    D_Precedence,
    E_Precedence
  };

  double root(double base, double root);

  Precedence getPrecedence();
  void setPrecedence(Precedence precedence);

  void setOperatorType(OperatorType type);

private:
  Precedence m_Precedence = {Unknown_Precedence};
  OperatorType m_OperatorType;

public:
  CalculatorOperator(const CalculatorOperator&) = delete;            // Copy Constructor Not Implemented
  CalculatorOperator(CalculatorOperator&&) = delete;                 // Move Constructor Not Implemented
  CalculatorOperator& operator=(const CalculatorOperator&) = delete; // Copy Assignment Not Implemented
  CalculatorOperator& operator=(CalculatorOperator&&) = delete;      // Move Assignment Not Implemented
};

#define CREATE_NEW_ARRAY_TWO_ARGUMENTS(dataStructure, units, calculatedArrayPath, executionStack, func)                                                                                                \
  ICalculatorArray::Pointer array1 = executionStack.top();                                                                                                                                             \
  if(executionStack.size() >= 2 && nullptr != array1)                                                                                                                                                  \
  {                                                                                                                                                                                                    \
    executionStack.pop();                                                                                                                                                                              \
    ICalculatorArray::Pointer array2 = executionStack.top();                                                                                                                                           \
    executionStack.pop();                                                                                                                                                                              \
                                                                                                                                                                                                       \
    Float64Array* newArray = nullptr;                                                                                                                                                                  \
    if(array1->getType() == ICalculatorArray::Array)                                                                                                                                                   \
    {                                                                                                                                                                                                  \
      newArray = Float64Array::CreateWithStore<Float64DataStore>(dataStructure, calculatedArrayPath.getTargetName(), array1->getArray()->getTupleShape(), array1->getArray()->getComponentShape());    \
    }                                                                                                                                                                                                  \
    else                                                                                                                                                                                               \
    {                                                                                                                                                                                                  \
      newArray = Float64Array::CreateWithStore<Float64DataStore>(dataStructure, calculatedArrayPath.getTargetName(), array2->getArray()->getTupleShape(), array2->getArray()->getComponentShape());    \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    int numComps = newArray->getNumberOfComponents();                                                                                                                                                  \
    for(int i = 0; i < static_cast<int>(newArray->getNumberOfTuples()); i++)                                                                                                                           \
    {                                                                                                                                                                                                  \
      for(int c = 0; c < newArray->getNumberOfComponents(); c++)                                                                                                                                       \
      {                                                                                                                                                                                                \
        int index = numComps * i + c;                                                                                                                                                                  \
        double num1 = array1->getValue(index);                                                                                                                                                         \
        double num2 = array2->getValue(index);                                                                                                                                                         \
        (*newArray)[index] = func(num2, num1);                                                                                                                                                         \
      }                                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    if(array1->getType() == ICalculatorArray::Array || array2->getType() == ICalculatorArray::Array)                                                                                                   \
    {                                                                                                                                                                                                  \
      executionStack.push(CalculatorArray<double>::New(dataStructure, newArray, ICalculatorArray::Array, true));                                                                                       \
    }                                                                                                                                                                                                  \
    else                                                                                                                                                                                               \
    {                                                                                                                                                                                                  \
      executionStack.push(CalculatorArray<double>::New(dataStructure, newArray, ICalculatorArray::Number, true));                                                                                      \
    }                                                                                                                                                                                                  \
    return;                                                                                                                                                                                            \
  }

} // namespace complex
