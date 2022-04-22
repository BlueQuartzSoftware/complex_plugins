#------------------------------------------------------------------------------
# Required EbsdLib and H5Support
#------------------------------------------------------------------------------
find_package(H5Support REQUIRED)
find_package(EbsdLib REQUIRED)


#------------------------------------------------------------------------------
# Include the standard complex plugin cmake support codes
#------------------------------------------------------------------------------
include("${complex_SOURCE_DIR}/cmake/Plugin.cmake")

set(PLUGIN_NAME "OrientationAnalysis")
#------------------------------------------------------------------------------
# These are all the filters in the plugin. All filters should be kept in the
# OrientationAnalysis/src/OrientationAnalysis/Filters/ directory.
set(FilterList
  GenerateFZQuaternions
  ConvertOrientations
  ReadH5Ebsd
  )
  
set(STUB_FILTERS
  BadDataNeighborOrientationCheck
  ConvertQuaternion
  #ConvertHexGridToSquareGrid # MISSING 1 or more Parameter Implementations
  #CreateEnsembleInfo # MISSING 1 or more Parameter Implementations
  CreateLambertSphere
  EMsoftSO3Sampler
  #EbsdToH5Ebsd # MISSING 1 or more Parameter Implementations
  EnsembleInfoReader
  FindAvgCAxes
  FindAvgOrientations
  FindBoundaryStrengths
  FindCAxisLocations
  FindDistsToCharactGBs
  FindFeatureNeighborCAxisMisalignments
  FindFeatureReferenceCAxisMisorientations
  FindFeatureReferenceMisorientations
  FindGBCD
  FindGBCDMetricBased
  FindGBPDMetricBased
  FindKernelAvgMisorientations
  FindMisorientations
  FindSchmids
  FindSlipTransmissionMetrics
  FindTwinBoundaries
  FindTwinBoundarySchmidFactors
  GenerateFaceIPFColoring
  GenerateFaceMisorientationColoring
  GenerateIPFColors
  GenerateOrientationMatrixTranspose
  GenerateQuaternionConjugate
  INLWriter
  #ImportEbsdMontage # MISSING 1 or more Parameter Implementations
  #ImportH5EspritData # MISSING 1 or more Parameter Implementations
  #ImportH5OimData # MISSING 1 or more Parameter Implementations
  NeighborOrientationCorrelation
  #OrientationUtility # MISSING 1 or more Parameter Implementations
  ReadAngData
  ReadCtfData
  ReplaceElementAttributesWithNeighborValues
  RodriguesConvertor
  RotateEulerRefFrame
  Stereographic3D
  WritePoleFigure
  WriteStatsGenOdfAngleFile
)

set(Parameter_List
  H5EbsdReaderParameter
)

create_complex_plugin(NAME ${PLUGIN_NAME}
                      FILTER_LIST ${FilterList}
                      DESCRIPTION "Plugin for ${PLUGIN_NAME}"
                      VERSION "0.2.0"
)


#------------------------------------------------------------------------------
# Add in all the parameters.
# This should be integrated with the `create_complex_plugin` function call
#------------------------------------------------------------------------------
foreach(source ${Parameter_List})
  target_sources(${PLUGIN_NAME} PRIVATE
    ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Parameters/H5EbsdReaderParameter.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/Parameters/H5EbsdReaderParameter.cpp)
endforeach()


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
# If there are additional libraries that this plugin needs to link against you
# can use the target_link_libraries() cmake call
#------------------------------------------------------------------------------
target_link_libraries(${PLUGIN_NAME} PUBLIC EbsdLib::EbsdLib NXComponents)


#------------------------------------------------------------------------------
# If there are additional include directories that are needed for this plugin
# you can use the target_include_directories(.....) cmake call
target_include_directories(${PLUGIN_NAME}
  PUBLIC
  ${EbsdLib_INCLUDE_DIRS}
  ${${PLUGIN_NAME}_BINARY_DIR}
  )

#------------------------------------------------------------------------------
# If this plugin is also creating UI parameter widgets, then we can optionally
# compile those sources
#------------------------------------------------------------------------------
if(ENABLE_PLUGIN_GUI)
  #------------------------------------------------------------------------------
  # If there are additional library packages that need to be found, this is where
  # to do that using the usual find_package(....) call
  #------------------------------------------------------------------------------
  find_package(Qt5 COMPONENTS Core Widgets)

  target_link_libraries(${PLUGIN_NAME} PUBLIC  Qt::Core Qt::Widgets)

  QT5_WRAP_CPP(moc_files  ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/ui/Parameters/NXH5EbsdReaderParameter.h
                          ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/ui/Widgets/NXH5EbsdReaderWidget.h
    )
  set_source_files_properties(${moc_files} PROPERTIES GENERATED TRUE)
  #set_source_files_properties(${moc_files} PROPERTIES HEADER_FILE_ONLY TRUE)

  QT5_WRAP_UI(ui_files ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/ui/Widgets/UI_Files/NXH5EbsdReaderWidget.ui)

  #------------------------------------------------------------------------------
  # If there are additional source files that need to be compiled for this plugin
  # can use the target_sources(...) cmake call
  target_sources(${PLUGIN_NAME}
    PRIVATE
    ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/ui/Parameters/NXH5EbsdReaderParameter.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/ui/Parameters/NXH5EbsdReaderParameter.cpp

    ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/ui/Widgets/NXH5EbsdReaderWidget.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/ui/Widgets/NXH5EbsdReaderWidget.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/ui/Widgets/UI_Files/NXH5EbsdReaderWidget.ui
    ${moc_files}
    ${ui_files}
  )

  target_include_directories(${PLUGIN_NAME}
    PUBLIC
      ${NXComponents_BINARY_DIR}
    )

endif()



#------------------------------------------------------------------------------
# Add in the unit tests for the filters included in this plugin
enable_testing()
add_subdirectory(${${PLUGIN_NAME}_SOURCE_DIR}/test)
