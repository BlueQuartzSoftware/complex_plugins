#pragma once

#include "ITKImageProcessing/ITKImageProcessing_export.hpp"

#include "complex/Filter/FilterTraits.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{
/**
 * @class ITKMaximumProjectionImage
 * @brief Maximum projection.
 *
 * This class was contributed to the insight journal by Gaetan Lehmann. The original paper can be found at https://hdl.handle.net/1926/164
 *
 * @author Gaetan Lehmann. Biologie du Developpement et de la reproduction, inra de jouy-en-josas, France.
 *
 *
 * @see ProjectionImageFilter
 *
 *
 * @see MedianProjectionImageFilter
 *
 *
 * @see MeanProjectionImageFilter
 *
 *
 * @see MinimumProjectionImageFilter
 *
 *
 * @see StandardDeviationProjectionImageFilter
 *
 *
 * @see SumProjectionImageFilter
 *
 *
 * @see BinaryProjectionImageFilter
 *
 * ITK Module: ITKImageStatistics
 * ITK Group: ImageStatistics
 */
class ITKIMAGEPROCESSING_EXPORT ITKMaximumProjectionImage : public IFilter
{
public:
  ITKMaximumProjectionImage() = default;
  ~ITKMaximumProjectionImage() noexcept override = default;

  ITKMaximumProjectionImage(const ITKMaximumProjectionImage&) = delete;
  ITKMaximumProjectionImage(ITKMaximumProjectionImage&&) noexcept = delete;

  ITKMaximumProjectionImage& operator=(const ITKMaximumProjectionImage&) = delete;
  ITKMaximumProjectionImage& operator=(ITKMaximumProjectionImage&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_SelectedImageGeomPath_Key = "SelectedImageGeomPath";
  static inline constexpr StringLiteral k_SelectedImageDataPath_Key = "InputImageDataPath";
  static inline constexpr StringLiteral k_OutputImageDataPath_Key = "OutputImageDataPath";
  static inline constexpr StringLiteral k_ProjectionDimension_Key = "ProjectionDimension";

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
   * @param shouldCancel Boolean that gets set if the filter should stop executing and return
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  PreflightResult preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;

  /**
   * @brief Applies the filter's algorithm to the DataStructure with the given arguments. Returns any warnings/errors.
   * On failure, there is no guarantee that the DataStructure is in a correct state.
   * @param ds The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @param shouldCancel Boolean that gets set if the filter should stop executing and return
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  Result<> executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;
};
} // namespace complex

COMPLEX_DEF_FILTER_TRAITS(complex, ITKMaximumProjectionImage, "b2cb7ad7-6f62-51c4-943d-54d19c64e7be");
