#include <catch2/catch.hpp>

#include "ITKImageProcessing/Filters/ITKIsoContourDistanceImage.hpp"

#include "ITKImageProcessing/Common/sitkCommon.hpp"

#include "complex/Parameters/NumberParameter.hpp"

#include "ITKImageProcessing/ITKImageProcessing_test_dirs.hpp"
#include "ITKTestBase.hpp"

#include "complex/UnitTest/UnitTestCommon.hpp"

#include <filesystem>

namespace fs = std::filesystem;

using namespace complex;

TEST_CASE("ITKIsoContourDistanceImageFilter(default)", "[ITKImageProcessing][ITKIsoContourDistanceImage][default]")
{
  DataStructure dataGraph;
  ITKIsoContourDistanceImage filter;

  DataPath inputGeometryPath({ITKTestBase::k_ImageGeometryPath});
  DataPath inputDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_InputDataPath);
  DataPath cellDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_ImageCellDataPath);
  DataPath outputDataPath = cellDataPath.createChildPath(ITKTestBase::k_OutputDataPath);

  { // Start Image Comparison Scope
    fs::path inputFilePath = fs::path(unit_test::k_SourceDir.view()) / unit_test::k_DataDir.view() / "JSONFilters" / "Input/2th_cthead1.png";
    Result<> imageReadResult = ITKTestBase::ReadImage(dataGraph, inputFilePath, inputGeometryPath, cellDataPath, inputDataPath);
    COMPLEX_RESULT_REQUIRE_VALID(imageReadResult);
  } // End Image Comparison Scope

  Arguments args;
  args.insertOrAssign(ITKIsoContourDistanceImage::k_SelectedImageGeomPath_Key, std::make_any<DataPath>(inputGeometryPath));
  args.insertOrAssign(ITKIsoContourDistanceImage::k_SelectedImageDataPath_Key, std::make_any<DataPath>(inputDataPath));
  args.insertOrAssign(ITKIsoContourDistanceImage::k_OutputImageDataPath_Key, std::make_any<DataPath>(outputDataPath));
  args.insertOrAssign(ITKIsoContourDistanceImage::k_LevelSetValue_Key, std::make_any<Float64Parameter::ValueType>(50.0));

  auto preflightResult = filter.preflight(dataGraph, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

  auto executeResult = filter.execute(dataGraph, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);

  fs::path baselineFilePath = fs::path(complex::unit_test::k_DataDir.view()) / "JSONFilters/Baseline/BasicFilters_IsoContourDistanceImageFilter_default.nrrd";
  DataPath baselineGeometryPath({ITKTestBase::k_BaselineGeometryPath});
  DataPath baseLineCellDataPath = baselineGeometryPath.createChildPath(ITKTestBase::k_ImageCellDataPath);
  DataPath baselineDataPath = baseLineCellDataPath.createChildPath(ITKTestBase::k_BaselineDataPath);
  Result<> readBaselineResult = ITKTestBase::ReadImage(dataGraph, baselineFilePath, baselineGeometryPath, baseLineCellDataPath, baselineDataPath);
  Result<> compareResult = ITKTestBase::CompareImages(dataGraph, baselineGeometryPath, baselineDataPath, inputGeometryPath, outputDataPath, 0.0001);
  COMPLEX_RESULT_REQUIRE_VALID(compareResult);
}
