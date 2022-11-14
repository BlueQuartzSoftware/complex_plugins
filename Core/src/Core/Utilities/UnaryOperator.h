#pragma once

#include <memory>
#include <stack>

#include "Core/Utilities/CalculatorOperator.h"

#include "complex/DataStructure/DataPath.hpp"

namespace complex
{
class CORE_EXPORT UnaryOperator : public CalculatorOperator
{
public:
  using Self = UnaryOperator;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New()
  {
    return Pointer(new UnaryOperator());
  }

  ~UnaryOperator() override;

  void calculate(DataStructure& dataStructure, CalculatorParameter::AngleUnits units, DataPath calculatedArrayPath, std::stack<ICalculatorArray::Pointer>& executionStack) override;

  CalculatorItem::ErrorCode checkValidity(std::vector<CalculatorItem::Pointer> infixVector, int currentIndex, std::string& msg) final;

  int getNumberOfArguments();

protected:
  UnaryOperator();

  void setNumberOfArguments(int numOfArguments);

private:
  int m_NumOfArguments;

public:
  UnaryOperator(const UnaryOperator&) = delete;            // Copy Constructor Not Implemented
  UnaryOperator(UnaryOperator&&) = delete;                 // Move Constructor Not Implemented
  UnaryOperator& operator=(const UnaryOperator&) = delete; // Copy Assignment Not Implemented
  UnaryOperator& operator=(UnaryOperator&&) = delete;      // Move Assignment Not Implemented
};

#define CREATE_NEW_ARRAY_STANDARD_UNARY(dataStructure, units, calculatedArrayPath, executionStack, func)                                                                                               \
  ICalculatorArray::Pointer arrayPtr = executionStack.top();                                                                                                                                           \
  if(!executionStack.empty() && nullptr != arrayPtr)                                                                                                                                                   \
  {                                                                                                                                                                                                    \
    executionStack.pop();                                                                                                                                                                              \
                                                                                                                                                                                                       \
    Float64Array* newArray =                                                                                                                                                                           \
        Float64Array::CreateWithStore<Float64DataStore>(dataStructure, calculatedArrayPath.getTargetName(), arrayPtr->getArray()->getTupleShape(), arrayPtr->getArray()->getComponentShape());         \
                                                                                                                                                                                                       \
    int numComps = newArray->getNumberOfComponents();                                                                                                                                                  \
    for(int i = 0; i < newArray->getNumberOfTuples(); i++)                                                                                                                                             \
    {                                                                                                                                                                                                  \
      for(int c = 0; c < newArray->getNumberOfComponents(); c++)                                                                                                                                       \
      {                                                                                                                                                                                                \
        int index = numComps * i + c;                                                                                                                                                                  \
        double num = arrayPtr->getValue(index);                                                                                                                                                        \
        (*newArray)[index] = func(num);                                                                                                                                                                \
      }                                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    executionStack.push(CalculatorArray<double>::New(newArray, arrayPtr->getType()));                                                                                                                  \
    return;                                                                                                                                                                                            \
  }

#define CREATE_NEW_ARRAY_TRIG(dataStructure, units, calculatedArrayPath, executionStack, func)                                                                                                         \
  ICalculatorArray::Pointer arrayPtr = executionStack.top();                                                                                                                                           \
  if(!executionStack.empty() && nullptr != arrayPtr)                                                                                                                                                   \
  {                                                                                                                                                                                                    \
    executionStack.pop();                                                                                                                                                                              \
    Float64Array* newArray =                                                                                                                                                                           \
        Float64Array::CreateWithStore<Float64DataStore>(dataStructure, calculatedArrayPath.getTargetName(), arrayPtr->getArray()->getTupleShape(), arrayPtr->getArray()->getComponentShape());         \
                                                                                                                                                                                                       \
    int numComps = newArray->getNumberOfComponents();                                                                                                                                                  \
    for(int i = 0; i < newArray->getNumberOfTuples(); i++)                                                                                                                                             \
    {                                                                                                                                                                                                  \
      for(int c = 0; c < newArray->getNumberOfComponents(); c++)                                                                                                                                       \
      {                                                                                                                                                                                                \
        int index = numComps * i + c;                                                                                                                                                                  \
        double num = arrayPtr->getValue(index);                                                                                                                                                        \
                                                                                                                                                                                                       \
        if(units == CalculatorParameter::AngleUnits::Degrees)                                                                                                                                          \
        {                                                                                                                                                                                              \
          (*newArray)[index] = func(toRadians(num));                                                                                                                                                   \
        }                                                                                                                                                                                              \
        else                                                                                                                                                                                           \
        {                                                                                                                                                                                              \
          (*newArray)[index] = func(num);                                                                                                                                                              \
        }                                                                                                                                                                                              \
      }                                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    executionStack.push(CalculatorArray<double>::New(newArray, arrayPtr->getType()));                                                                                                                  \
    return;                                                                                                                                                                                            \
  }

#define CREATE_NEW_ARRAY_ARCTRIG(dataStructure, units, calculatedArrayPath, executionStack, func)                                                                                                      \
  ICalculatorArray::Pointer arrayPtr = executionStack.top();                                                                                                                                           \
  if(!executionStack.empty() && nullptr != arrayPtr)                                                                                                                                                   \
  {                                                                                                                                                                                                    \
    executionStack.pop();                                                                                                                                                                              \
                                                                                                                                                                                                       \
    Float64Array* newArray =                                                                                                                                                                           \
        Float64Array::CreateWithStore<Float64DataStore>(dataStructure, calculatedArrayPath.getTargetName(), arrayPtr->getArray()->getTupleShape(), arrayPtr->getArray()->getComponentShape());         \
                                                                                                                                                                                                       \
    int numComps = newArray->getNumberOfComponents();                                                                                                                                                  \
    for(int i = 0; i < newArray->getNumberOfTuples(); i++)                                                                                                                                             \
    {                                                                                                                                                                                                  \
      for(int c = 0; c < newArray->getNumberOfComponents(); c++)                                                                                                                                       \
      {                                                                                                                                                                                                \
        int index = numComps * i + c;                                                                                                                                                                  \
        double num = arrayPtr->getValue(index);                                                                                                                                                        \
                                                                                                                                                                                                       \
        if(units == CalculatorParameter::AngleUnits::Degrees)                                                                                                                                          \
        {                                                                                                                                                                                              \
          (*newArray)[index] = toDegrees(func(num));                                                                                                                                                   \
        }                                                                                                                                                                                              \
        else                                                                                                                                                                                           \
        {                                                                                                                                                                                              \
          (*newArray)[index] = func(num);                                                                                                                                                              \
        }                                                                                                                                                                                              \
      }                                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    executionStack.push(CalculatorArray<double>::New(newArray, arrayPtr->getType()));                                                                                                                  \
    return;                                                                                                                                                                                            \
  }

} // namespace complex
