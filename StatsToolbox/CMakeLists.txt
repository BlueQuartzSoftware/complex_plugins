include("${complex_SOURCE_DIR}/cmake/Plugin.cmake")

set(PLUGIN_NAME "StatsToolbox")

# ------------------------------------------------------------------------------
# These are all the filters in the plugin. All filters should be kept in the
# StatsToolbox/src/StatsToolbox/Filters/ directory.
set(FilterList
)

set(STUB_FILTERS
  CalculateArrayHistogram
  ComputeMomentInvariants2D
  FindAvgScalarValueForFeatures
  FindBoundaryElementFractions
  FindDifferenceMap
  FindEuclideanDistMap
  FindFeatureClustering
  FindLargestCrossSections
  FindNeighborhoods
  FindNeighbors
  FindNumFeatures
  FindSurfaceAreaToVolume
  FindVolFractions
  FitFeatureData

  # GenerateEnsembleStatistics # MISSING 1 or more Parameter Implementations
)

create_complex_plugin(NAME ${PLUGIN_NAME}
  FILTER_LIST ${FilterList}
  DESCRIPTION "Plugin for ${PLUGIN_NAME}"
  VERSION "0.1.0"
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
# target_sources(${PLUGIN_NAME}
# PUBLIC
# additional header files
# PRIVATE
# additional source files
# )

# ------------------------------------------------------------------------------
# Include filters that need to be compiled but not available to the general user
# These are typically auto-generated sources that need to be compiled to ensure
# the stubs are following the latest API changes
# ------------------------------------------------------------------------------
foreach(file_name ${STUB_FILTERS})
  list(APPEND hidden_filter_HDRS
    "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Filters_Private/${file_name}.hpp"
  )
  list(APPEND hidden_filter_SRCS
    "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Filters_Private/${file_name}.cpp"
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
)
set(filter_algorithms_disabled
  CalculateArrayHistogram
  ComputeMomentInvariants2D
  FindAvgScalarValueForFeatures
  FindBoundaryElementFractions
  FindDifferenceMap
  FindEuclideanDistMap
  FindFeatureClustering
  FindLargestCrossSections
  FindNeighborhoods
  FindNeighbors
  FindNumFeatures
  FindShapes
  FindSurfaceAreaToVolume
  FindVolFractions
  FitFeatureData
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
# If there are additional include directories that are needed for this plugin
# you can use the target_include_directories(.....) cmake call
# target_include_directories(${PLUGIN_NAME}
# PUBLIC
# additional include directories here
# )

# ------------------------------------------------------------------------------
# Add in the unit tests for the filters included in this plugin
enable_testing()
add_subdirectory(${${PLUGIN_NAME}_SOURCE_DIR}/test)
