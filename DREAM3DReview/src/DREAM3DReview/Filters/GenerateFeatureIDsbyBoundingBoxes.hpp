#pragma once

#include "DREAM3DReview/DREAM3DReview_export.hpp"

#include "complex/Filter/FilterTraits.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{
/**
 * @class GenerateFeatureIDsbyBoundingBoxes
 * @brief This filter will ....
 */
class DREAM3DREVIEW_EXPORT GenerateFeatureIDsbyBoundingBoxes : public IFilter
{
public:
  GenerateFeatureIDsbyBoundingBoxes() = default;
  ~GenerateFeatureIDsbyBoundingBoxes() noexcept override = default;

  GenerateFeatureIDsbyBoundingBoxes(const GenerateFeatureIDsbyBoundingBoxes&) = delete;
  GenerateFeatureIDsbyBoundingBoxes(GenerateFeatureIDsbyBoundingBoxes&&) noexcept = delete;

  GenerateFeatureIDsbyBoundingBoxes& operator=(const GenerateFeatureIDsbyBoundingBoxes&) = delete;
  GenerateFeatureIDsbyBoundingBoxes& operator=(GenerateFeatureIDsbyBoundingBoxes&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_FeatureIDsArrayPath_Key = "FeatureIDsArrayPath";
  static inline constexpr StringLiteral k_FeatureAttributeMatrixArrayPath_Key = "FeatureAttributeMatrixArrayPath";
  static inline constexpr StringLiteral k_BoxCenterArrayPath_Key = "BoxCenterArrayPath";
  static inline constexpr StringLiteral k_BoxDimensionsArrayPath_Key = "BoxDimensionsArrayPath";
  static inline constexpr StringLiteral k_BoxFeatureIDsArrayPath_Key = "BoxFeatureIDsArrayPath";

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
   * @param ds The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  PreflightResult preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;

  /**
   * @brief Applies the filter's algorithm to the DataStructure with the given arguments. Returns any warnings/errors.
   * On failure, there is no guarantee that the DataStructure is in a correct state.
   * @param ds The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  Result<> executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;
};
} // namespace complex

COMPLEX_DEF_FILTER_TRAITS(complex, GenerateFeatureIDsbyBoundingBoxes, "8b2fa51e-3bad-58ec-9fbf-b03b065e67fc");
