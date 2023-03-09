#pragma once

#include "OrientationAnalysis/OrientationAnalysis_export.hpp"

#include "complex/Filter/FilterTraits.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{
/**
 * @class EMsoftSO3Sampler
 * @brief This filter will ....
 */
class ORIENTATIONANALYSIS_EXPORT EMsoftSO3Sampler : public IFilter
{
public:
  EMsoftSO3Sampler() = default;
  ~EMsoftSO3Sampler() noexcept override = default;

  EMsoftSO3Sampler(const EMsoftSO3Sampler&) = delete;
  EMsoftSO3Sampler(EMsoftSO3Sampler&&) noexcept = delete;

  EMsoftSO3Sampler& operator=(const EMsoftSO3Sampler&) = delete;
  EMsoftSO3Sampler& operator=(EMsoftSO3Sampler&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_sampleModeSelector_Key = "sample_mode_selector";
  static inline constexpr StringLiteral k_PointGroup_Key = "point_group";
  static inline constexpr StringLiteral k_OffsetGrid_Key = "offset_grid";
  static inline constexpr StringLiteral k_MisOr_Key = "mis_or";
  static inline constexpr StringLiteral k_RefOr_Key = "ref_or";
  static inline constexpr StringLiteral k_MisOrFull_Key = "mis_or_full";
  static inline constexpr StringLiteral k_RefOrFull_Key = "ref_or_full";
  static inline constexpr StringLiteral k_Numsp_Key = "numsp";
  static inline constexpr StringLiteral k_EulerAnglesArrayName_Key = "euler_angles_array_name";

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

COMPLEX_DEF_FILTER_TRAITS(complex, EMsoftSO3Sampler, "74478e86-ce29-40b8-8c17-d20009195f91");