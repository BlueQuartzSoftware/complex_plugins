include("${complex_SOURCE_DIR}/cmake/Plugin.cmake")

set(PLUGIN_NAME "SIMPL")
#------------------------------------------------------------------------------
# These are all the filters in the plugin. All filters should be kept in the
# SIMPL/src/SIMPL/Filters/ directory.
set(FilterList

)

set(STUB_FILTERS
  #ArrayCalculator # MISSING 1 or more Parameter Implementations
  CombineAttributeArrays
  CombineAttributeMatrices
  ConvertColorToGrayScale
  ConvertData
  CopyFeatureArrayToElementArray
  CopyObject
  #CreateAttributeMatrix # MISSING 1 or more Parameter Implementations
  #CreateDataArray # MISSING 1 or more Parameter Implementations
  CreateDataContainer
  CreateFeatureArrayFromElementArray
  CreateGeometry
  CreateGridMontage
  CreateStringArray
  #DataContainerReader # MISSING 1 or more Parameter Implementations
  ExecuteProcess
  ExtractAttributeArraysFromGeometry
  ExtractComponentAsArray
  ExtractVertexGeometry
  FeatureCountDecision
  FeatureDataCSVWriter
  FindDerivatives
  #GenerateColorTable # MISSING 1 or more Parameter Implementations
  GenerateTiltSeries
  GenerateVertexCoordinates
  ImportAsciDataArray
  #ImportHDF5Dataset # MISSING 1 or more Parameter Implementations
  #InitializeData # MISSING 1 or more Parameter Implementations
  LinkFeatureMapToElementArray
  MaskCountDecision
  MoveData
  #MoveMultiData # MISSING 1 or more Parameter Implementations
  #MultiThresholdObjects # MISSING 1 or more Parameter Implementations
  #MultiThresholdObjects2 # MISSING 1 or more Parameter Implementations
  #PipelineAnnotation # MISSING 1 or more Parameter Implementations
  PostSlackMessage
  #ReadASCIIData # MISSING 1 or more Parameter Implementations
  #RemoveArrays # MISSING 1 or more Parameter Implementations
  RemoveComponentFromArray
  RenameAttributeArray
  RenameAttributeMatrix
  RenameDataContainer
  ReplaceValueInArray
  RequiredZThickness
  #RotateSampleRefFrame # MISSING 1 or more Parameter Implementations
  ScaleVolume
  SplitAttributeArray
  WriteASCIIData
  WriteTriangleGeometry

)

create_complex_plugin(NAME ${PLUGIN_NAME}
                      FILTER_LIST ${FilterList}
                      DESCRIPTION "Plugin for ${PLUGIN_NAME}"
                      VERSION "0.1.0"
)

#------------------------------------------------------------------------------
# If there are additional library packages that need to be found, this is where
# to do that using the usual find_package(....) call
# find_package(....)


#------------------------------------------------------------------------------
# If there are additional libraries that this plugin needs to link against you
# can use the target_link_libraries() cmake call
# target_link_libraries(${PLUGIN_NAME} PUBLIC [name of library])

#------------------------------------------------------------------------------
# If there are additional source files that need to be compiled for this plugin
# can use the target_sources(...) cmake call
# target_sources(${PLUGIN_NAME}
#   PUBLIC
#     additional header files
#   PRIVATE
#     additional source files
# )

#------------------------------------------------------------------------------
# Include filters that need to be compiled but not available to the general user
# These are typically auto-generated sources that need to be compiled to ensure
# the stubs are following the latest API changes
#------------------------------------------------------------------------------
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

#------------------------------------------------------------------------------
# If there are additional include directories that are needed for this plugin
# you can use the target_include_directories(.....) cmake call
# target_include_directories(${PLUGIN_NAME}
#     PUBLIC
#     additional include directories here
# )

#------------------------------------------------------------------------------
# Add in the unit tests for the filters included in this plugin
enable_testing()
add_subdirectory(${${PLUGIN_NAME}_SOURCE_DIR}/test)
