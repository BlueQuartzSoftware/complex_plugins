#include <catch2/catch.hpp>

#include "ITKImageProcessing/Common/sitkCommon.hpp"
#include "ITKImageProcessing/Filters/ITKExpNegativeImage.hpp"
#include "ITKImageProcessing/ITKImageProcessing_test_dirs.hpp"
#include "ITKTestBase.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"
#include "complex/Utilities/Parsing/HDF5/H5FileWriter.hpp"

#include <filesystem>

namespace fs = std::filesystem;

using namespace complex;

// Simply run with default settings
TEST_CASE("ITK::ExpNegativeImageFilter(defaults)", "[ITKImageProcessing][ITKExpNegativeImage][defaults]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  ITKExpNegativeImage filter;
  DataStructure ds;
  // Read the input image: Input/Ramp-Zero-One-Float.nrrd
  {
    Arguments args;
    fs::path filePath = fs::path(unit_test::k_SourceDir.view()) / complex::unit_test::k_DataDir.str() / "JSONFilters" / "Input/Ramp-Zero-One-Float.nrrd";
    DataPath inputGeometryPath({complex::ITKTestBase::k_ImageGeometryPath});
    DataPath inputDataPath = inputGeometryPath.createChildPath(complex::ITKTestBase::k_InputDataPath);
    int32_t result = complex::ITKTestBase::ReadImage(ds, filePath, inputGeometryPath, inputDataPath);
    REQUIRE(result == 0);
  } // End Scope Section

  // Test the filter itself
  {
    Arguments args;
    DataPath inputGeometryPath({complex::ITKTestBase::k_ImageGeometryPath});
    DataPath inputDataPath = inputGeometryPath.createChildPath(complex::ITKTestBase::k_InputDataPath);
    DataPath outputDataPath = inputGeometryPath.createChildPath(complex::ITKTestBase::k_OutputDataPath);
    args.insertOrAssign(ITKExpNegativeImage::k_SelectedImageGeomPath_Key, std::make_any<DataPath>(inputGeometryPath));
    args.insertOrAssign(ITKExpNegativeImage::k_SelectedImageDataPath_Key, std::make_any<DataPath>(inputDataPath));
    args.insertOrAssign(ITKExpNegativeImage::k_OutputIamgeDataPath_Key, std::make_any<DataPath>(outputDataPath));
    // Preflight the filter and check result
    auto preflightResult = filter.preflight(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);
    // Execute the filter and check the result
    auto executeResult = filter.execute(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  } // End Scope Section

  // Compare to baseline image
  {
    fs::path baselineFilePath = fs::path(unit_test::k_SourceDir.view()) / complex::unit_test::k_DataDir.str() / "JSONFilters/Baseline/BasicFilters_ExpNegativeImageFilter_defaults.nrrd";
    DataPath baselineGeometryPath({complex::ITKTestBase::k_BaselineGeometryPath});
    DataPath baselineDataPath = baselineGeometryPath.createChildPath(complex::ITKTestBase::k_BaselineDataPath);
    DataPath inputGeometryPath({complex::ITKTestBase::k_ImageGeometryPath});
    DataPath outputDataPath = inputGeometryPath.createChildPath(complex::ITKTestBase::k_OutputDataPath);
    int32_t error = complex::ITKTestBase::ReadImage(ds, baselineFilePath, baselineGeometryPath, baselineDataPath);
    REQUIRE(error == 0);
    Result<> result = complex::ITKTestBase::CompareImages(ds, baselineGeometryPath, baselineDataPath, inputGeometryPath, outputDataPath, 0.01);
    if(result.invalid())
    {
      for(const auto& err : result.errors())
      {
        std::cout << err.code << ": " << err.message << std::endl;
      }
    }
    REQUIRE(result.valid() == true);
  }

  // Write the output data to a file
  {
    fs::path filePath = fs::path(unit_test::k_BinaryDir.view()) / "test/BasicFilters_ExpNegativeImageFilter_defaults.nrrd";
    DataPath inputGeometryPath({complex::ITKTestBase::k_ImageGeometryPath});
    DataPath outputDataPath = inputGeometryPath.createChildPath(complex::ITKTestBase::k_OutputDataPath);
    int32_t error = complex::ITKTestBase::WriteImage(ds, filePath, inputGeometryPath, outputDataPath);
    // Remove *all* files generated by this test
    fs::path testDir = fs::path(unit_test::k_BinaryDir.view()) / "test";
    ITKTestBase::RemoveFiles(testDir, "BasicFilters_ExpNegativeImageFilter_defaults");
  } // End Scope Section
#if 0
  {
    fs::path filePath =fs::path( unit_test::k_BinaryDir.view()) / "test" / "ExpNegativeImageFilter_defaults.h5";
    Result<H5::FileWriter> result = H5::FileWriter::CreateFile(filePath);
    REQUIRE(result.valid() == true);
    H5::FileWriter fileWriter = std::move(result.value());
    herr_t err = ds.writeHdf5(fileWriter);
    REQUIRE(err == 0);
  }
#endif
}
