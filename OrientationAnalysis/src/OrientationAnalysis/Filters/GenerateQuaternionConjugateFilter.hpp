#pragma once

#include "OrientationAnalysis/OrientationAnalysis_export.hpp"

#include "complex/Filter/FilterTraits.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{
/**
 * @class GenerateQuaternionConjugateFilter
 * @brief This filter will ....
 */
class ORIENTATIONANALYSIS_EXPORT GenerateQuaternionConjugateFilter : public IFilter
{
public:
  GenerateQuaternionConjugateFilter() = default;
  ~GenerateQuaternionConjugateFilter() noexcept override = default;

  GenerateQuaternionConjugateFilter(const GenerateQuaternionConjugateFilter&) = delete;
  GenerateQuaternionConjugateFilter(GenerateQuaternionConjugateFilter&&) noexcept = delete;

  GenerateQuaternionConjugateFilter& operator=(const GenerateQuaternionConjugateFilter&) = delete;
  GenerateQuaternionConjugateFilter& operator=(GenerateQuaternionConjugateFilter&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_CellQuatsArrayPath_Key = "quaternion_data_array_path";
  static inline constexpr StringLiteral k_OutputDataArrayPath_Key = "output_data_array_path";
  static inline constexpr StringLiteral k_DeleteOriginalData_Key = "delete_original_data";

  /**
   * @brief Returns the name of the filter.
   * @return
   */
  std::string name() const override;

  /**
   * @brief Returns the C++ classname of this filter.
   * @return
   */
  std::string className() const override;

  /**
   * @brief Returns the uuid of the filter.
   * @return
   */
  Uuid uuid() const override;

  /**
   * @brief Returns the human readable name of the filter.
   * @return
   */
  std::string humanName() const override;

  /**
   * @brief Returns the default tags for this filter.
   * @return
   */
  std::vector<std::string> defaultTags() const override;

  /**
   * @brief Returns the parameters of the filter (i.e. its inputs)
   * @return
   */
  Parameters parameters() const override;

  /**
   * @brief Returns a copy of the filter.
   * @return
   */
  UniquePointer clone() const override;

protected:
  /**
   * @brief Takes in a DataStructure and checks that the filter can be run on it with the given arguments.
   * Returns any warnings/errors. Also returns the changes that would be applied to the DataStructure.
   * Some parts of the actions may not be completely filled out if all the required information is not available at preflight time.
   * @param dataStructure The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  PreflightResult preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;

  /**
   * @brief Applies the filter's algorithm to the DataStructure with the given arguments. Returns any warnings/errors.
   * On failure, there is no guarantee that the DataStructure is in a correct state.
   * @param dataStructure The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  Result<> executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;
};
} // namespace complex

COMPLEX_DEF_FILTER_TRAITS(complex, GenerateQuaternionConjugateFilter, "e1653c62-3894-4b95-8449-605400038110");
/* LEGACY UUID FOR THIS FILTER 630d7486-75ea-5e04-874c-894460cd7c4d */
