#pragma once

#include "Core/Core_export.hpp"
#include "complex/DataStructure/DataArray.hpp"

#include "Core/Utilities/ICalculatorArray.h"

namespace complex
{

template <typename T>
class CORE_EXPORT CalculatorArray : public ICalculatorArray
{
public:
  using Self = CalculatorArray<T>;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  virtual std::string getNameOfClass() const
  {
    return "CalculatorArray<T>";
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static std::string ClassName()
  {
    return "CalculatorArray<T>";
  }

  static Pointer New(DataArray<T>* array, ValueType type)
  {
    return Pointer(new CalculatorArray(array, type));
  }

  ~CalculatorArray() override = default;

  IDataArray* getArray() override
  {
    if(m_ArrayId.has_value())
    {
      return m_DataStructure->getDataAs<IDataArray>(m_ArrayId.value());
    }
    return nullptr;
  }

  void setValue(int i, double val) override
  {
    if(m_ArrayId.has_value())
    {
      m_DataStructure->getDataRefAs<Float64Array>(m_ArrayId.value())[i] = val;
    }
  }

  double getValue(int i) override
  {
    if(!m_ArrayId.has_value())
    {
      // ERROR: The array is empty!
      return 0.0;
    }
    auto& array = m_DataStructure->getDataRefAs<Float64Array>(m_ArrayId.value());
    if(array.getNumberOfTuples() > 1)
    {
      return static_cast<double>(array[i]);
    }
    if(array.getNumberOfTuples() == 1)
    {
      return static_cast<double>(array[0]);
    }
    // ERROR: The array is empty!
    return 0.0;
  }

  ICalculatorArray::ValueType getType() override
  {
    return m_Type;
  }

  // Float64Array* reduceToOneComponent(int c, bool allocate = true) override
  //{
  //   if(c >= 0 && c <= m_Array->getNumberOfComponents())
  //   {
  //     if(m_Array->getNumberOfComponents() > 1)
  //     {
  //       DoubleArrayType::Pointer newArray = DoubleArrayType::CreateArray(m_Array->getNumberOfTuples(), {1}, m_Array->getName(), allocate);
  //       if(allocate)
  //       {
  //         for(int i = 0; i < m_Array->getNumberOfTuples(); i++)
  //         {
  //           newArray->setComponent(i, 0, m_Array->getComponent(i, c));
  //         }
  //       }
  //
  //      return newArray;
  //    }
  //  }
  //
  //  return nullptr;
  //}

  CalculatorItem::ErrorCode checkValidity(std::vector<CalculatorItem::Pointer> infixVector, int currentIndex, std::string& msg) override
  {
    return CalculatorItem::ErrorCode::SUCCESS;
  }

protected:
  CalculatorArray() = default;

  CalculatorArray(DataArray<T>* array, ValueType type)
  : ICalculatorArray()
  , m_ArrayId(std::optional{array->getId()})
  , m_Type(type)
  , m_DataStructure(array->getDataStructure())
  {
  }

private:
  std::optional<DataObject::IdType> m_ArrayId = {};
  ValueType m_Type;
  DataStructure* m_DataStructure = nullptr;

public:
  CalculatorArray(const CalculatorArray&) = delete;            // Copy Constructor Not Implemented
  CalculatorArray(CalculatorArray&&) = delete;                 // Move Constructor Not Implemented
  CalculatorArray& operator=(const CalculatorArray&) = delete; // Copy Assignment Not Implemented
  CalculatorArray& operator=(CalculatorArray&&) = delete;      // Move Assignment Not Implemented
};

} // namespace complex
