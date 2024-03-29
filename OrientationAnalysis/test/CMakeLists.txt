# ------------------------------------------------------------------------------
# If there are additional library packages that need to be found, this is where
# to do that using the usual find_package(....) call
# find_package(....)

# ------------------------------------------------------------------------------
# Define the list of unit test source files
set(${PLUGIN_NAME}UnitTest_SRCS
  AlignSectionsMisorientationTest.cpp
  AlignSectionsMutualInformationTest.cpp
  BadDataNeighborOrientationCheckTest.cpp
  ConvertQuaternionTest.cpp
  ConvertOrientationsTest.cpp
  CreateEnsembleInfoTest.cpp
  EBSDSegmentFeaturesFilterTest.cpp
  EbsdToH5EbsdTest.cpp
  EnsembleInfoReaderTest.cpp
  ExportGBCDGMTFileTest.cpp
  ExportGBCDTriangleDataTest.cpp
  FindAvgOrientationsTest.cpp
  FindFeatureReferenceMisorientationsTest.cpp
  FindGBCDTest.cpp
  FindKernelAvgMisorientationsTest.cpp
  FindMisorientationsTest.cpp
  FindSchmidsTest.cpp
  FindShapesFilterTest.cpp
  FindTriangleGeomShapesTest.cpp
  GenerateFaceIPFColoringTest.cpp
  GenerateFaceMisorientationColoringTest.cpp
  GenerateFZQuaternionsTest.cpp
  GenerateGBCDPoleFigureTest.cpp
  GenerateIPFColorsTest.cpp
  GenerateQuaternionConjugateTest.cpp
  MergeTwinsTest.cpp
  NeighborOrientationCorrelationTest.cpp
  ReadAngDataTest.cpp
  ReadCtfDataTest.cpp
  ReadH5EbsdTest.cpp
  RotateEulerRefFrameTest.cpp
  RodriguesConvertorTest.cpp
)
set(DISABLED_TESTS

  # ConvertHexGridToSquareGridTest.cpp    # MISSING 1 or more Parameter Implementations

  CreateLambertSphereTest.cpp
  EMsoftSO3SamplerTest.cpp

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
  GenerateOrientationMatrixTransposeTest.cpp
  INLWriterTest.cpp

  # ImportEbsdMontageTest.cpp    # MISSING 1 or more Parameter Implementations
  # ImportH5EspritDataTest.cpp    # MISSING 1 or more Parameter Implementations
  # ImportH5OimDataTest.cpp    # MISSING 1 or more Parameter Implementations
  # OrientationUtilityTest.cpp    # MISSING 1 or more Parameter Implementations

  ReplaceElementAttributesWithNeighborValuesTest.cpp
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

  download_test_data(DREAM3D_DATA_DIR ${DREAM3D_DATA_DIR} ARCHIVE_NAME 6_6_Small_IN100_GBCD.tar.gz SHA512 543e3bdcee24ff9e5cd80dfdedc39ceef1529e7172cebd01d8e5518292ffdf4e0eb2e79d75854cb3eaca5c60e19c861ca67f369e21b81c306edb66327f47a1e3)
  download_test_data(DREAM3D_DATA_DIR ${DREAM3D_DATA_DIR} ARCHIVE_NAME 6_5_align_sections_mutual_information.tar.gz SHA512 363c52bf3b7677e06508fdb92350457d37bbd58ab2d26259e61ec9317f9146b85ec50c58151f69a0b38fc487f79af2896c8fef4e98d1e87c29af1cf19cbf884b)
  download_test_data(DREAM3D_DATA_DIR ${DREAM3D_DATA_DIR} ARCHIVE_NAME 6_6_stats_test.tar.gz SHA512 cb1d1c004ab5a3cb29cc10f7b6c291dd5819e57e303242b8162cd0b268dea24b1d3e5e3811ec4f5ee216179b6eb6b81c76ee7d37b7c49e83c6ab336147b4b14e)
  download_test_data(DREAM3D_DATA_DIR ${DREAM3D_DATA_DIR} ARCHIVE_NAME Small_IN100_Ang_Files.tar.gz SHA512 79e9f6948d4e8e06187e11216a67596fa786ffd2700e51f594ad014090383eb8bcc003e14de2e88082aa9ae512cc4fc9cee22c80066fc54f38c3ebc75267eb5b)
  download_test_data(DREAM3D_DATA_DIR ${DREAM3D_DATA_DIR} ARCHIVE_NAME 6_5_h5ebsd_exemplar.tar.gz SHA512 9e5936b6e8e6fd51ecc7c17521ff29898bb0e8bc3d516741c47ae65577b4f2e0f3df8f485f19ecf93f1848b70aeeb98b91c2c5742965107acabeaa40182d898d)

endif()

# -----------------------------------------------------------------------------
# Include any example pipelines into the unit testing
# -----------------------------------------------------------------------------
include(${complex_SOURCE_DIR}/cmake/Utility.cmake)

# These should be relative to the 
set(PREBUILT_PIPELINE_NAMES
  # THIS GROUP MUST GO FIRST because the .h5ebsd file is needed for some of the examples below
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/EBSD Reconstruction/(01) Small IN100 Archive.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/EBSD Reconstruction/(08) Small IN100 Full Reconstruction.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/EBSD Statistics/(01) Small IN100 Morphological Statistics.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/EBSD Statistics/(05) Small IN100 Crystallographic Statistics.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/EBSD SurfaceMeshing/(01) Small IN100 Quick Mesh.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/EBSD SurfaceMeshing/(02) Small IN100 Smooth Mesh.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/EBSD SurfaceMeshing/(03) Small IN100 Mesh Statistics.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/EBSD SurfaceMeshing/(04) Small IN100 GBCD.d3dpipeline"

  # Example or other test pipelines
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/AlignSectionsMutualInformation.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/CI_Histogram.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/Edax_IPF_Colors.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/EnembleInfoReader.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/ReadAng.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/ReadCTF.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/TxCopper_Exposed.d3dpipeline"
  "${${PLUGIN_NAME}_SOURCE_DIR}/pipelines/TxCopper_Unexposed.d3dpipeline"

)

create_pipeline_tests(PLUGIN_NAME ${PLUGIN_NAME} PIPELINE_LIST ${PREBUILT_PIPELINE_NAMES})
