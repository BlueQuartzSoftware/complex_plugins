#pragma once

#include "OrientationAnalysis/OrientationAnalysis_export.hpp"

#include "complex/Filter/FilterTraits.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{
/**
 * @class FindFeatureNeighborCAxisMisalignments
 * @brief This filter will ....
 */
class ORIENTATIONANALYSIS_EXPORT FindFeatureNeighborCAxisMisalignments : public IFilter
{
public:
  FindFeatureNeighborCAxisMisalignments() = default;
  ~FindFeatureNeighborCAxisMisalignments() noexcept override = default;

  FindFeatureNeighborCAxisMisalignments(const FindFeatureNeighborCAxisMisalignments&) = delete;
  FindFeatureNeighborCAxisMisalignments(FindFeatureNeighborCAxisMisalignments&&) noexcept = delete;

  FindFeatureNeighborCAxisMisalignments& operator=(const FindFeatureNeighborCAxisMisalignments&) = delete;
  FindFeatureNeighborCAxisMisalignments& operator=(FindFeatureNeighborCAxisMisalignments&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_FindAvgMisals_Key = "find_avg_misals";
  static inline constexpr StringLiteral k_NeighborListArrayPath_Key = "neighbor_list_array_path";
  static inline constexpr StringLiteral k_AvgQuatsArrayPath_Key = "avg_quats_array_path";
  static inline constexpr StringLiteral k_FeaturePhasesArrayPath_Key = "feature_phases_array_path";
  static inline constexpr StringLiteral k_CrystalStructuresArrayPath_Key = "crystal_structures_array_path";
  static inline constexpr StringLiteral k_CAxisMisalignmentListArrayName_Key = "c_axis_misalignment_list_array_name";
  static inline constexpr StringLiteral k_AvgCAxisMisalignmentsArrayName_Key = "avg_caxis_misalignments_array_name";

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

COMPLEX_DEF_FILTER_TRAITS(complex, FindFeatureNeighborCAxisMisalignments, "636ee030-9f07-4f16-a4f3-592eff8ef1ee");
