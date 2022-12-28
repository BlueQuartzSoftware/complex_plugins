#include <catch2/catch.hpp>

#include "ITKImageProcessing/Filters/ITKValuedRegionalMinimaImage.hpp"

#include "ITKImageProcessing/Common/sitkCommon.hpp"

#include "complex/Parameters/BoolParameter.hpp"

#include "ITKImageProcessing/ITKImageProcessing_test_dirs.hpp"
#include "ITKTestBase.hpp"

#include "complex/UnitTest/UnitTestCommon.hpp"

#include <filesystem>

namespace fs = std::filesystem;

using namespace complex;

TEST_CASE("ITKValuedRegionalMinimaImageFilter(defaults)", "[ITKImageProcessing][ITKValuedRegionalMinimaImage][defaults]")
{
  DataStructure dataGraph;
  ITKValuedRegionalMinimaImage filter;

  DataPath inputGeometryPath({ITKTestBase::k_ImageGeometryPath});
  DataPath inputDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_InputDataPath);
  DataPath cellDataPath = inputGeometryPath.createChildPath(ITKTestBase::k_ImageCellDataPath);
  DataPath outputDataPath = cellDataPath.createChildPath(ITKTestBase::k_OutputDataPath);

  { // Start Image Comparison Scope
    fs::path inputFilePath = fs::path(unit_test::k_SourceDir.view()) / unit_test::k_DataDir.view() / "JSONFilters" / "Input/cthead1.png";
    Result<> imageReadResult = ITKTestBase::ReadImage(dataGraph, inputFilePath, inputGeometryPath, cellDataPath, inputDataPath);
    COMPLEX_RESULT_REQUIRE_VALID(imageReadResult);
  } // End Image Comparison Scope

  Arguments args;
  args.insertOrAssign(ITKValuedRegionalMinimaImage::k_SelectedImageGeomPath_Key, std::make_any<DataPath>(inputGeometryPath));
  args.insertOrAssign(ITKValuedRegionalMinimaImage::k_SelectedImageDataPath_Key, std::make_any<DataPath>(inputDataPath));
  args.insertOrAssign(ITKValuedRegionalMinimaImage::k_OutputImageDataPath_Key, std::make_any<DataPath>(outputDataPath));

  auto preflightResult = filter.preflight(dataGraph, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

  auto executeResult = filter.execute(dataGraph, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);

  std::string md5Hash = ITKTestBase::ComputeMd5Hash(dataGraph, outputDataPath);
  REQUIRE(md5Hash == "8297d018757b1477e31293ab8a8f0db1");
}
