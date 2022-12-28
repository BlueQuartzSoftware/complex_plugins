#include <catch2/catch.hpp>

#include "ITKImageProcessing/Filters/ITKSumProjectionImage.hpp"

#include "ITKImageProcessing/Common/sitkCommon.hpp"

#include "complex/Parameters/NumberParameter.hpp"

#include "ITKImageProcessing/ITKImageProcessing_test_dirs.hpp"
#include "ITKTestBase.hpp"

#include "complex/UnitTest/UnitTestCommon.hpp"

#include <filesystem>

namespace fs = std::filesystem;

using namespace complex;

TEST_CASE("ITKSumProjectionImageFilter(z_projection)", "[ITKImageProcessing][ITKSumProjectionImage][z_projection]")
{
  DataStructure dataGraph;
  ITKSumProjectionImage filter;

  DataPath inputGeometryPath({ITKTestBase::k_ImageGeometryPath});
  DataPath inputDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_InputDataPath);
  DataPath cellDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_ImageCellDataPath);
  DataPath outputDataPath = cellDataPath.createChildPath(ITKTestBase::k_OutputDataPath);

  { // Start Image Comparison Scope
    fs::path inputFilePath = fs::path(unit_test::k_SourceDir.view()) / unit_test::k_DataDir.view() / "JSONFilters" / "Input/RA-Float.nrrd";
    Result<> imageReadResult = ITKTestBase::ReadImage(dataGraph, inputFilePath, inputGeometryPath, cellDataPath, inputDataPath);
    COMPLEX_RESULT_REQUIRE_VALID(imageReadResult);
  } // End Image Comparison Scope

  Arguments args;
  args.insertOrAssign(ITKSumProjectionImage::k_SelectedImageGeomPath_Key, std::make_any<DataPath>(inputGeometryPath));
  args.insertOrAssign(ITKSumProjectionImage::k_SelectedImageDataPath_Key, std::make_any<DataPath>(inputDataPath));
  args.insertOrAssign(ITKSumProjectionImage::k_OutputImageDataPath_Key, std::make_any<DataPath>(outputDataPath));
  args.insertOrAssign(ITKSumProjectionImage::k_ProjectionDimension_Key, std::make_any<UInt32Parameter::ValueType>(2));

  auto preflightResult = filter.preflight(dataGraph, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

  auto executeResult = filter.execute(dataGraph, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);

  fs::path baselineFilePath = fs::path(complex::unit_test::k_DataDir.view()) / "JSONFilters/Baseline/BasicFilters_SumProjectionImageFilter_z_projection.nrrd";
  DataPath baselineGeometryPath({ITKTestBase::k_BaselineGeometryPath});
  DataPath baseLineCellDataPath = baselineGeometryPath.createChildPath(ITKTestBase::k_ImageCellDataPath);
  DataPath baselineDataPath = baseLineCellDataPath.createChildPath(ITKTestBase::k_BaselineDataPath);
  Result<> readBaselineResult = ITKTestBase::ReadImage(dataGraph, baselineFilePath, baselineGeometryPath, baseLineCellDataPath, baselineDataPath);
  Result<> compareResult = ITKTestBase::CompareImages(dataGraph, baselineGeometryPath, baselineDataPath, inputGeometryPath, outputDataPath, 0.0001);
  COMPLEX_RESULT_REQUIRE_VALID(compareResult);
}
