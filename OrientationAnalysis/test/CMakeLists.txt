# ------------------------------------------------------------------------------
# If there are additional library packages that need to be found, this is where
# to do that using the usual find_package(....) call
# find_package(....)

# ------------------------------------------------------------------------------
# Define the list of unit test source files
set(${PLUGIN_NAME}UnitTest_SRCS
  AlignSectionsMisorientationTest.cpp
  BadDataNeighborOrientationCheckTest.cpp
  ConvertOrientationsTest.cpp
  CreateEnsembleInfoTest.cpp
  EBSDSegmentFeaturesFilterTest.cpp
  EbsdToH5EbsdTest.cpp
  FindAvgOrientationsTest.cpp
  FindFeatureReferenceMisorientationsTest.cpp
  FindGBCDTest.cpp
  FindKernelAvgMisorientationsTest.cpp
  FindMisorientationsTest.cpp
  FindSchmidsTest.cpp
  FindShapesFilterTest.cpp
  FindTriangleGeomShapesTest.cpp
  GenerateFZQuaternionsTest.cpp
  GenerateIPFColorsTest.cpp
  MergeTwinsTest.cpp
  NeighborOrientationCorrelationTest.cpp
  ReadAngDataTest.cpp
  ReadCtfDataTest.cpp
  ReadH5EbsdTest.cpp
  RotateEulerRefFrameTest.cpp
  VisualizeGBCDPoleFigureTest.cpp
  VisualizeGBCDGMTTest.cpp
)
set(DISABLED_TESTS

  # ConvertHexGridToSquareGridTest.cpp    # MISSING 1 or more Parameter Implementations
  ConvertQuaternionTest.cpp

  # CreateEnsembleInfoTest.cpp    # MISSING 1 or more Parameter Implementations
  CreateLambertSphereTest.cpp
  EMsoftSO3SamplerTest.cpp

  # EbsdToH5EbsdTest.cpp    # MISSING 1 or more Parameter Implementations
  EnsembleInfoReaderTest.cpp
  FindAvgCAxesTest.cpp
  FindBoundaryStrengthsTest.cpp
  FindCAxisLocationsTest.cpp
  FindDistsToCharactGBsTest.cpp
  FindFeatureNeighborCAxisMisalignmentsTest.cpp
  FindFeatureReferenceCAxisMisorientationsTest.cpp
  FindGBCDMetricBasedTest.cpp
  FindGBPDMetricBasedTest.cpp
  FindSlipTransmissionMetricsTest.cpp
  FindTwinBoundariesTest.cpp
  FindTwinBoundarySchmidFactorsTest.cpp
  GenerateFaceIPFColoringTest.cpp
  GenerateFaceMisorientationColoringTest.cpp
  GenerateOrientationMatrixTransposeTest.cpp
  GenerateQuaternionConjugateTest.cpp
  INLWriterTest.cpp

  # ImportEbsdMontageTest.cpp    # MISSING 1 or more Parameter Implementations
  # ImportH5EspritDataTest.cpp    # MISSING 1 or more Parameter Implementations
  # ImportH5OimDataTest.cpp    # MISSING 1 or more Parameter Implementations
  # OrientationUtilityTest.cpp    # MISSING 1 or more Parameter Implementations

  ReplaceElementAttributesWithNeighborValuesTest.cpp
  RodriguesConvertorTest.cpp
  Stereographic3DTest.cpp
  WritePoleFigureTest.cpp
  WriteStatsGenOdfAngleFileTest.cpp
)

create_complex_plugin_unit_test(PLUGIN_NAME ${PLUGIN_NAME}
  FILTER_LIST ${${PLUGIN_NAME}UnitTest_SRCS})

# ------------------------------------------------------------------------------
# If there are additional libraries that this plugin needs to link against you
# can use the target_link_libraries() cmake call
# target_link_libraries(${PLUGIN_NAME}UnitTest PUBLIC [name of library])

# ------------------------------------------------------------------------------
# If there are additional source files that need to be compiled for this plugin
# can use the target_sources(...) cmake call
# target_sources(${PLUGIN_NAME}UnitTest
# PUBLIC
# additional header files
# PRIVATE
# additional source files
# )

# ------------------------------------------------------------------------------
# If there are additional include directories that are needed for this plugin
# you can use the target_include_directories(.....) cmake call
target_include_directories(${PLUGIN_NAME}UnitTest
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../../src
)


# -----------------------------------------------------------------------------
# Here we are going to setup to download and decompress the test files. In order
# to add your own test files you will need to tar.gz the test file, compute the
# SHA 512 Hash of the file and then upload the file to 
# https://github.com/BlueQuartzSoftware/complex/releases/tag/Data_Archive.
#
# Go to the web site above, "edit" the release, add the filename and SHA 512 to
# the table and then upload your compressed file.
# Save the release so that the repo is updated
# -----------------------------------------------------------------------------

option(COMPLEX_DOWNLOAD_TEST_FILES "Download the test files" ON)

if(EXISTS "${DREAM3D_DATA_DIR}" AND COMPLEX_DOWNLOAD_TEST_FILES) 
  if(NOT EXISTS ${DREAM3D_DATA_DIR}/TestFiles/)
    file(MAKE_DIRECTORY "${DREAM3D_DATA_DIR}/TestFiles/")
  endif()

  download_test_data(DREAM3D_DATA_DIR ${DREAM3D_DATA_DIR} ARCHIVE_NAME 6_6_Small_IN100_GBCD.tar.gz SHA512 7b3b0df01ad50f2d6547923fdc23d69a5de019e569478cf21991ad64784f3d2f57c33c794ea974d9329758d3cb8c84e9d66452243d0a5b412941572c8247bd42)

endif()
