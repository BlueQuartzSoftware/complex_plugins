#pragma once

#include <memory>
#include <stack>

#include "Core/Utilities/CalculatorOperator.h"

#include "complex/DataStructure/DataPath.hpp"

namespace complex
{

class CORE_EXPORT BinaryOperator : public CalculatorOperator
{
public:
  using Self = BinaryOperator;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New()
  {
    return Pointer(new BinaryOperator());
  }

  ~BinaryOperator() override;

  void calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath, std::stack<ICalculatorArray::Pointer>& executionStack) override;

  CalculatorItem::ErrorCode checkValidity(std::vector<CalculatorItem::Pointer> infixVector, int currentIndex, std::string& msg) final;

protected:
  BinaryOperator();

public:
  BinaryOperator(const BinaryOperator&) = delete;            // Copy Constructor Not Implemented
  BinaryOperator(BinaryOperator&&) = delete;                 // Move Constructor Not Implemented
  BinaryOperator& operator=(const BinaryOperator&) = delete; // Copy Assignment Not Implemented
  BinaryOperator& operator=(BinaryOperator&&) = delete;      // Move Assignment Not Implemented

private:
};

#define CREATE_NEW_ARRAY_STANDARD_BINARY(dataStructure, units, calculatedArrayPath, executionStack, op)                                                                                                \
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
    for(int i = 0; i < newArray->getNumberOfTuples(); i++)                                                                                                                                             \
    {                                                                                                                                                                                                  \
      for(int c = 0; c < newArray->getNumberOfComponents(); c++)                                                                                                                                       \
      {                                                                                                                                                                                                \
        int index = numComps * i + c;                                                                                                                                                                  \
        double num1 = array1->getValue(index);                                                                                                                                                         \
        double num2 = array2->getValue(index);                                                                                                                                                         \
        (*newArray)[index] = num2 op num1;                                                                                                                                                             \
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
