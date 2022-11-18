# ------------------------------------------------------------------------------
# Required EbsdLib and H5Support
# ------------------------------------------------------------------------------
find_package(H5Support REQUIRED)
find_package(EbsdLib REQUIRED)

include("${complex_SOURCE_DIR}/cmake/Plugin.cmake")

set(PLUGIN_NAME "Core")
set(${PLUGIN_NAME}_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

# ------------------------------------------------------------------------------
# These are all the filters in the plugin. All filters should be kept in the
# Core/src/Core/Filters/ directory.
set(FilterList
  AlignSectionsFeatureCentroidFilter
  ArrayCalculatorFilter
  CreateGeometryFilter
  ConvertColorToGrayScaleFilter
  ConvertDataFilter
  FillBadDataFilter
  FindFeatureCentroidsFilter
  FindShapesFilter
  ResampleImageGeomFilter
  RotateSampleRefFrameFilter
  ErodeDilateBadDataFilter
  WriteASCIIDataFilter
  WriteBinaryDataFilter
  FindEuclideanDistMapFilter
  FindNeighborhoodsFilter
  FindSurfaceAreaToVolumeFilter
  CalculateArrayHistogramFilter
  RemoveFlaggedFeaturesFilter
  SplitAttributeArrayFilter
  FeatureDataCSVWriterFilter
)

set(STUB_FILTERS
)

create_complex_plugin(NAME ${PLUGIN_NAME}
  FILTER_LIST ${FilterList}
  DESCRIPTION "Plugin for ${PLUGIN_NAME}"
  VERSION "0.1.0"
  DOC_CHECK
  ADD_TO_GLOBAL_LIST
  )

# ------------------------------------------------------------------------------
# If there are additional library packages that need to be found, this is where
# to do that using the usual find_package(....) call
# find_package(....)

# ------------------------------------------------------------------------------
# If there are additional libraries that this plugin needs to link against you
# can use the target_link_libraries() cmake call
# target_link_libraries(${PLUGIN_NAME} PUBLIC [name of library])

# ------------------------------------------------------------------------------
# If there are additional source files that need to be compiled for this plugin
# can use the target_sources(...) cmake call
set(Utilities_SRCS
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CoreUtilities.hpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CalculatorItem.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CalculatorItem.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ICalculatorArray.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ICalculatorArray.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CalculatorArray.hpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CalculatorOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CalculatorOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/UnaryOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/UnaryOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/BinaryOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/BinaryOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ABSOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ABSOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/PowOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/PowOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ExpOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ExpOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/SinOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/SinOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CosOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CosOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/TanOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/TanOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ASinOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ASinOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ACosOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ACosOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ATanOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/ATanOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/SqrtOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/SqrtOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/RootOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/RootOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/LogOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/LogOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/Log10Operator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/Log10Operator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/LnOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/LnOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/AdditionOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/AdditionOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/SubtractionOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/SubtractionOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/NegativeOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/NegativeOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/MultiplicationOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/MultiplicationOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/DivisionOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/DivisionOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/LeftParenthesisItem.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/LeftParenthesisItem.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/RightParenthesisItem.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/RightParenthesisItem.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CalculatorSeparator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CalculatorSeparator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CommaSeparator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CommaSeparator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/FloorOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/FloorOperator.cpp"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CeilOperator.h"
	"${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Utilities/CeilOperator.cpp"
)

target_sources(${PLUGIN_NAME}
  PRIVATE ${Utilities_SRCS}
)
source_group(TREE "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}"
  PREFIX ${PLUGIN_NAME}
  FILES ${Utilities_SRCS}
)

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
# Algorithms
# This should be integrated with the `create_complex_plugin` function call
# ------------------------------------------------------------------------------
set(filter_algorithms
  AlignSectionsFeatureCentroid
  ArrayCalculator
  ConvertColorToGrayScale
  ConvertData
  FillBadData
  FindFeatureCentroids
  FindShapes
  ResampleImageGeom
  ErodeDilateBadData
  FindEuclideanDistMap
  FindNeighborhoods
  FindSurfaceAreaToVolume
  CalculateArrayHistogram
  RemoveFlaggedFeatures
  SplitAttributeArray
)


foreach(algorithm ${filter_algorithms})
  target_sources(${PLUGIN_NAME}
    PRIVATE "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Filters/Algorithms/${algorithm}.cpp"
    "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Filters/Algorithms/${algorithm}.hpp"
  )
  source_group(TREE "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}" PREFIX ${PLUGIN_NAME}
    FILES "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Filters/Algorithms/${algorithm}.cpp"
    "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Filters/Algorithms/${algorithm}.hpp"
  )
endforeach()


# ------------------------------------------------------------------------------
# If there are additional libraries that this plugin needs to link against you
# can use the target_link_libraries() cmake call
# ------------------------------------------------------------------------------
target_link_libraries(${PLUGIN_NAME} PUBLIC EbsdLib::EbsdLib)

# ------------------------------------------------------------------------------
# If there are additional include directories that are needed for this plugin
# you can use the target_include_directories(.....) cmake call
 target_include_directories(${PLUGIN_NAME}
  PUBLIC
    ${Eigen_INCLUDE_DIRS} ${EbsdLib_INCLUDE_DIRS}
 )

# ------------------------------------------------------------------------------
# Add in the unit tests for the filters included in this plugin
enable_testing()
add_subdirectory(${${PLUGIN_NAME}_SOURCE_DIR}/test)
