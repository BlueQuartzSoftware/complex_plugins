#------------------------------------------------------------------------------
# If there are additional library packages that need to be found, this is where
# to do that using the usual find_package(....) call
# find_package(....)

#------------------------------------------------------------------------------
# Define the list of unit test source files
set(${PLUGIN_NAME}UnitTest_SRCS
  AlignSectionsMisorientationTest.cpp
  BadDataNeighborOrientationCheckTest.cpp
  ConvertOrientationsTest.cpp
  EBSDSegmentFeaturesFilterTest.cpp   # NEEDS TEST IMPLEMENTED
  FindAvgOrientationsTest.cpp
  GenerateFZQuaternionsTest.cpp
  GenerateIPFColorsTest.cpp
  NeighborOrientationCorrelationTest.cpp
  RotateEulerRefFrameTest.cpp
  MergeTwinsTest.cpp
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
  FindFeatureReferenceMisorientationsTest.cpp
  FindGBCDTest.cpp
  FindGBCDMetricBasedTest.cpp
  FindGBPDMetricBasedTest.cpp
  FindKernelAvgMisorientationsTest.cpp
  FindMisorientationsTest.cpp
  FindSchmidsTest.cpp
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
  ReadAngDataTest.cpp
  ReadCtfDataTest.cpp

  # ReadH5EbsdTest.cpp    # MISSING 1 or more Parameter Implementations
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
# target_include_directories(${PLUGIN_NAME}UnitTest
# PUBLIC
# additional include directories here
# )
