#pragma once

#include "SurfaceMeshing/SurfaceMeshing_export.hpp"

#include "complex/Filter/FilterTraits.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{
/**
 * @class FeatureFaceCurvatureFilter
 * @brief This filter will ....
 */
class SURFACEMESHING_EXPORT FeatureFaceCurvatureFilter : public IFilter
{
public:
  FeatureFaceCurvatureFilter() = default;
  ~FeatureFaceCurvatureFilter() noexcept override = default;

  FeatureFaceCurvatureFilter(const FeatureFaceCurvatureFilter&) = delete;
  FeatureFaceCurvatureFilter(FeatureFaceCurvatureFilter&&) noexcept = delete;

  FeatureFaceCurvatureFilter& operator=(const FeatureFaceCurvatureFilter&) = delete;
  FeatureFaceCurvatureFilter& operator=(FeatureFaceCurvatureFilter&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_NRing_Key = "NRing";
  static inline constexpr StringLiteral k_ComputePrincipalDirectionVectors_Key = "ComputePrincipalDirectionVectors";
  static inline constexpr StringLiteral k_ComputeGaussianCurvature_Key = "ComputeGaussianCurvature";
  static inline constexpr StringLiteral k_ComputeMeanCurvature_Key = "ComputeMeanCurvature";
  static inline constexpr StringLiteral k_UseNormalsForCurveFitting_Key = "UseNormalsForCurveFitting";
  static inline constexpr StringLiteral k_FaceAttributeMatrixPath_Key = "FaceAttributeMatrixPath";
  static inline constexpr StringLiteral k_SurfaceMeshFaceLabelsArrayPath_Key = "SurfaceMeshFaceLabelsArrayPath";
  static inline constexpr StringLiteral k_SurfaceMeshFeatureFaceIdsArrayPath_Key = "SurfaceMeshFeatureFaceIdsArrayPath";
  static inline constexpr StringLiteral k_SurfaceMeshFaceNormalsArrayPath_Key = "SurfaceMeshFaceNormalsArrayPath";
  static inline constexpr StringLiteral k_SurfaceMeshTriangleCentroidsArrayPath_Key = "SurfaceMeshTriangleCentroidsArrayPath";
  static inline constexpr StringLiteral k_SurfaceMeshPrincipalCurvature1sArrayName_Key = "SurfaceMeshPrincipalCurvature1sArrayName";
  static inline constexpr StringLiteral k_SurfaceMeshPrincipalCurvature2sArrayName_Key = "SurfaceMeshPrincipalCurvature2sArrayName";
  static inline constexpr StringLiteral k_SurfaceMeshPrincipalDirection1sArrayName_Key = "SurfaceMeshPrincipalDirection1sArrayName";
  static inline constexpr StringLiteral k_SurfaceMeshPrincipalDirection2sArrayName_Key = "SurfaceMeshPrincipalDirection2sArrayName";
  static inline constexpr StringLiteral k_SurfaceMeshGaussianCurvaturesArrayName_Key = "SurfaceMeshGaussianCurvaturesArrayName";
  static inline constexpr StringLiteral k_SurfaceMeshMeanCurvaturesArrayName_Key = "SurfaceMeshMeanCurvaturesArrayName";

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

COMPLEX_DEF_FILTER_TRAITS(complex, FeatureFaceCurvatureFilter, "a69d8d43-5be9-59a0-b997-81773a635673");
