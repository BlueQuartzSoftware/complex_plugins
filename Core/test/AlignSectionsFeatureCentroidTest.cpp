

#include <catch2/catch.hpp>

#include "complex/Core/Application.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/ArrayThresholdsParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/Dream3dImportParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/NumericTypeParameter.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"

#include <filesystem>
namespace fs = std::filesystem;

#include "Core/Core_test_dirs.hpp"
#include "Core/Filters/AlignSectionsFeatureCentroidFilter.hpp"

using namespace complex;

/**
 * Read H5Ebsd File
 * MultiThreshold Objects
 * Convert Orientation Representation (Euler->Quats)
 * Align Sections Misorientation
 * Identify Sample
 * Align Sections Feature Centroid
 *
 * Read DREAM3D File (read the exemplar 'align_sections_feature_centroid.dream3d' file from
 * [Optional] Write out dream3d file
 *
 *
 * Compare the shifts file 'align_sections_feature_centroid.txt' to what was written
 *
 * Compare all the data arrays from the "Exemplar Data / CellData"
 */

template <typename T>
void CompareDataArrays(const IDataArray& left, const IDataArray& right)
{
  const auto& oldDataStore = left.getIDataStoreRefAs<AbstractDataStore<T>>();
  const auto& newDataStore = right.getIDataStoreRefAs<AbstractDataStore<T>>();
  usize start = 0;
  usize end = oldDataStore.getSize();
  bool same = true;
  usize badIndex = 0;
  for(usize i = start; i < end; i++)
  {
    if(oldDataStore[i] != newDataStore[i])
    {
      badIndex = i;
      REQUIRE(oldDataStore[badIndex] == newDataStore[badIndex]);
      break;
    }
  }
}

struct make_shared_enabler : public complex::Application
{
};

TEST_CASE("Core::AlignSectionsFeatureCentroidFilter: Instantiation and Parameter Check", "[Reconstruction][AlignSectionsFeatureCentroidFilter]")
{
  std::shared_ptr<make_shared_enabler> app = std::make_shared<make_shared_enabler>();
  app->loadPlugins(unit_test::k_BuildDir.view(), true);
  auto* filterList = Application::Instance()->getFilterList();

  // Make sure we can load the needed filters from the plugins
  const Uuid k_ComplexCorePluginId = *Uuid::FromString("05cc618b-781f-4ac0-b9ac-43f26ce1854f");

  // Make sure we can instantiate the Import Text Filter
  const Uuid k_MultiThresholdObjectsId = *Uuid::FromString("4246245e-1011-4add-8436-0af6bed19228");
  const FilterHandle k_MultiThresholdObjectsFilterHandle(k_MultiThresholdObjectsId, k_ComplexCorePluginId);

  const Uuid k_ImportTextFilterId = *Uuid::FromString("25f7df3e-ca3e-4634-adda-732c0e56efd4");
  const FilterHandle k_ImportTextFilterHandle(k_ImportTextFilterId, k_ComplexCorePluginId);

  const Uuid k_ImportDream3dFilterId = *Uuid::FromString("0dbd31c7-19e0-4077-83ef-f4a6459a0e2d");
  const FilterHandle k_ImportDream3dFilterHandle(k_ImportDream3dFilterId, k_ComplexCorePluginId);

  const Uuid k_OrientationAnalysisPluginId = *Uuid::FromString("c09cf01b-014e-5adb-84eb-ea76fc79eeb1");

  const Uuid k_ReadH5EbsdFilterId = *Uuid::FromString("4ef7f56b-616e-5a80-9e68-1da8f35ad235");
  const FilterHandle k_ReadH5EbsdFilterHandle(k_ReadH5EbsdFilterId, k_OrientationAnalysisPluginId);

#if 0
  // Instantiate the filter, a DataStructure object and an Arguments Object
  const std::string k_Quats("Quats");
  const std::string k_Phases("Phases");
  const std::string k_ConfidenceIndex = EbsdLib::Ang::ConfidenceIndex;
  const std::string k_ImageQuality = EbsdLib::Ang::ImageQuality;

  const std::string k_Mask("Mask");
  const std::string k_DataContainer("DataContainer");
  const DataPath k_DataContainerPath({k_DataContainer});

  const std::string k_ExemplarDataContainer("Exemplar Data");
  const DataPath k_ExemplarDataContainerPath({k_ExemplarDataContainer});

  const DataPath k_CalculatedShiftsPath = k_DataContainerPath.createChildPath("Calculated Shifts");

  const DataPath k_CellAttributeMatrix = k_DataContainerPath.createChildPath("CellData");
  const DataPath k_EulersArrayPath = k_CellAttributeMatrix.createChildPath(EbsdLib::CellData::EulerAngles);
  const DataPath k_QuatsArrayPath = k_CellAttributeMatrix.createChildPath(k_Quats);
  const DataPath k_PhasesArrayPath = k_CellAttributeMatrix.createChildPath(k_Phases);
  const DataPath k_ConfidenceIndexArrayPath = k_CellAttributeMatrix.createChildPath(k_ConfidenceIndex);
  const DataPath k_ImageQualityArrayPath = k_CellAttributeMatrix.createChildPath(k_ImageQuality);
  const DataPath k_MaskArrayPath = k_CellAttributeMatrix.createChildPath(k_Mask);

  const DataPath k_CellEnsembleAttributeMatrix = k_DataContainerPath.createChildPath("CellEnsembleData");
  const DataPath k_CrystalStructuresArrayPath = k_CellEnsembleAttributeMatrix.createChildPath(EbsdLib::EnsembleData::CrystalStructures);

  const DataPath k_ExemplarShiftsPath = k_DataContainerPath.createChildPath("Exemplar Shifts");

  DataStructure dataStructure;

  // Read Exemplar DREAM3D File Filter
  {
    constexpr StringLiteral k_ImportFileData = "Import_File_Data";

    auto filter = filterList->createFilter(k_ImportDream3dFilterHandle);
    REQUIRE(nullptr != filter);

    Dream3dImportParameter::ImportData parameter;
    parameter.FilePath = fs::path(fmt::format("{}/TestFiles/align_sections_feature_centroid.dream3d", unit_test::k_DREAM3DDataDir));

    Arguments args;
    args.insertOrAssign(k_ImportFileData, std::make_any<Dream3dImportParameter::ImportData>(parameter));

    // Preflight the filter and check result
    auto preflightResult = filter->preflight(dataStructure, args);
    REQUIRE(preflightResult.outputActions.valid());

    // Execute the filter and check the result
    auto executeResult = filter->execute(dataStructure, args);
    REQUIRE(executeResult.result.valid());
  }

  // Read the H5Ebsd File
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

  // MultiThreshold Objects Filter
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
    auto preflightResult = filter->preflight(dataStructure, args);
    REQUIRE(preflightResult.outputActions.valid());

    // Execute the filter and check the result
    auto executeResult = filter->execute(dataStructure, args);
    REQUIRE(executeResult.result.valid());
  }

  // Convert Orientations Filter
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

  // Align Sections Misorientation Filter
  {
    Arguments args;
    AlignSectionsMisorientationFilter filter;
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
    auto preflightResult = filter.preflight(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
  }

  // Identify Sample Filter
  {

  }

  // Align Sections Feature Centroid Filter
  {
  }

  // Compare Shift File
  {

  }
  // Compare cell arrays
  {}

#endif
}

#if 0
{
  "isDisabled": false,
  "name": "align_sections_featur_centroid.d3dpipeline",
  "pinnedParams": [],
  "pipeline": [
    {
      "args": {
        "CellAttributeMatrixName": "Data Container/CellData",
        "CellEnsembleAttributeMatrixName": "Data Container/CellEnsembleData",
        "DataContainerName": "Exemplar Data",
        "ReadH5EbsdFilter": {
          "endSlice": 117,
          "eulerRepresentation": 0,
          "hdf5DataPaths": [
            "Confidence Index",
            "EulerAngles",
            "Image Quality",
            "Phases"
          ],
          "inputFilePath": "SmallIN100.h5ebsd",
          "startSlice": 1,
          "useRecommendedTransform": true
        }
      },
      "filter": {
        "name": "complex::ReadH5EbsdFilter",
        "uuid": "4ef7f56b-616e-5a80-9e68-1da8f35ad235"
      },
      "isDisabled": false
    },
    {
      "args": {
        "array_thresholds": {
          "inverted": false,
          "thresholds": [
            {
              "array_path": "Data Container/CellData/Confidence Index",
              "comparison": 0,
              "inverted": false,
              "type": "array",
              "union": 0,
              "value": 0.1
            },
            {
              "array_path": "Data Container/CellData/Image Quality",
              "comparison": 0,
              "inverted": false,
              "type": "array",
              "union": 0,
              "value": 120.0
            }
          ],
          "type": "collection",
          "union": 0
        },
        "created_data_path": "Data Container/CellData/Mask"
      },
      "filter": {
        "name": "complex::MultiThresholdObjects",
        "uuid": "4246245e-1011-4add-8436-0af6bed19228"
      },
      "isDisabled": false
    },
    {
      "args": {
        "InputOrientationArrayPath": "Data Container/CellData/EulerAngles",
        "InputType": 0,
        "OutputOrientationArrayName": "Data Container/CellData/Quats",
        "OutputType": 2
      },
      "filter": {
        "name": "complex::ConvertOrientations",
        "uuid": "e5629880-98c4-5656-82b8-c9fe2b9744de"
      },
      "isDisabled": false
    },
    {
      "args": {
        "AlignmentShiftFileName": "align_sections_misorientation.txt",
        "CellPhasesArrayPath": "Data Container/CellData/Phases",
        "CrystalStructuresArrayPath": "Data Container/CellEnsembleData/CrystalStructures",
        "GoodVoxelsArrayPath": "Data Container/CellData/Mask",
        "MisorientationTolerance": 5.0,
        "QuatsArrayPath": "Data Container/CellData/Quats",
        "SelectedCellDataPath": "Data Container/CellData",
        "SelectedImageGeometryPath": "Exemplar Data",
        "UseGoodVoxels": true,
        "WriteAlignmentShifts": true
      },
      "filter": {
        "name": "complex::AlignSectionsMisorientationFilter",
        "uuid": "4fb2b9de-3124-534b-b914-dbbbdbc14604"
      },
      "isDisabled": false
    },
    {
      "args": {
        "fill_holes": false,
        "good_voxels": "Data Container/CellData/Mask",
        "image_geometry": "Exemplar Data"
      },
      "filter": {
        "name": "complex::IdentifySample",
        "uuid": "0e8c0818-a3fb-57d4-a5c8-7cb8ae54a40a"
      },
      "isDisabled": false
    },
    {
      "args": {
        "AlignmentShiftFileName": "align_sections_feature_centroid.txt",
        "GoodVoxelsArrayPath": "Data Container/CellData/Mask",
        "ReferenceSlice": 0,
        "SelectedCellDataPath": "Data Container/CellData",
        "SelectedImageGeometryPath": "Exemplar Data",
        "UseReferenceSlice": true,
        "WriteAlignmentShifts": true
      },
      "filter": {
        "name": "complex::AlignSectionsFeatureCentroidFilter",
        "uuid": "886f8b46-51b6-5682-a289-6febd10b7ef0"
      },
      "isDisabled": false
    },
    {
      "args": {
        "Export_File_Path": "align_sections_feature_centroid.dream3d"
      },
      "filter": {
        "name": "complex::ExportDREAM3DFilter",
        "uuid": "b3a95784-2ced-11ec-8d3d-0242ac130003"
      },
      "isDisabled": false
    },
{
  "args": {
    "Import_File_Data": {
      "datapaths": [
        "Exemplar Data/CellEnsembleData",
        "Exemplar Data/CellEnsembleData/CrystalStructures",
        "Exemplar Data/CellEnsembleData/LatticeConstants",
        "Exemplar Data",
        "Exemplar Data/CellData",
        "Exemplar Data/CellData/Confidence Index",
        "Exemplar Data/CellData/EulerAngles",
        "Exemplar Data/CellData/Image Quality",
        "Exemplar Data/CellData/Phases",
        "Exemplar Data/CellData/Mask",
        "Exemplar Data/CellData/Quats"
      ],
      "filepath": "DREAM3D_Data/TestFiles/align_sections_feature_centroid.dream3d"
    }
    },
    "filter": {
      "name": "complex::ImportDREAM3DFilter",
      "uuid": "0dbd31c7-19e0-4077-83ef-f4a6459a0e2d"
    },
    "isDisabled": false
  }
  ],
  "workflowParams": []
}
#endif
