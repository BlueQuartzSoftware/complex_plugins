#include "ArrayCalculatorFilter.hpp"

#include "Core/Filters/Algorithms/ArrayCalculator.hpp"

#include "complex/Common/TypesUtility.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/CreateArrayAction.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/AttributeMatrixSelectionParameter.hpp"
#include "complex/Parameters/CalculatorParameter.hpp"
#include "complex/Parameters/NumericTypeParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string ArrayCalculatorFilter::name() const
{
  return FilterTraits<ArrayCalculatorFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string ArrayCalculatorFilter::className() const
{
  return FilterTraits<ArrayCalculatorFilter>::className;
}

//------------------------------------------------------------------------------
Uuid ArrayCalculatorFilter::uuid() const
{
  return FilterTraits<ArrayCalculatorFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string ArrayCalculatorFilter::humanName() const
{
  return "Attribute Array Calculator";
}

//------------------------------------------------------------------------------
std::vector<std::string> ArrayCalculatorFilter::defaultTags() const
{
  return {"#Core", "#Generation"};
}

//------------------------------------------------------------------------------
Parameters ArrayCalculatorFilter::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insertSeparator(Parameters::Separator{"Input Parameters"});
  params.insert(std::make_unique<AttributeMatrixSelectionParameter>(k_SelectedAttributeMatrix_Key, "Cell Attribute Matrix",
                                                                    "The attribute matrix containing the target source arrays to be used in the calculation", DataPath{}));
  params.insert(std::make_unique<CalculatorParameter>(k_InfixEquation_Key, "Infix Expression", "The mathematical expression used to calculate the output array", ""));
  params.insertSeparator(Parameters::Separator{"Created Cell Data"});
  params.insert(std::make_unique<NumericTypeParameter>(k_ScalarType_Key, "Scalar Type", "The data type of the calculated array", NumericType::float64));
  params.insert(std::make_unique<ArrayCreationParameter>(k_CalculatedArray_Key, "Calculated Array", "The path to the calculated array", DataPath{}));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ArrayCalculatorFilter::clone() const
{
  return std::make_unique<ArrayCalculatorFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ArrayCalculatorFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                              const std::atomic_bool& shouldCancel) const
{
  auto pSelectedAttributeMatrixValue = filterArgs.value<DataPath>(k_SelectedAttributeMatrix_Key);
  auto pInfixEquationValue = filterArgs.value<CalculatorParameter::ValueType>(k_InfixEquation_Key);
  auto pScalarTypeValue = filterArgs.value<NumericTypeParameter::ValueType>(k_ScalarType_Key);
  auto pCalculatedArrayValue = filterArgs.value<DataPath>(k_CalculatedArray_Key);

  PreflightResult preflightResult;
  complex::Result<OutputActions> resultOutputActions;
  std::vector<PreflightValue> preflightUpdatedValues;

  // check selected attribute matrix type
  const auto& selectedAm = dataStructure.getDataRefAs<AttributeMatrix>(pSelectedAttributeMatrixValue);

  // TODO : parse infix
  // std::vector<CalculatorItem::Pointer> parsedInfix = parseInfixEquation();

  // TODO : check infix individual items validity

  // TODO : collect calculated array dimensions, check for consistent array component dimensions in infix & make sure it yields a numeric result

  // TODO : check expected calculated results dimensions with destination array parent

  {
    auto createArrayAction = std::make_unique<CreateArrayAction>(ConvertNumericTypeToDataType(pScalarTypeValue), std::vector<usize>{NUM_TUPLES_VALUE}, NUM_COMPONENTS, pCalculatedArrayValue);
    resultOutputActions.value().actions.push_back(std::move(createArrayAction));
  }

  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> ArrayCalculatorFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                            const std::atomic_bool& shouldCancel) const
{

  ArrayCalculatorInputValues inputValues;

  inputValues.SelectedAttributeMatrix = filterArgs.value<DataPath>(k_SelectedAttributeMatrix_Key);
  inputValues.InfixEquation = filterArgs.value<<<<NOT_IMPLEMENTED>>>>(k_InfixEquation_Key);
  inputValues.ScalarType = filterArgs.value<<<<NOT_IMPLEMENTED>>>>(k_ScalarType_Key);
  inputValues.CalculatedArray = filterArgs.value<DataPath>(k_CalculatedArray_Key);

  return ArrayCalculator(dataStructure, messageHandler, shouldCancel, &inputValues)();
}
} // namespace complex
