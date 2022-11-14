#include "ArrayCalculator.hpp"

#include "Core/Utilities/CommaSeparator.h"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/Utilities/StringUtilities.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
ArrayCalculator::ArrayCalculator(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, ArrayCalculatorInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
ArrayCalculator::~ArrayCalculator() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& ArrayCalculator::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> ArrayCalculator::operator()()
{
  /**
   * This section of the code should contain the actual algorithmic codes that
   * will accomplish the goal of the file.
   *
   * If you can parallelize the code there are a number of examples on how to do that.
   *    GenerateIPFColors is one example
   *
   * If you need to determine what kind of array you have (Int32Array, Float32Array, etc)
   * look to the ExecuteDataFunction() in complex/Utilities/FilterUtilities.hpp template
   * function to help with that code.
   *   An Example algorithm class is `CombineAttributeArrays` and `RemoveFlaggedVertices`
   *
   * There are other utility classes that can help alleviate the amount of code that needs
   * to be written.
   *
   * REMOVE THIS COMMENT BLOCK WHEN YOU ARE FINISHED WITH THE FILTER_HUMAN_NAME
   */

  return {};
}

// -----------------------------------------------------------------------------
std::vector<CalculatorItem::Pointer> ArrayCalculator::ParseInfixEquation(const AttributeMatrix& selectedAttributeMatrix, const std::string& infixEquation)
{
  int err = 0;

  std::vector<std::string> itemList = GetRegularExpressionMatches(infixEquation);

  // Iterate through the QStringList and create the proper CalculatorItems
  std::vector<CalculatorItem::Pointer> parsedInfix;

  for(int i = 0; i < itemList.size(); i++)
  {
    std::string strItem = itemList[i];
    CalculatorItem::Pointer itemPtr;

    bool ok;
    double num = std::stod(strItem);
    if(ok)
    {
      // This is a numeric value
      parseNumericValue(strItem, parsedInfix, num);
    }
    else if(strItem == "-")
    {
      // This is a minus sign
      parseMinusSign(strItem, parsedInfix, i);
    }
    else if(StringUtilities::contains(strItem, "[") && StringUtilities::contains(strItem, "]"))
    {
      // This is an index operator
      if(!parseIndexOperator(strItem, parsedInfix))
      {
        return std::vector<CalculatorItem::Pointer>();
      }
    }
    else
    {
      itemPtr = m_SymbolMap.value(strItem);

      if(nullptr != std::dynamic_pointer_cast<CommaSeparator>(itemPtr))
      {
        // This is a comma operator
        if(!parseCommaOperator(strItem, parsedInfix))
        {
          return std::vector<CalculatorItem::Pointer>();
        }
      }
      else if(nullptr != itemPtr)
      {
        // This is another type of operator
        std::string ss = fmt::format("Item '{}' in the infix expression is the name of an array in the selected Attribute Matrix, but it is currently being used as a mathematical operator", strItem);
        checkForAmbiguousArrayName(strItem, ss);

        parsedInfix.push_back(itemPtr);
      }
      else if(selectedAM->getAttributeArrayNames().contains(strItem) || (!strItem.empty() && strItem[0] == '\"' && strItem[strItem.size() - 1] == '\"'))
      {
        if(!parseArray(strItem, parsedInfix, selectedAM))
        {
          return std::vector<CalculatorItem::Pointer>();
        }
      }
      else
      {
        std::string ss = fmt::format("An unrecognized item '{}' was found in the chosen infix expression", strItem);
        setErrorCondition(static_cast<int>(CalculatorItem::ErrorCode::UNRECOGNIZED_ITEM), ss);
        return std::vector<CalculatorItem::Pointer>();
      }
    }
  }

  // Return the parsed infix expression as a vector of CalculatorItems
  return parsedInfix;
}

// -----------------------------------------------------------------------------
std::vector<std::string> ArrayCalculator::GetRegularExpressionMatches(const std::string& infixEquation)
{
  // Parse all the items into a QVector of strings using a regular expression
  std::vector<std::string> itemList;
  // Match all array names that start with two alphabetical characters and have spaces.  Match all numbers, decimal or integer.
  // Match one letter array names.  Match all special character operators.
  QRegularExpression regExp("(\"((\\[)?\\d+(\\.\\d+)?(\\])?|(\\[)?\\.\\d+(\\])?|\\w{1,1}((\\w|\\s|\\d)*(\\w|\\d){1,1})?|\\S)\")|(((\\[)?\\d+(\\.\\d+)?(\\])?|(\\[)?\\.\\d+(\\])?|\\w{1,1}((\\w|\\s|\\d)"
                            "*(\\w|\\d){1,1})?|\\S))");
  QRegularExpressionMatchIterator iter = regExp.globalMatch(infixEquation);
  while(iter.hasNext())
  {
    QRegularExpressionMatch match = iter.next();
    itemList.push_back(match.captured());
  }

  return itemList;
}
