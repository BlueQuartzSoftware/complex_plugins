#pragma once

#include "Core/Core_export.hpp"
#include "Core/Utilities/CalculatorItem.h"

#include "complex/DataStructure/AttributeMatrix.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Filter/IFilter.hpp"
#include "complex/Parameters/NumericTypeParameter.hpp"

namespace complex
{

struct CORE_EXPORT ArrayCalculatorInputValues
{
  DataPath SelectedAttributeMatrix;
  std::string InfixEquation;
  NumericType ScalarType;
  DataPath CalculatedArray;
};

/**
 * @class ConditionalSetValue
 * @brief This filter replaces values in the target array with a user specified value
 * where a bool mask array specifies.
 */

class CORE_EXPORT ArrayCalculator
{
public:
  ArrayCalculator(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, ArrayCalculatorInputValues* inputValues);
  ~ArrayCalculator() noexcept;

  ArrayCalculator(const ArrayCalculator&) = delete;
  ArrayCalculator(ArrayCalculator&&) noexcept = delete;
  ArrayCalculator& operator=(const ArrayCalculator&) = delete;
  ArrayCalculator& operator=(ArrayCalculator&&) noexcept = delete;

  Result<> operator()();

  const std::atomic_bool& getCancel();

  static std::vector<CalculatorItem*> ParseInfixEquation(const AttributeMatrix& selectedAttributeMatrix, const std::string& infixEquation);
  static std::vector<std::string> GetRegularExpressionMatches(const std::string& infixEquation);

private:
  DataStructure& m_DataStructure;
  const ArrayCalculatorInputValues* m_InputValues = nullptr;
  const std::atomic_bool& m_ShouldCancel;
  const IFilter::MessageHandler& m_MessageHandler;
};

} // namespace complex
