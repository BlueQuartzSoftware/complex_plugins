#pragma once

#include "ITKImageProcessing/ITKImageProcessing_export.hpp"

#include "complex/Filter/FilterTraits.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{
/**
 * @class ITKRegionalMinimaImage
 * @brief Produce a binary image where foreground is the regional minima of the input image.
 *
 * Regional minima are flat zones surrounded by pixels of greater value.
 *
 * If the input image is constant, the entire image can be considered as a minima or not. The SetFlatIsMinima() method let the user choose which behavior to use.
 *
 * This class was contributed to the Insight Journal by @author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France. https://hdl.handle.net/1926/153
 *
 *
 * @see RegionalMaximaImageFilter
 *
 *
 * @see ValuedRegionalMinimaImageFilter
 *
 *
 * @see HConcaveImageFilter
 *
 * ITK Module: ITKMathematicalMorphology
 * ITK Group: MathematicalMorphology
 */
class ITKIMAGEPROCESSING_EXPORT ITKRegionalMinimaImage : public IFilter
{
public:
  ITKRegionalMinimaImage() = default;
  ~ITKRegionalMinimaImage() noexcept override = default;

  ITKRegionalMinimaImage(const ITKRegionalMinimaImage&) = delete;
  ITKRegionalMinimaImage(ITKRegionalMinimaImage&&) noexcept = delete;

  ITKRegionalMinimaImage& operator=(const ITKRegionalMinimaImage&) = delete;
  ITKRegionalMinimaImage& operator=(ITKRegionalMinimaImage&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_SelectedImageGeomPath_Key = "selected_image_geom_path";
  static inline constexpr StringLiteral k_SelectedImageDataPath_Key = "input_image_data_path";
  static inline constexpr StringLiteral k_OutputImageDataPath_Key = "output_image_data_path";
  static inline constexpr StringLiteral k_BackgroundValue_Key = "background_value";
  static inline constexpr StringLiteral k_ForegroundValue_Key = "foreground_value";
  static inline constexpr StringLiteral k_FullyConnected_Key = "fully_connected";
  static inline constexpr StringLiteral k_FlatIsMinima_Key = "flat_is_minima";

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
   * @param dataGraph The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @param shouldCancel Boolean that gets set if the filter should stop executing and return
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  PreflightResult preflightImpl(const DataStructure& dataGraph, const Arguments& filterArgs, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;

  /**
   * @brief Applies the filter's algorithm to the DataStructure with the given arguments. Returns any warnings/errors.
   * On failure, there is no guarantee that the DataStructure is in a correct state.
   * @param dataGraph The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @param shouldCancel Boolean that gets set if the filter should stop executing and return
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  Result<> executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;
};
} // namespace complex

COMPLEX_DEF_FILTER_TRAITS(complex, ITKRegionalMinimaImage, "23611690-605e-4d8a-8090-f13d1879fa94");
