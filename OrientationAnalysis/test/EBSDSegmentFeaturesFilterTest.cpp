#include <catch2/catch.hpp>

#include "complex/Core/Application.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/ArrayThresholdsParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/Dream3dImportParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"
#include "complex/Utilities/Parsing/HDF5/H5FileReader.hpp"

#include "OrientationAnalysis/Filters/AlignSectionsMisorientationFilter.hpp"
#include "OrientationAnalysis/Filters/BadDataNeighborOrientationCheckFilter.hpp"
#include "OrientationAnalysis/Filters/ConvertOrientations.hpp"
#include "OrientationAnalysis/Filters/EBSDSegmentFeaturesFilter.hpp"
#include "OrientationAnalysis/Filters/NeighborOrientationCorrelationFilter.hpp"
#include "OrientationAnalysis/Filters/ReadH5EbsdFilter.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"
#include "OrientationAnalysis/Parameters/H5EbsdReaderParameter.h"

#include <filesystem>
namespace fs = std::filesystem;

using namespace complex;

constexpr float EPSILON = 0.000001;

namespace EbsdLib
{
namespace Ang
{
const std::string ConfidenceIndex("Confidence Index");
const std::string ImageQuality("Image Quality");

} // namespace Ang
namespace EnsembleData
{
inline const std::string CrystalStructures("CrystalStructures");
inline const std::string LatticeConstants("LatticeConstants");
inline const std::string MaterialName("MaterialName");
} // namespace EnsembleData
} // namespace EbsdLib

struct make_shared_enabler : public complex::Application
{
};

TEST_CASE("Reconstruction::EBSDSegmentFeatures: Instantiation and Parameter Check", "[Reconstruction][EBSDSegmentFeatures]")
{
  std::shared_ptr<make_shared_enabler> app = std::make_shared<make_shared_enabler>();
  app->loadPlugins(unit_test::k_BuildDir.view(), true);
  auto* filterList = Application::Instance()->getFilterList();

  // Make sure we can load the needed filters from the plugins
  const Uuid k_ComplexCorePluginId = *Uuid::FromString("05cc618b-781f-4ac0-b9ac-43f26ce1854f");
  const Uuid k_CorePluginId = *Uuid::FromString("65a0a3fc-8c93-5405-8ac6-182e7f313a69");

  const Uuid k_ImportDream3dFilterId = *Uuid::FromString("0dbd31c7-19e0-4077-83ef-f4a6459a0e2d");
  const FilterHandle k_ImportDream3dFilterHandle(k_ImportDream3dFilterId, k_ComplexCorePluginId);

  const Uuid k_MultiThresholdObjectsId = *Uuid::FromString("4246245e-1011-4add-8436-0af6bed19228");
  const FilterHandle k_MultiThresholdObjectsFilterHandle(k_MultiThresholdObjectsId, k_ComplexCorePluginId);

  const Uuid k_IdentifySampleFilterId = *Uuid::FromString("0e8c0818-a3fb-57d4-a5c8-7cb8ae54a40a");
  const FilterHandle k_IdentifySampleFilterHandle(k_IdentifySampleFilterId, k_ComplexCorePluginId);

  const Uuid k_DeleteDataFilterId = *Uuid::FromString("bf286740-e987-49fe-a7c8-6e566e3a0606");
  const FilterHandle k_DeleteDataFilterHandle(k_DeleteDataFilterId, k_ComplexCorePluginId);

  const Uuid k_AlignSectionsFeatCentroidFilterId = *Uuid::FromString("886f8b46-51b6-5682-a289-6febd10b7ef0");
  const FilterHandle k_AlignSectionsFeatCentroidFilterHandle(k_AlignSectionsFeatCentroidFilterId, k_CorePluginId);

  const std::string k_Quats("Quats");
  const std::string k_Phases("Phases");
  const std::string k_FeatureIds("FeatureIds");
  const std::string k_ConfidenceIndex = EbsdLib::Ang::ConfidenceIndex;
  const std::string k_ImageQuality = EbsdLib::Ang::ImageQuality;
  const std::string k_Mask("Mask");
  const std::string k_Active("Active");

  const std::string k_DataContainer("DataContainer");
  const DataPath k_DataContainerPath({k_DataContainer});
  const std::string k_GrainData("Grain Data");

  const DataPath k_CalculatedShiftsPath = k_DataContainerPath.createChildPath("Calculated Shifts");

  const DataPath k_CellAttributeMatrix = k_DataContainerPath.createChildPath("CellData");
  const DataPath k_EulersArrayPath = k_CellAttributeMatrix.createChildPath(EbsdLib::CellData::EulerAngles);
  const DataPath k_QuatsArrayPath = k_CellAttributeMatrix.createChildPath(k_Quats);
  const DataPath k_PhasesArrayPath = k_CellAttributeMatrix.createChildPath(k_Phases);
  const DataPath k_FitArrayPath = k_CellAttributeMatrix.createChildPath("Fit");
  const DataPath k_ConfidenceIndexArrayPath = k_CellAttributeMatrix.createChildPath(k_ConfidenceIndex);
  const DataPath k_ImageQualityArrayPath = k_CellAttributeMatrix.createChildPath(k_ImageQuality);
  const DataPath k_MaskArrayPath = k_CellAttributeMatrix.createChildPath(k_Mask);
  const DataPath k_SEMSignalArrayPath = k_CellAttributeMatrix.createChildPath("SEM Signal");
  const DataPath k_FeatureIdsArrayPath = k_CellAttributeMatrix.createChildPath(k_FeatureIds);

  const DataPath k_CellEnsembleAttributeMatrix = k_DataContainerPath.createChildPath("CellEnsembleData");
  const DataPath k_CrystalStructuresArrayPath = k_CellEnsembleAttributeMatrix.createChildPath(EbsdLib::EnsembleData::CrystalStructures);

  const DataPath k_CellFeatureAttributeMatrix = k_DataContainerPath.createChildPath(k_GrainData);
  const DataPath k_ActiveArrayPath = k_CellFeatureAttributeMatrix.createChildPath(k_Active);

  DataStructure exemplarDataStructure;
  // Read Exemplar DREAM3D File Filter
  {
    auto exemplarFilePath = fs::path(fmt::format("{}/TestFiles/6_6_ebsd_segment_features.dream3d", unit_test::k_DREAM3DDataDir));
    REQUIRE(fs::exists(exemplarFilePath));
    H5::FileReader exemplarReader(exemplarFilePath);
    H5::ErrorType h5Error = 0;
    exemplarDataStructure = DataStructure::readFromHdf5(exemplarReader, h5Error);
    REQUIRE(h5Error >= 0);
  }

  DataStructure ds;
  // Import DREAM3D filter
  {
    constexpr StringLiteral k_ImportFileData = "Import_File_Data";

    auto filter = filterList->createFilter(k_ImportDream3dFilterHandle);
    REQUIRE(nullptr != filter);

    Dream3dImportParameter::ImportData parameter;
    parameter.FilePath = fs::path(fmt::format("{}/TestFiles/Small_IN100.dream3d", unit_test::k_DREAM3DDataDir));

    Arguments args;
    args.insertOrAssign(k_ImportFileData, std::make_any<Dream3dImportParameter::ImportData>(parameter));

    // Preflight the filter and check result
    auto preflightResult = filter->preflight(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

    // Execute the filter and check the result
    auto executeResult = filter->execute(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result)
  }

  // Threshold Objects filter
  {
    constexpr StringLiteral k_ArrayThresholds_Key = "array_thresholds";
    constexpr StringLiteral k_CreatedDataPath_Key = "created_data_path";

    auto filter = filterList->createFilter(k_MultiThresholdObjectsFilterHandle);
    REQUIRE(nullptr != filter);

    Arguments args;

    ArrayThresholdSet arrayThresholdset;
    ArrayThresholdSet::CollectionType thresholds;

    std::shared_ptr<ArrayThreshold> ciThreshold = std::make_shared<ArrayThreshold>();
    ciThreshold->setArrayPath(k_ConfidenceIndexArrayPath);
    ciThreshold->setComparisonType(ArrayThreshold::ComparisonType::GreaterThan);
    ciThreshold->setComparisonValue(0.1);
    thresholds.push_back(ciThreshold);

    std::shared_ptr<ArrayThreshold> iqThreshold = std::make_shared<ArrayThreshold>();
    iqThreshold->setArrayPath(k_ImageQualityArrayPath);
    iqThreshold->setComparisonType(ArrayThreshold::ComparisonType::GreaterThan);
    iqThreshold->setComparisonValue(120.0);
    thresholds.push_back(iqThreshold);

    arrayThresholdset.setArrayThresholds(thresholds);

    args.insertOrAssign(k_ArrayThresholds_Key, std::make_any<ArrayThresholdsParameter::ValueType>(arrayThresholdset));
    args.insertOrAssign(k_CreatedDataPath_Key, std::make_any<DataPath>(k_MaskArrayPath));

    // Preflight the filter and check result
    auto preflightResult = filter->preflight(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter->execute(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Convert Orientation Representation filter
  {
    ConvertOrientations filter;
    Arguments args;
    args.insertOrAssign(ConvertOrientations::k_InputType_Key, std::make_any<ChoicesParameter::ValueType>(0));
    args.insertOrAssign(ConvertOrientations::k_OutputType_Key, std::make_any<ChoicesParameter::ValueType>(2));
    args.insertOrAssign(ConvertOrientations::k_InputOrientationArrayPath_Key, std::make_any<DataPath>(k_EulersArrayPath));
    args.insertOrAssign(ConvertOrientations::k_OutputOrientationArrayName_Key, std::make_any<DataPath>(k_QuatsArrayPath));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Align Sections (Misorientation) filter
  {
    AlignSectionsMisorientationFilter filter;
    Arguments args;

    // Create default Parameters for the filter.
    args.insertOrAssign(AlignSectionsMisorientationFilter::k_WriteAlignmentShifts_Key, std::make_any<bool>(true));
    args.insertOrAssign(AlignSectionsMisorientationFilter::k_AlignmentShiftFileName_Key,
                        std::make_any<FileSystemPathParameter::ValueType>(fs::path(fmt::format("{}/AlignSectionsMisorientation_1.txt", unit_test::k_BinaryDir))));

    args.insertOrAssign(AlignSectionsMisorientationFilter::k_MisorientationTolerance_Key, std::make_any<float32>(5.0F));

    args.insertOrAssign(AlignSectionsMisorientationFilter::k_GoodVoxels_Key, std::make_any<bool>(true));
    args.insertOrAssign(AlignSectionsMisorientationFilter::k_GoodVoxelsArrayPath_Key, std::make_any<DataPath>(k_MaskArrayPath));

    args.insertOrAssign(AlignSectionsMisorientationFilter::k_QuatsArrayPath_Key, std::make_any<DataPath>(k_QuatsArrayPath));
    args.insertOrAssign(AlignSectionsMisorientationFilter::k_CellPhasesArrayPath_Key, std::make_any<DataPath>(k_PhasesArrayPath));

    args.insertOrAssign(AlignSectionsMisorientationFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(k_CrystalStructuresArrayPath));

    args.insertOrAssign(AlignSectionsMisorientationFilter::k_SelectedImageGeometry_Key, std::make_any<DataPath>(k_DataContainerPath));
    args.insertOrAssign(AlignSectionsMisorientationFilter::k_SelectedCellDataGroup_Key, std::make_any<DataPath>(k_CellAttributeMatrix));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Identify Sample filter
  {
    auto filter = filterList->createFilter(k_IdentifySampleFilterHandle);
    REQUIRE(nullptr != filter);

    // Parameter Keys
    constexpr StringLiteral k_FillHoles_Key = "fill_holes";
    constexpr StringLiteral k_ImageGeom_Key = "image_geometry";
    constexpr StringLiteral k_GoodVoxels_Key = "good_voxels";

    Arguments args;
    args.insertOrAssign(k_FillHoles_Key, std::make_any<BoolParameter::ValueType>(false));
    args.insertOrAssign(k_ImageGeom_Key, std::make_any<GeometrySelectionParameter::ValueType>(k_DataContainerPath));
    args.insertOrAssign(k_GoodVoxels_Key, std::make_any<ArraySelectionParameter::ValueType>(k_MaskArrayPath));

    // Preflight the filter and check result
    auto preflightResult = filter->preflight(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter->execute(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Align Sections (Feature Centroid) filter
  {
    auto filter = filterList->createFilter(k_AlignSectionsFeatCentroidFilterHandle);
    REQUIRE(nullptr != filter);

    // Parameter Keys
    constexpr StringLiteral k_WriteAlignmentShifts_Key = "WriteAlignmentShifts";
    constexpr StringLiteral k_AlignmentShiftFileName_Key = "AlignmentShiftFileName";
    constexpr StringLiteral k_UseReferenceSlice_Key = "UseReferenceSlice";
    constexpr StringLiteral k_ReferenceSlice_Key = "ReferenceSlice";
    constexpr StringLiteral k_GoodVoxelsArrayPath_Key = "GoodVoxelsArrayPath";
    constexpr StringLiteral k_SelectedImageGeometry_Key = "SelectedImageGeometryPath";
    constexpr StringLiteral k_SelectedCellDataGroup_Key = "SelectedCellDataPath";

    Arguments args;
    // Create default Parameters for the filter.
    args.insertOrAssign(k_WriteAlignmentShifts_Key, std::make_any<bool>(true));
    args.insertOrAssign(k_AlignmentShiftFileName_Key, std::make_any<FileSystemPathParameter::ValueType>(fs::path(fmt::format("{}/AlignSectionsFeatureCentroid_1.txt", unit_test::k_BinaryDir))));
    args.insertOrAssign(k_UseReferenceSlice_Key, std::make_any<bool>(true));
    args.insertOrAssign(k_ReferenceSlice_Key, std::make_any<int32>(0));
    args.insertOrAssign(k_GoodVoxelsArrayPath_Key, std::make_any<DataPath>(k_MaskArrayPath));
    args.insertOrAssign(k_SelectedImageGeometry_Key, std::make_any<DataPath>(k_DataContainerPath));
    args.insertOrAssign(k_SelectedCellDataGroup_Key, std::make_any<DataPath>(k_CellAttributeMatrix));

    // Preflight the filter and check result
    auto preflightResult = filter->preflight(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter->execute(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Neighbor Orientation Comparison (Bad Data) filter
  {
    BadDataNeighborOrientationCheckFilter filter;
    Arguments args;
    // Create default Parameters for the filter.
    args.insertOrAssign(BadDataNeighborOrientationCheckFilter::k_MisorientationTolerance_Key, std::make_any<float32>(5.0f));
    args.insertOrAssign(BadDataNeighborOrientationCheckFilter::k_NumberOfNeighbors_Key, std::make_any<int32>(4));
    args.insertOrAssign(BadDataNeighborOrientationCheckFilter::k_ImageGeometryPath_Key, std::make_any<DataPath>(k_DataContainerPath));
    args.insertOrAssign(BadDataNeighborOrientationCheckFilter::k_QuatsArrayPath_Key, std::make_any<DataPath>(k_QuatsArrayPath));
    args.insertOrAssign(BadDataNeighborOrientationCheckFilter::k_GoodVoxelsArrayPath_Key, std::make_any<DataPath>(k_MaskArrayPath));
    args.insertOrAssign(BadDataNeighborOrientationCheckFilter::k_CellPhasesArrayPath_Key, std::make_any<DataPath>(k_PhasesArrayPath));
    args.insertOrAssign(BadDataNeighborOrientationCheckFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(k_CrystalStructuresArrayPath));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Neighbor Orientation Correlation filter
  {
    NeighborOrientationCorrelationFilter filter;
    Arguments args;
    // Create default Parameters for the filter.
    args.insertOrAssign(NeighborOrientationCorrelationFilter::k_ImageGeometryPath_Key, std::make_any<DataPath>(k_DataContainerPath));
    args.insertOrAssign(NeighborOrientationCorrelationFilter::k_MinConfidence_Key, std::make_any<float32>(0.2f));
    args.insertOrAssign(NeighborOrientationCorrelationFilter::k_MisorientationTolerance_Key, std::make_any<float32>(5.0f));
    args.insertOrAssign(NeighborOrientationCorrelationFilter::k_Level_Key, std::make_any<int32>(2));
    args.insertOrAssign(NeighborOrientationCorrelationFilter::k_ConfidenceIndexArrayPath_Key, std::make_any<DataPath>(k_ConfidenceIndexArrayPath));
    args.insertOrAssign(NeighborOrientationCorrelationFilter::k_CellPhasesArrayPath_Key, std::make_any<DataPath>(k_PhasesArrayPath));
    args.insertOrAssign(NeighborOrientationCorrelationFilter::k_QuatsArrayPath_Key, std::make_any<DataPath>(k_QuatsArrayPath));
    args.insertOrAssign(NeighborOrientationCorrelationFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(k_CrystalStructuresArrayPath));
    args.insertOrAssign(NeighborOrientationCorrelationFilter::k_IgnoredDataArrayPaths_Key, std::make_any<std::vector<DataPath>>());

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // EBSD Segment Features/Semgent Features (Misorientation) Filter
  {
    EBSDSegmentFeaturesFilter filter;
    Arguments args;

    // Create default Parameters for the filter.
    args.insertOrAssign(EBSDSegmentFeaturesFilter::k_MisorientationTolerance_Key, std::make_any<float32>(5.0F));
    args.insertOrAssign(EBSDSegmentFeaturesFilter::k_UseGoodVoxels_Key, std::make_any<bool>(true));
    args.insertOrAssign(EBSDSegmentFeaturesFilter::k_GridGeomPath_Key, std::make_any<DataPath>(k_DataContainerPath));
    args.insertOrAssign(EBSDSegmentFeaturesFilter::k_QuatsArrayPath_Key, std::make_any<DataPath>(k_QuatsArrayPath));
    args.insertOrAssign(EBSDSegmentFeaturesFilter::k_CellPhasesArrayPath_Key, std::make_any<DataPath>(k_PhasesArrayPath));
    args.insertOrAssign(EBSDSegmentFeaturesFilter::k_GoodVoxelsPath_Key, std::make_any<DataPath>(k_MaskArrayPath));
    args.insertOrAssign(EBSDSegmentFeaturesFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(k_CrystalStructuresArrayPath));
    args.insertOrAssign(EBSDSegmentFeaturesFilter::k_FeatureIdsArrayName_Key, std::make_any<DataPath>(k_FeatureIdsArrayPath));
    args.insertOrAssign(EBSDSegmentFeaturesFilter::k_CellFeatureAttributeMatrixName_Key, std::make_any<DataPath>(k_CellFeatureAttributeMatrix));
    args.insertOrAssign(EBSDSegmentFeaturesFilter::k_ActiveArrayName_Key, std::make_any<DataPath>(k_ActiveArrayPath));
    args.insertOrAssign(EBSDSegmentFeaturesFilter::k_RandomizeFeatures_Key, std::make_any<bool>(false));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(ds, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Loop and compare each array from the 'Exemplar Data / CellData' to the 'Data Container / CellData' group
  {
    const auto& generatedDataArray = ds.getDataRefAs<Int32Array>(k_FeatureIdsArrayPath);
    const auto& exemplarDataArray = exemplarDataStructure.getDataRefAs<Int32Array>(DataPath({"Small IN100", "EBSD Scan Data", k_FeatureIds}));

    const auto& newDataStore = generatedDataArray.getIDataStoreRefAs<Int32DataStore>();
    const auto& oldDataStore = exemplarDataArray.getIDataStoreRefAs<Int32DataStore>();
    usize start = 0;
    usize end = oldDataStore.getSize();
    for(usize i = start; i < end; i++)
    {
      if(oldDataStore[i] != newDataStore[i])
      {
        auto oldVal = oldDataStore[i];
        auto newVal = newDataStore[i];
        float diff = std::fabs(static_cast<float>(oldVal - newVal));
        REQUIRE(diff < EPSILON);
        break;
      }
    }
  }
}
