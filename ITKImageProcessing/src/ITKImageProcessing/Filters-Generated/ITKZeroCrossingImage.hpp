#pragma once

#include "ITKImageProcessing/ITKImageProcessing_export.hpp"

#include "complex/Filter/FilterTraits.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{
/**
 * @class ITKZeroCrossingImage
 * @brief This filter finds the closest pixel to the zero-crossings (sign changes) in a signed itk::Image .
 *
 * Pixels closest to zero-crossings are labeled with a foreground value. All other pixels are marked with a background value. The algorithm works by detecting differences in sign among neighbors using
 * city-block style connectivity (4-neighbors in 2d, 6-neighbors in 3d, etc.).
 *
 * \par Inputs and Outputs
 * The input to this filter is an itk::Image of arbitrary dimension. The algorithm assumes a signed data type (zero-crossings are not defined for unsigned data types), and requires that operator>,
 * operator<, operator==, and operator!= are defined.
 *
 *
 * \par
 * The output of the filter is a binary, labeled image of user-specified type. By default, zero-crossing pixels are labeled with a default "foreground" value of
 * itk::NumericTraits<OutputDataType>::OneValue() , where OutputDataType is the data type of the output image. All other pixels are labeled with a default "background" value of
 * itk::NumericTraits<OutputDataType>::ZeroValue() .
 *
 *
 * \par Parameters
 * There are two parameters for this filter. ForegroundValue is the value that marks zero-crossing pixels. The BackgroundValue is the value given to all other pixels.
 *
 *
 * @see Image
 *
 *
 * @see Neighborhood
 *
 *
 * @see NeighborhoodOperator
 *
 *
 * @see NeighborhoodIterator
 *
 * ITK Module: ITKImageFeature
 * ITK Group: ImageFeature
 */
class ITKIMAGEPROCESSING_EXPORT ITKZeroCrossingImage : public IFilter
{
public:
  ITKZeroCrossingImage() = default;
  ~ITKZeroCrossingImage() noexcept override = default;

  ITKZeroCrossingImage(const ITKZeroCrossingImage&) = delete;
  ITKZeroCrossingImage(ITKZeroCrossingImage&&) noexcept = delete;

  ITKZeroCrossingImage& operator=(const ITKZeroCrossingImage&) = delete;
  ITKZeroCrossingImage& operator=(ITKZeroCrossingImage&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_SelectedImageGeomPath_Key = "SelectedImageGeomPath";
  static inline constexpr StringLiteral k_SelectedImageDataPath_Key = "InputImageDataPath";
  static inline constexpr StringLiteral k_OutputImageDataPath_Key = "OutputImageDataPath";
  static inline constexpr StringLiteral k_ForegroundValue_Key = "ForegroundValue";
  static inline constexpr StringLiteral k_BackgroundValue_Key = "BackgroundValue";

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

COMPLEX_DEF_FILTER_TRAITS(complex, ITKZeroCrossingImage, "0259fa1a-4706-5df1-8418-95ffc7b932dd");
