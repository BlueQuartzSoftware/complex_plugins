#include <catch2/catch.hpp>

#include "ITKImageProcessing/Filters/ITKSaltAndPepperNoiseImage.hpp"

#include "ITKImageProcessing/Common/sitkCommon.hpp"

#include "complex/Parameters/NumberParameter.hpp"

#include "ITKImageProcessing/ITKImageProcessing_test_dirs.hpp"
#include "ITKTestBase.hpp"

#include "complex/UnitTest/UnitTestCommon.hpp"

#include <filesystem>

namespace fs = std::filesystem;

using namespace complex;

TEST_CASE("ITKSaltAndPepperNoiseImageFilter(2d)", "[ITKImageProcessing][ITKSaltAndPepperNoiseImage][2d]")
{
  DataStructure dataStructure;
  ITKSaltAndPepperNoiseImage filter;

  DataPath inputGeometryPath({ITKTestBase::k_ImageGeometryPath});
  DataPath inputDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_InputDataPath);
  DataPath cellDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_ImageCellDataPath);
  DataPath outputDataPath = cellDataPath.createChildPath(ITKTestBase::k_OutputDataPath);

  { // Start Image Comparison Scope
    fs::path inputFilePath = fs::path(unit_test::k_SourceDir.view()) / unit_test::k_DataDir.view() / "JSONFilters" / "Input/cthead1.png";
    Result<> imageReadResult = ITKTestBase::ReadImage(dataStructure, inputFilePath, inputGeometryPath, cellDataPath, inputDataPath);
    COMPLEX_RESULT_REQUIRE_VALID(imageReadResult);
  } // End Image Comparison Scope

  Arguments args;
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_SelectedImageGeomPath_Key, std::make_any<DataPath>(inputGeometryPath));
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_SelectedImageDataPath_Key, std::make_any<DataPath>(inputDataPath));
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_OutputImageDataPath_Key, std::make_any<DataPath>(outputDataPath));
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_Seed_Key, std::make_any<UInt32Parameter::ValueType>(123));

  auto preflightResult = filter.preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

  auto executeResult = filter.execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);

  fs::path baselineFilePath = fs::path(complex::unit_test::k_DataDir.view()) / "JSONFilters/Baseline/BasicFilters_SaltAndPepperNoiseImageFilter_2d.nrrd";
  DataPath baselineGeometryPath({ITKTestBase::k_BaselineGeometryPath});
  DataPath baseLineCellDataPath = baselineGeometryPath.createChildPath(ITKTestBase::k_ImageCellDataPath);
  DataPath baselineDataPath = baseLineCellDataPath.createChildPath(ITKTestBase::k_BaselineDataPath);
  Result<> readBaselineResult = ITKTestBase::ReadImage(dataStructure, baselineFilePath, baselineGeometryPath, baseLineCellDataPath, baselineDataPath);
  Result<> compareResult = ITKTestBase::CompareImages(dataStructure, baselineGeometryPath, baselineDataPath, inputGeometryPath, outputDataPath, 20);
  COMPLEX_RESULT_REQUIRE_VALID(compareResult);
}

TEST_CASE("ITKSaltAndPepperNoiseImageFilter(3d)", "[ITKImageProcessing][ITKSaltAndPepperNoiseImage][3d]")
{
  DataStructure dataStructure;
  ITKSaltAndPepperNoiseImage filter;

  DataPath inputGeometryPath({ITKTestBase::k_ImageGeometryPath});
  DataPath inputDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_InputDataPath);
  DataPath cellDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_ImageCellDataPath);
  DataPath outputDataPath = cellDataPath.createChildPath(ITKTestBase::k_OutputDataPath);

  { // Start Image Comparison Scope
    fs::path inputFilePath = fs::path(unit_test::k_SourceDir.view()) / unit_test::k_DataDir.view() / "JSONFilters" / "Input/RA-Short.nrrd";
    Result<> imageReadResult = ITKTestBase::ReadImage(dataStructure, inputFilePath, inputGeometryPath, cellDataPath, inputDataPath);
    COMPLEX_RESULT_REQUIRE_VALID(imageReadResult);
  } // End Image Comparison Scope

  Arguments args;
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_SelectedImageGeomPath_Key, std::make_any<DataPath>(inputGeometryPath));
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_SelectedImageDataPath_Key, std::make_any<DataPath>(inputDataPath));
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_OutputImageDataPath_Key, std::make_any<DataPath>(outputDataPath));
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_Seed_Key, std::make_any<UInt32Parameter::ValueType>(123));

  auto preflightResult = filter.preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

  auto executeResult = filter.execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);

  fs::path baselineFilePath = fs::path(complex::unit_test::k_DataDir.view()) / "JSONFilters/Baseline/BasicFilters_SaltAndPepperNoiseImageFilter_3d.nrrd";
  DataPath baselineGeometryPath({ITKTestBase::k_BaselineGeometryPath});
  DataPath baseLineCellDataPath = baselineGeometryPath.createChildPath(ITKTestBase::k_ImageCellDataPath);
  DataPath baselineDataPath = baseLineCellDataPath.createChildPath(ITKTestBase::k_BaselineDataPath);
  Result<> readBaselineResult = ITKTestBase::ReadImage(dataStructure, baselineFilePath, baselineGeometryPath, baseLineCellDataPath, baselineDataPath);
  Result<> compareResult = ITKTestBase::CompareImages(dataStructure, baselineGeometryPath, baselineDataPath, inputGeometryPath, outputDataPath, 10000.0);
  COMPLEX_RESULT_REQUIRE_VALID(compareResult);
}

TEST_CASE("ITKSaltAndPepperNoiseImageFilter(rgb)", "[ITKImageProcessing][ITKSaltAndPepperNoiseImage][rgb]")
{
  DataStructure dataStructure;
  ITKSaltAndPepperNoiseImage filter;

  DataPath inputGeometryPath({ITKTestBase::k_ImageGeometryPath});
  DataPath inputDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_InputDataPath);
  DataPath cellDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_ImageCellDataPath);
  DataPath outputDataPath = cellDataPath.createChildPath(ITKTestBase::k_OutputDataPath);

  { // Start Image Comparison Scope
    fs::path inputFilePath = fs::path(unit_test::k_SourceDir.view()) / unit_test::k_DataDir.view() / "JSONFilters" / "Input/VM1111Shrink-RGB.png";
    Result<> imageReadResult = ITKTestBase::ReadImage(dataStructure, inputFilePath, inputGeometryPath, cellDataPath, inputDataPath);
    COMPLEX_RESULT_REQUIRE_VALID(imageReadResult);
  } // End Image Comparison Scope

  Arguments args;
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_SelectedImageGeomPath_Key, std::make_any<DataPath>(inputGeometryPath));
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_SelectedImageDataPath_Key, std::make_any<DataPath>(inputDataPath));
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_OutputImageDataPath_Key, std::make_any<DataPath>(outputDataPath));
  args.insertOrAssign(ITKSaltAndPepperNoiseImage::k_Seed_Key, std::make_any<UInt32Parameter::ValueType>(123));

  auto preflightResult = filter.preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

  auto executeResult = filter.execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);

  fs::path baselineFilePath = fs::path(complex::unit_test::k_DataDir.view()) / "JSONFilters/Baseline/BasicFilters_SaltAndPepperNoiseImageFilter_rgb.nrrd";
  DataPath baselineGeometryPath({ITKTestBase::k_BaselineGeometryPath});
  DataPath baseLineCellDataPath = baselineGeometryPath.createChildPath(ITKTestBase::k_ImageCellDataPath);
  DataPath baselineDataPath = baseLineCellDataPath.createChildPath(ITKTestBase::k_BaselineDataPath);
  Result<> readBaselineResult = ITKTestBase::ReadImage(dataStructure, baselineFilePath, baselineGeometryPath, baseLineCellDataPath, baselineDataPath);
  Result<> compareResult = ITKTestBase::CompareImages(dataStructure, baselineGeometryPath, baselineDataPath, inputGeometryPath, outputDataPath, 20);
  COMPLEX_RESULT_REQUIRE_VALID(compareResult);
}
