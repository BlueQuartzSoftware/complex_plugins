

#include <catch2/catch.hpp>

#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
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
 * Read DREAM3D File (read the exemplar 'align_sections_misorientation.dream3d' file from
 * [Optional] Write out dream3d file
 *
 *
 * Compare the shifts file 'align_sections_misorientation.txt' to what was written
 *
 * Compare all the data arrays from the "Exemplar Data / CellData"
 */

TEST_CASE("Core::AlignSectionsFeatureCentroidFilter: Instantiation and Parameter Check", "[Reconstruction][AlignSectionsFeatureCentroidFilter]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  AlignSectionsFeatureCentroidFilter filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(AlignSectionsFeatureCentroidFilter::k_WriteAlignmentShifts_Key, std::make_any<bool>(false));
  args.insertOrAssign(AlignSectionsFeatureCentroidFilter::k_AlignmentShiftFileName_Key, std::make_any<FileSystemPathParameter::ValueType>(fs::path("/Path/To/Output/File/To/Write.data")));
  args.insertOrAssign(AlignSectionsFeatureCentroidFilter::k_UseReferenceSlice_Key, std::make_any<bool>(true));
  args.insertOrAssign(AlignSectionsFeatureCentroidFilter::k_ReferenceSlice_Key, std::make_any<int32>(0));
  args.insertOrAssign(AlignSectionsFeatureCentroidFilter::k_GoodVoxelsArrayPath_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result);
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
