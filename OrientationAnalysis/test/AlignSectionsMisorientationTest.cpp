#include <catch2/catch.hpp>

#include "OrientationAnalysis/Filters/Algorithms/ReadH5Ebsd.hpp"
#include "OrientationAnalysis/Filters/AlignSectionsMisorientationFilter.hpp"
#include "OrientationAnalysis/Filters/ConvertOrientations.hpp"
#include "OrientationAnalysis/Filters/ReadH5EbsdFilter.hpp"
#include "OrientationAnalysis/Parameters/H5EbsdReaderParameter.h"

#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"

#include "complex/Common/Types.hpp"
#include "complex/Core/Application.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/NumericTypeParameter.hpp"

#include "complex/UnitTest/UnitTestCommon.hpp"

#include "EbsdLib/IO/TSL/AngConstants.h"

#include <filesystem>
namespace fs = std::filesystem;

using namespace complex;

/**
 * Read H5Ebsd File
 * Convert Orientation Representation (Euler->Quats)
 * Align Sections Misorientation
 * [Optional] Write out dream3d file
 *
 * Compare the shifts file
 */

struct make_shared_enabler : public complex::Application
{
};

TEST_CASE("OrientationAnalysis::AlignSectionsMisorientation_1", "[OrientationAnalysis][AlignSectionsMisorientation]")
{
  std::shared_ptr<make_shared_enabler> app = std::make_shared<make_shared_enabler>();
  app->loadPlugins(unit_test::k_BuildDir.view(), true);

  // Instantiate the filter, a DataStructure object and an Arguments Object
  const std::string k_Quats("Quats");
  const std::string k_Phases("Phases");

  const DataPath k_DataContainerPath({"DataContainer"});

  const DataPath k_CalculatedShiftsPath = k_DataContainerPath.createChildPath("Calculated Shifts");
  const DataPath k_ExemplarShiftsPath = k_DataContainerPath.createChildPath("Exemplar Shifts");

  const DataPath k_CellAttributeMatrix = k_DataContainerPath.createChildPath("CellData");
  const DataPath k_EulersArrayPath = k_CellAttributeMatrix.createChildPath(EbsdLib::CellData::EulerAngles);
  const DataPath k_QuatsArrayPath = k_CellAttributeMatrix.createChildPath(k_Quats);
  const DataPath k_PhasesArrayPath = k_CellAttributeMatrix.createChildPath(k_Phases);

  const DataPath k_CellEnsembleAttributeMatrix = k_DataContainerPath.createChildPath("CellEnsembleData");
  const DataPath k_CrystalStructuresArrayPath = k_CellEnsembleAttributeMatrix.createChildPath(EbsdLib::EnsembleData::CrystalStructures);

  DataStructure dataStructure;
  {
    ReadH5EbsdFilter filter;
    Arguments args;

    H5EbsdReaderParameter::ValueType h5ebsdParameter;
    h5ebsdParameter.inputFilePath = fmt::format("{}/TestFiles/SmallIN100.h5ebsd", unit_test::k_DREAM3DDataDir.view());
    h5ebsdParameter.startSlice = 1;
    h5ebsdParameter.endSlice = 117;
    h5ebsdParameter.eulerRepresentation = EbsdLib::AngleRepresentation::Radians;
    h5ebsdParameter.useRecommendedTransform = true;
    h5ebsdParameter.hdf5DataPaths = {EbsdLib::Ang::ConfidenceIndex, EbsdLib::Ang::ImageQuality, EbsdLib::H5Ebsd::Phases, EbsdLib::CellData::EulerAngles};

    // Create default Parameters for the filter.
    args.insertOrAssign(ReadH5EbsdFilter::k_ReadH5EbsdFilter_Key, std::make_any<H5EbsdReaderParameter::ValueType>(h5ebsdParameter));
    args.insertOrAssign(ReadH5EbsdFilter::k_DataContainerName_Key, std::make_any<DataPath>(k_DataContainerPath));
    args.insertOrAssign(ReadH5EbsdFilter::k_CellAttributeMatrixName_Key, std::make_any<DataPath>(k_CellAttributeMatrix));
    args.insertOrAssign(ReadH5EbsdFilter::k_CellEnsembleAttributeMatrixName_Key, std::make_any<DataPath>(k_CellEnsembleAttributeMatrix));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    REQUIRE(preflightResult.outputActions.valid());

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    REQUIRE(executeResult.result.valid());
  }

  {
    Arguments args;
    ConvertOrientations filter;

    args.insertOrAssign(ConvertOrientations::k_InputType_Key, std::make_any<ChoicesParameter::ValueType>(0));
    args.insertOrAssign(ConvertOrientations::k_OutputType_Key, std::make_any<ChoicesParameter::ValueType>(2));
    args.insertOrAssign(ConvertOrientations::k_InputOrientationArrayPath_Key, std::make_any<DataPath>(k_EulersArrayPath));
    args.insertOrAssign(ConvertOrientations::k_OutputOrientationArrayName_Key, std::make_any<DataPath>(k_QuatsArrayPath));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    REQUIRE(preflightResult.outputActions.valid());

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    REQUIRE(executeResult.result.valid());
  }

  {
    Arguments args;
    AlignSectionsMisorientationFilter filter;
    // Create default Parameters for the filter.
    args.insertOrAssign(AlignSectionsMisorientationFilter::k_WriteAlignmentShifts_Key, std::make_any<bool>(true));
    args.insertOrAssign(AlignSectionsMisorientationFilter::k_AlignmentShiftFileName_Key,
                        std::make_any<FileSystemPathParameter::ValueType>(fs::path(fmt::format("{}/AlignSectionsMisorientation_1.txt", unit_test::k_BinaryDir))));

    args.insertOrAssign(AlignSectionsMisorientationFilter::k_MisorientationTolerance_Key, std::make_any<float32>(5.0F));

    args.insertOrAssign(AlignSectionsMisorientationFilter::k_GoodVoxels_Key, std::make_any<bool>(false));
    args.insertOrAssign(AlignSectionsMisorientationFilter::k_GoodVoxelsArrayPath_Key, std::make_any<DataPath>(DataPath{}));

    args.insertOrAssign(AlignSectionsMisorientationFilter::k_QuatsArrayPath_Key, std::make_any<DataPath>(k_QuatsArrayPath));
    args.insertOrAssign(AlignSectionsMisorientationFilter::k_CellPhasesArrayPath_Key, std::make_any<DataPath>(k_PhasesArrayPath));

    args.insertOrAssign(AlignSectionsMisorientationFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(k_CrystalStructuresArrayPath));

    args.insertOrAssign(AlignSectionsMisorientationFilter::k_SelectedImageGeometry_Key, std::make_any<DataPath>(k_DataContainerPath));
    args.insertOrAssign(AlignSectionsMisorientationFilter::k_SelectedCellDataGroup_Key, std::make_any<DataPath>(k_CellAttributeMatrix));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  {
    static constexpr StringLiteral k_InputFileKey = "input_file";
    static constexpr StringLiteral k_ScalarTypeKey = "scalar_type";
    static constexpr StringLiteral k_NTuplesKey = "n_tuples";
    static constexpr StringLiteral k_NCompKey = "n_comp";
    static constexpr StringLiteral k_NSkipLinesKey = "n_skip_lines";
    static constexpr StringLiteral k_DelimiterChoiceKey = "delimiter_choice";
    static constexpr StringLiteral k_DataArrayKey = "output_data_array";

    // Compare the output of the shifts file with the exemplar file
    const Uuid k_CorePluginId = *Uuid::FromString("65a0a3fc-8c93-5405-8ac6-182e7f313a69");
    const Uuid k_ComplexCorePluginId = *Uuid::FromString("05cc618b-781f-4ac0-b9ac-43f26ce1854f");
    auto* filterList = Application::Instance()->getFilterList();

    const Uuid k_ImportTextFilterId = *Uuid::FromString("25f7df3e-ca3e-4634-adda-732c0e56efd4");
    const FilterHandle k_ImportTextFilterHandle(k_ImportTextFilterId, k_ComplexCorePluginId);
    auto filter = filterList->createFilter(k_ImportTextFilterHandle);
    REQUIRE(nullptr != filter);

    Arguments args;
    args.insertOrAssign(k_InputFileKey, std::make_any<FileSystemPathParameter::ValueType>(fs::path(fmt::format("{}/AlignSectionsMisorientation_1.txt", unit_test::k_BinaryDir))));
    args.insertOrAssign(k_ScalarTypeKey, std::make_any<NumericTypeParameter::ValueType>(complex::NumericType::int32));
    args.insertOrAssign(k_NTuplesKey, std::make_any<uint64>(116));
    args.insertOrAssign(k_NCompKey, std::make_any<uint64>(6));
    args.insertOrAssign(k_NSkipLinesKey, std::make_any<uint64>(0));
    args.insertOrAssign(k_DelimiterChoiceKey, std::make_any<ChoicesParameter::ValueType>(4));
    args.insertOrAssign(k_DataArrayKey, std::make_any<DataPath>(k_CalculatedShiftsPath));

    // Preflight the filter and check result
    auto preflightResult = filter->preflight(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter->execute(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);

    // Run the filter again, but this time read in the exemplar data set.
    args.insertOrAssign(k_InputFileKey, std::make_any<FileSystemPathParameter::ValueType>(fs::path(fmt::format("{}/align_sections_shift_file_6_5.txt", unit_test::k_DataDir))));
    args.insertOrAssign(k_ScalarTypeKey, std::make_any<NumericTypeParameter::ValueType>(complex::NumericType::int32));
    args.insertOrAssign(k_NTuplesKey, std::make_any<uint64>(116));
    args.insertOrAssign(k_NCompKey, std::make_any<uint64>(6));
    args.insertOrAssign(k_NSkipLinesKey, std::make_any<uint64>(0));
    args.insertOrAssign(k_DelimiterChoiceKey, std::make_any<ChoicesParameter::ValueType>(4));
    args.insertOrAssign(k_DataArrayKey, std::make_any<DataPath>(k_ExemplarShiftsPath));

    // Preflight the filter and check result
    preflightResult = filter->preflight(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    executeResult = filter->execute(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);

    const auto& calcShifts = dataStructure.getDataRefAs<Int32Array>(k_CalculatedShiftsPath);
    const auto& exemplarShifts = dataStructure.getDataRefAs<Int32Array>(k_ExemplarShiftsPath);

    size_t numElements = calcShifts.getSize();
    bool sameValue = true;
    for(size_t i = 0; i < numElements; i++)
    {
      sameValue = (calcShifts[i] == exemplarShifts[i]);
      if(!sameValue)
      {
        REQUIRE(calcShifts[i] == exemplarShifts[i]);
      }
    }
  }
}
