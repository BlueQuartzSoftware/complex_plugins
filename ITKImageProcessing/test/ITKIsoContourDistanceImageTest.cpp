#include <catch2/catch.hpp>

#include "complex/UnitTest/UnitTestCommon.hpp"
#include "complex/Utilities/Parsing/HDF5/H5FileWriter.hpp"
#include "ITKImageProcessing/Common/sitkCommon.hpp"
#include "ITKImageProcessing/Filters/ITKIsoContourDistanceImage.hpp"
#include "ITKImageProcessing/ITKImageProcessing_test_dirs.hpp"
#include "ITKTestBase.hpp"

#include <filesystem>

namespace fs = std::filesystem;

using namespace complex;

// Default parameter settings
TEST_CASE("ITK::IsoContourDistanceImageFilter(default)","[ITKImageProcessing][ITKIsoContourDistanceImage][default]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  ITKIsoContourDistanceImage filter;
  DataStructure ds;
  // Read the input image: Input/2th_cthead1.png
  {
    Arguments args;
    fs::path filePath = fs::path(unit_test::k_SourceDir.view()) / complex::unit_test::k_DataDir.str() / "JSONFilters" / "Input/2th_cthead1.png";
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
    args.insertOrAssign(ITKIsoContourDistanceImage::k_SelectedImageGeomPath_Key, std::make_any<DataPath>(inputGeometryPath));
    args.insertOrAssign(ITKIsoContourDistanceImage::k_SelectedImageDataPath_Key, std::make_any<DataPath>(inputDataPath));
    args.insertOrAssign(ITKIsoContourDistanceImage::k_OutputIamgeDataPath_Key, std::make_any<DataPath>(outputDataPath));
    args.insertOrAssign(ITKIsoContourDistanceImage::k_LevelSetValue_Key, std::make_any<double>(50.0));
    // Preflight the filter and check result
    auto preflightResult = filter.preflight(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);
    // Execute the filter and check the result
    auto executeResult = filter.execute(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  } // End Scope Section

  // Write the output data to a file, read and compare to baseline image
  {
    fs::path filePath = fs::path(unit_test::k_BinaryDir.view()) / "test/BasicFilters_IsoContourDistanceImageFilter_default.nrrd";
    DataPath inputGeometryPath({complex::ITKTestBase::k_ImageGeometryPath});
    DataPath outputDataPath = inputGeometryPath.createChildPath(complex::ITKTestBase::k_OutputDataPath);
    int32_t error = complex::ITKTestBase::WriteImage(ds, filePath, inputGeometryPath, outputDataPath);
    // Remove *all* files generated by this test
    fs::path testDir = fs::path(unit_test::k_BinaryDir.view()) / "test";
    ITKTestBase::RemoveFiles(testDir, "BasicFilters_IsoContourDistanceImageFilter_default");
    REQUIRE(error == 0);
    fs::path baselineFilePath = fs::path(unit_test::k_SourceDir.view()) / complex::unit_test::k_DataDir.str() / "JSONFilters/Baseline/BasicFilters_IsoContourDistanceImageFilter_default.nrrd";
    DataPath baselineGeometryPath({complex::ITKTestBase::k_BaselineGeometryPath});
    DataPath baselineDataPath = baselineGeometryPath.createChildPath(complex::ITKTestBase::k_BaselineDataPath);
    error = complex::ITKTestBase::ReadImage(ds, baselineFilePath, baselineGeometryPath, baselineDataPath);
    REQUIRE(error == 0);
    Result<> result = complex::ITKTestBase::CompareImages(ds, baselineGeometryPath, baselineDataPath, inputGeometryPath, outputDataPath, 0.0001);
    if(result.invalid())
    {
       for(const auto& err : result.errors())
       {
         std::cout << err.code << ": " << err.message << std::endl;
       }
    }
    REQUIRE(result.valid() == true);
  }
#if 0
  {
    fs::path filePath =fs::path( unit_test::k_BinaryDir.view()) / "test" / "IsoContourDistanceImageFilter_default.h5";
    Result<H5::FileWriter> result = H5::FileWriter::CreateFile(filePath);
    REQUIRE(result.valid() == true);
    H5::FileWriter fileWriter = std::move(result.value());
    herr_t err = ds.writeHdf5(fileWriter);
    REQUIRE(err == 0);
  }
#endif
}
