
set(ItkComponents

  # ITKClassifiers
  ITKCommon
  ITKConvolution
  ITKDenoising

  # ITKImageCompare
  ITKImageFeature
  ITKImageIntensity

  # ITKImageNoise
  ITKIOBioRad
  ITKIOBMP
  ITKIOGE
  ITKIOGIPL
  ITKIOImageBase
  ITKIOIPL
  ITKIOJPEG
  ITKIOMeta
  ITKIOMRC
  ITKIONIFTI
  ITKIONRRD
  ITKIOPNG
  ITKIOStimulate
  ITKIOTIFF
  ITKIOVTK
  ITKMathematicalMorphology
  ITKOptimizers

  # ITKRegistrationCommon
  ITKReview
  ITKSmoothing
  ITKStatistics
  ITKTestKernel
  ITKTransform
  ITKWatersheds
)

if(ITK_VERSION_MAJOR EQUAL 5 AND ITK_VERSION_MINOR EQUAL 2)
  list(APPEND ItkComponents ITKMathematicalMorphology)
endif()

find_package(ITK COMPONENTS ${ItkComponents} REQUIRED)

get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(NOT GENERATOR_IS_MULTI_CONFIG)
  string(TOUPPER "${CMAKE_BUILD_TYPE}" TYPE_UPPER)
  get_target_property(ITKCOMMON_PATH ITKCommon IMPORTED_LOCATION_${TYPE_UPPER})
  get_filename_component(ITK_LIB_DIR ${ITKCOMMON_PATH} DIRECTORY CACHE)

  get_property(COMPLEX_EXTRA_LIBRARY_DIRS GLOBAL PROPERTY COMPLEX_EXTRA_LIBRARY_DIRS)
  set_property(GLOBAL PROPERTY COMPLEX_EXTRA_LIBRARY_DIRS ${COMPLEX_EXTRA_LIBRARY_DIRS} ${ITK_LIB_DIR})
endif()

include("${complex_SOURCE_DIR}/cmake/Plugin.cmake")

set(PLUGIN_NAME "ITKImageProcessing")
set(${PLUGIN_NAME}_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

# ------------------------------------------------------------------------------
# These are all the filters in the plugin. All filters should be kept in the
# ITKImageProcessing/src/ITKImageProcessing/Filters/ directory.
set(FilterList
  ITKImageReader
  ITKImageWriter
  ITKImportImageStack

  ITKAbsImage
  ITKAcosImage
  ITKAsinImage
  ITKAtanImage

  # ITKBilateralImage
  # ITKBinaryClosingByReconstructionImage
  ITKBinaryContourImage

  # ITKBinaryDilateImage
  # ITKBinaryErodeImage
  ITKBinaryMorphologicalClosingImage
  # ITKBinaryMorphologicalOpeningImage
  # ITKBinaryOpeningByReconstructionImage
  # ITKBinaryProjectionImage
  # ITKBinaryThinningImage
  # ITKBinomialBlurImage
  # ITKBlackTopHatImage
  ITKClosingByReconstructionImage
  ITKCosImage

  # ITKDilateObjectMorphologyImage
  # ITKErodeObjectMorphologyImage
  # ITKExpImage
  # ITKExpNegativeImage
  # ITKGrayscaleDilateImage
  # ITKGrayscaleErodeImage
  ITKGrayscaleFillholeImage

  # ITKGrayscaleGrindPeakImage
  # ITKGrayscaleMorphologicalClosingImage
  # ITKGrayscaleMorphologicalOpeningImage
  # ITKHConvexImage
  # ITKHMaximaImage
  # ITKHMinimaImage
  # ITKIntensityWindowingImage
  ITKInvertIntensityImage

  # ITKLabelContourImage
  # ITKLaplacianSharpeningImage
  ITKLog10Image
  ITKLogImage

  # ITKMaximumProjectionImage
  # ITKMedianProjectionImage
  # ITKMinimumProjectionImage
  # ITKMorphologicalGradientImage
  ITKMorphologicalWatershedImage
  # ITKMultiScaleHessianBasedObjectnessImage
  # ITKNotImage
  ITKOpeningByReconstructionImage

  # ITKRelabelComponentImage
  # ITKSaltAndPepperNoiseImage
  # ITKShiftScaleImage
  # ITKShotNoiseImage
  # ITKSigmoidImage
  ITKSinImage

  # ITKSpeckleNoiseImage
  ITKSqrtImage

  # ITKSquareImage
  ITKTanImage

  # ITKThresholdImage
  # ITKValuedRegionalMaximaImage
  # ITKValuedRegionalMinimaImage
  # ITKWhiteTopHatImage

  # -----------------------------------------------------------------------------
  # These filters have a fixed output type
  # -----------------------------------------------------------------------------
  # ITKApproximateSignedDistanceMapImage
  # ITKMinMaxCurvatureFlowImage
  # ITKGradientAnisotropicDiffusionImage
  # ITKCurvatureAnisotropicDiffusionImage
  ITKBinaryThresholdImage

  # ITKBoundedReciprocalImage
  # ITKConnectedComponentImage
  # ITKCurvatureFlowImage
  # ITKDanielssonDistanceMapImage
  # ITKDoubleThresholdImage
  ITKGradientMagnitudeImage

  # ITKGradientMagnitudeRecursiveGaussianImage
  # ITKIsoContourDistanceImage
  # ITKLaplacianRecursiveGaussianImage
  # ITKMeanProjectionImage
  ITKNormalizeImage

  # ITKNormalizeToConstantImage
  ITKOtsuMultipleThresholdsImage

  # ITKRegionalMaximaImage
  # ITKRegionalMinimaImage
  # ITKStandardDeviationProjectionImage
  # ITKSumProjectionImage
  # ITKThresholdMaximumConnectedComponentsImage
  # ITKZeroCrossingImage
  # ITKMorphologicalWatershedFromMarkersImage
  # ITKSignedDanielssonDistanceMapImage
  ITKSignedMaurerDistanceMapImage

  # ITKSmoothingRecursiveGaussianImage

  # -----------------------------------------------------------------------------
  # There are parameters from the old version that do not appear in the JSON when generating.
  # -----------------------------------------------------------------------------
  # ITKRescaleIntensityImage
  # ITKBoxMeanImage
  ITKAdaptiveHistogramEqualizationImage
  ITKMedianImage

  # ITKDiscreteGaussianImage
  ITKMaskImage
)

# -----------------------------------------------------------------------------
# These are filters that need to be compiled but not exposed to a user
# -----------------------------------------------------------------------------
set(STUB_FILTERS
)

set(Filters_Disabled

  # -----------------------------------------------------------------------------
  # These filters are custom written filters by BQ and do not conform to the
  # standard ITK filter wrapping. These Filters are not even compiling
  # -----------------------------------------------------------------------------
  IlluminationCorrection # MISSING 1 or more Parameter Implementations
  ImportAxioVisionV4Montage
  AxioVisionV4ToTileConfiguration
  ImportZenInfoMontage
  ApplyDewarpParameters # MISSING 1 or more Parameter Implementations
  CalcDewarpParameters # MISSING 1 or more Parameter Implementations
  ImportVectorImageStack # MISSING 1 or more Parameter Implementations
  ITKRefineTileCoordinates # MISSING 1 or more Parameter Implementations
  ITKImportFijiMontage
  ITKImportRoboMetMontage
  ITKPCMTileRegistration
  ITKProxTVImage
  ITKStitchMontage # MISSING 1 or more Parameter Implementations
  ITKRGBToLuminanceImage # Multiple Different Return types so auto has issues with that

  # -----------------------------------------------------------------------------
  # These filters have a fixed output type
  # -----------------------------------------------------------------------------
  ITKVectorConnectedComponentImage # Vector Input, uint32_t output

  # -----------------------------------------------------------------------------
  # These filters use the itk::InPlaceDream3DDataToImageFilter filter
  # -----------------------------------------------------------------------------
  ITKPatchBasedDenoisingImage # itk::InPlaceDream3DDataToImageFilter
  ITKFFTNormalizedCorrelationImage # itk::InPlaceDream3DDataToImageFilter
  ITKHistogramMatchingImage # itk::InPlaceDream3DDataToImageFilter

  # -----------------------------------------------------------------------------
  # These filters use the Dream3DArraySwitchOutputComponentMacro macro
  # -----------------------------------------------------------------------------
  ITKCastImage # Dream3DArraySwitchOutputComponentMacro
  ITKVectorRescaleIntensityImage # Dream3DArraySwitchOutputComponentMacro
)

create_complex_plugin(NAME ${PLUGIN_NAME}
  FILTER_LIST ${FilterList}
  DESCRIPTION "${PLUGIN_NAME} Plugin"
  VERSION "0.1.0"
  DOC_CHECK
  ADD_TO_GLOBAL_LIST
)

target_link_libraries(${PLUGIN_NAME} PUBLIC ${ItkComponents})

if(MSVC)
  target_compile_options(${PLUGIN_NAME}
    PUBLIC
    /bigobj
  )
endif()

target_sources(${PLUGIN_NAME}
  PRIVATE
  ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Common/ITKArrayHelper.hpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Common/ITKArrayHelper.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Common/ITKProgressObserver.hpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Common/ITKDream3DFilterInterruption.hpp
)

target_include_directories(${PLUGIN_NAME} PUBLIC ${ITK_INCLUDE_DIRS})

if(WIN32)
  # This removes some C++17 Deprecation Warnings inside of ITK 5.1
  target_compile_definitions(${PLUGIN_NAME} PUBLIC "_SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING")
  set_target_properties(${PLUGIN_NAME} PROPERTIES LINK_FLAGS_DEBUG "/INCREMENTAL:NO")
endif()

# ------------------------------------------------------------------------------
# Include filters that need to be compiled but not available to the general user
# These are typically auto-generated sources that need to be compiled to ensure
# the stubs are following the latest API changes
# ------------------------------------------------------------------------------
foreach(file_name ${STUB_FILTERS})
  list(APPEND hidden_filter_HDRS
    "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Filters/${file_name}.hpp"
  )
  list(APPEND hidden_filter_SRCS
    "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Filters/${file_name}.cpp"
  )
endforeach()

target_sources(${PLUGIN_NAME}
  PRIVATE ${hidden_filter_HDRS} ${hidden_filter_SRCS}
)
source_group(TREE "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}" PREFIX ${PLUGIN_NAME} FILES ${hidden_filter_HDRS} ${hidden_filter_SRCS})

# ------------------------------------------------------------------------------
# Add in the unit tests for the filters included in this plugin
enable_testing()
add_subdirectory(${${PLUGIN_NAME}_SOURCE_DIR}/test)

# ------------------------------------------------------------------------------
# Add in the install rules for ITK Libraries
# ------------------------------------------------------------------------------
get_filename_component(ITKImageProcessing_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY CACHE)
get_filename_component(COMPLEX_PLUGINS_SOURCE_DIR ${ITKImageProcessing_SOURCE_DIR} DIRECTORY CACHE)

# message(STATUS "ITKImageProcessing_SOURCE_DIR: ${ITKImageProcessing_SOURCE_DIR}")
include(${COMPLEX_PLUGINS_SOURCE_DIR}/cmake/ITKInstall.cmake)
AddItkCopyInstallRules(LIBS ${ItkComponents})


# -----------------------------------------------------------------------
# Install example pipelines
# -----------------------------------------------------------------------
if(EXISTS "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines")
  set(PIPELINE_DEST_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/pipelines/${PLUGIN_NAME}")
  add_custom_target(${PLUGIN_NAME}PipelineFolderCopy ALL
                    COMMAND ${CMAKE_COMMAND} -E copy_directory "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines"
                    ${PIPELINE_DEST_DIR}
                    COMMENT "Copying Pipeline Folder into Binary Directory")
  set_target_properties(${PLUGIN_NAME}PipelineFolderCopy PROPERTIES FOLDER ZZ_COPY_FILES)

  set(INSTALL_DESTINATION "pipelines/${PLUGIN_NAME}")
  install(DIRECTORY
          "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/"
          DESTINATION "${INSTALL_DESTINATION}"
          COMPONENT Applications
        )
endif()

# -----------------------------------------------------------------------
# Download Example Data Files
# -----------------------------------------------------------------------
include(${complex_SOURCE_DIR}/cmake/Utility.cmake)
set(DATA_DEST_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/Data")

file(TO_CMAKE_PATH "${DREAM3D_DATA_DIR}" DREAM3D_DATA_DIR_NORM)

# -----------------------------------------------------------------------
# These data files are pulled from the 'complex data archive' on github,
# decompressed and then copied to the build folder. Install rules are also
# generated for each of the data sets.
# -----------------------------------------------------------------------
download_test_data(DREAM3D_DATA_DIR ${DREAM3D_DATA_DIR} ARCHIVE_NAME Porosity_Image.tar.gz SHA512 950c71b4f4721f14ea532ed5830b0786f49d2855b950bdf867c335b9fb0eb5dc28727b0899d665af0a2bc56816e1e66a08a008ef06a62b71efea4a6e46bff168)

set(DREAM3D_DATA_ARCHIVES "Porosity_Image")
foreach(archive ${DREAM3D_DATA_ARCHIVES})
  add_custom_target(Copy_${archive} ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DREAM3D_DATA_DIR}/TestFiles/${archive} ${DATA_DEST_DIR}/${archive}
    COMMENT "Copying ${archive} into Binary Directory"
    DEPENDS Fetch_${archive})
  set_target_properties(Copy_${archive} PROPERTIES FOLDER ZZ_COPY_FILES)

  install(DIRECTORY 
    ${DREAM3D_DATA_DIR_NORM}/TestFiles/${archive}
    DESTINATION Data/
    COMPONENT Applications
  )
endforeach()

# -----------------------------------------------------------------------
# Create build folder copy rules and install rules for the 'data' folder
# for this plugin.
# CURRENTLY DISABLED AS WE ARE NOT INSTALLING ANYTHING FROM DATA. IT IS
# USED FOR TESTING ONLY AND SERVES NO PURPOSE IN A DISTRIBUTION
# -----------------------------------------------------------------------
if(FALSE)
  add_custom_target(Copy_${PLUGIN_NAME}_Data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${${PLUGIN_NAME}_SOURCE_DIR}/data ${DATA_DEST_DIR}/${PLUGIN_NAME}
    COMMENT "Copying ${PLUGIN_NAME} data into Binary Directory"
    )
  set_target_properties(Copy_${PLUGIN_NAME}_Data PROPERTIES FOLDER ZZ_COPY_FILES)


  set(Installed_Data_Files
  )

  install(FILES 
    ${Installed_Data_Files}
    DESTINATION Data/${PLUGIN_NAME}
    COMPONENT Applications
  )


endif()
