#pragma once

#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/ArrayThresholdsParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"

#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"

#include <fmt/format.h>

#include <filesystem>
namespace fs = std::filesystem;

using namespace complex;

namespace SmallIn100
{
// Make sure we can load the needed filters from the plugins
const Uuid k_ComplexCorePluginId = *Uuid::FromString("05cc618b-781f-4ac0-b9ac-43f26ce1854f");
// Make sure we can instantiate the MultiThreshold Objects Filter
const Uuid k_MultiThresholdObjectsId = *Uuid::FromString("4246245e-1011-4add-8436-0af6bed19228");
const FilterHandle k_MultiThresholdObjectsFilterHandle(k_MultiThresholdObjectsId, k_ComplexCorePluginId);
// Make sure we can instantiate the IdentifySample
const Uuid k_IdentifySampleFilterId = *Uuid::FromString("94d47495-5a89-4c7f-a0ee-5ff20e6bd273");
const FilterHandle k_IdentifySampleFilterHandle(k_IdentifySampleFilterId, k_ComplexCorePluginId);

/* This is temporary until it is ported to ComplexCore */
const Uuid k_CorePluginId = *Uuid::FromString("65a0a3fc-8c93-5405-8ac6-182e7f313a69");
// Make sure we can instantiate the Align Sections Feature Centroid
const Uuid k_AlignSectionsFeatureCentroidFilterId = *Uuid::FromString("b83f9bae-9ccf-4932-96c3-7f2fdb091452");
const FilterHandle k_AlignSectionsFeatureCentroidFilterHandle(k_AlignSectionsFeatureCentroidFilterId, k_CorePluginId);

// Orientation Analysis Plugin List
const Uuid k_OrientationAnalysisPluginId = *Uuid::FromString("c09cf01b-014e-5adb-84eb-ea76fc79eeb1");
// Make sure we can instantiate the Convert Orientations
const Uuid k_ConvertOrientationsFilterId = *Uuid::FromString("501e54e6-a66f-4eeb-ae37-00e649c00d4b");
const FilterHandle k_ConvertOrientationsFilterHandle(k_ConvertOrientationsFilterId, k_OrientationAnalysisPluginId);
// Make sure we can instantiate the EbsdSegmentFeatures
const Uuid k_EbsdSegmentFeaturesFilterId = *Uuid::FromString("1810c2c7-63e3-41db-b204-a5821e6271c0");
const FilterHandle k_EbsdSegmentFeaturesFilterHandle(k_EbsdSegmentFeaturesFilterId, k_OrientationAnalysisPluginId);
// Make sure we can instantiate the Align Sections Misorientation
const Uuid k_AlignSectionMisorientationFilterId = *Uuid::FromString("8df2135c-7079-49f4-9756-4f3c028a5ced");
const FilterHandle k_AlignSectionMisorientationFilterHandle(k_AlignSectionMisorientationFilterId, k_OrientationAnalysisPluginId);
// Make sure we can instantiate the NeighborOrientationCorrelation
const Uuid k_NeighborOrientationCorrelationFilterId = *Uuid::FromString("4625c192-7e46-4333-a294-67a2eb64cb37");
const FilterHandle k_NeighborOrientationCorrelationFilterHandle(k_NeighborOrientationCorrelationFilterId, k_OrientationAnalysisPluginId);
// Make sure we can instantiate the
const Uuid k_BadDataNeighborOrientationCheckFilterId = *Uuid::FromString("3f342977-aea1-49e1-a9c2-f73760eba0d3");
const FilterHandle k_BadDataNeighborOrientationCheckFilterHandle(k_BadDataNeighborOrientationCheckFilterId, k_OrientationAnalysisPluginId);

//------------------------------------------------------------------------------
inline void ExecuteMultiThresholdObjects(DataStructure& dataStructure, const FilterList& filterList)
{
  constexpr StringLiteral k_ArrayThresholds_Key = "array_thresholds";
  constexpr StringLiteral k_CreatedDataPath_Key = "created_data_path";
  INFO(fmt::format("Error creating Filter '{}'  ", k_MultiThresholdObjectsFilterHandle.getFilterName()));

  auto filter = filterList.createFilter(k_MultiThresholdObjectsFilterHandle);
  REQUIRE(nullptr != filter);

  Arguments args;

  ArrayThresholdSet arrayThresholdset;
  ArrayThresholdSet::CollectionType thresholds;

  std::shared_ptr<ArrayThreshold> ciThreshold = std::make_shared<ArrayThreshold>();
  ciThreshold->setArrayPath(Constants::k_ConfidenceIndexArrayPath);
  ciThreshold->setComparisonType(ArrayThreshold::ComparisonType::GreaterThan);
  ciThreshold->setComparisonValue(0.1);
  thresholds.push_back(ciThreshold);

  std::shared_ptr<ArrayThreshold> iqThreshold = std::make_shared<ArrayThreshold>();
  iqThreshold->setArrayPath(Constants::k_ImageQualityArrayPath);
  iqThreshold->setComparisonType(ArrayThreshold::ComparisonType::GreaterThan);
  iqThreshold->setComparisonValue(120.0);
  thresholds.push_back(iqThreshold);

  arrayThresholdset.setArrayThresholds(thresholds);

  args.insertOrAssign(k_ArrayThresholds_Key, std::make_any<ArrayThresholdsParameter::ValueType>(arrayThresholdset));
  args.insertOrAssign(k_CreatedDataPath_Key, std::make_any<DataPath>(Constants::k_MaskArrayPath));

  // Preflight the filter and check result
  auto preflightResult = filter->preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

  // Execute the filter and check the result
  auto executeResult = filter->execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result)
}

//------------------------------------------------------------------------------
inline void ExecuteConvertOrientations(DataStructure& dataStructure, const FilterList& filterList)
{
  INFO(fmt::format("Error creating Filter '{}'  ", k_ConvertOrientationsFilterHandle.getFilterName()));
  auto filter = filterList.createFilter(k_ConvertOrientationsFilterHandle);
  REQUIRE(nullptr != filter);

  // Parameter Keys from AlignSectionsMisorientation. If those change these will need to be updated
  constexpr StringLiteral k_InputType_Key = "input_type";
  constexpr StringLiteral k_OutputType_Key = "output_type";
  constexpr StringLiteral k_InputOrientationArrayPath_Key = "input_orientation_array_path";
  constexpr StringLiteral k_OutputOrientationArrayName_Key = "output_orientation_array_name";

  Arguments args;
  args.insertOrAssign(k_InputType_Key, std::make_any<ChoicesParameter::ValueType>(0));
  args.insertOrAssign(k_OutputType_Key, std::make_any<ChoicesParameter::ValueType>(2));
  args.insertOrAssign(k_InputOrientationArrayPath_Key, std::make_any<DataPath>(Constants::k_EulersArrayPath));
  args.insertOrAssign(k_OutputOrientationArrayName_Key, std::make_any<std::string>(Constants::k_Quats));

  // Preflight the filter and check result
  auto preflightResult = filter->preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

  // Execute the filter and check the result
  auto executeResult = filter->execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result)
}

//------------------------------------------------------------------------------
inline void ExecuteIdentifySample(DataStructure& dataStructure, const FilterList& filterList)
{
  INFO(fmt::format("Error creating Filter '{}'  ", k_IdentifySampleFilterHandle.getFilterName()));
  auto filter = filterList.createFilter(k_IdentifySampleFilterHandle);
  REQUIRE(nullptr != filter);

  // Parameter Keys
  constexpr StringLiteral k_FillHoles_Key = "fill_holes";
  constexpr StringLiteral k_ImageGeom_Key = "image_geometry";
  constexpr StringLiteral k_GoodVoxels_Key = "good_voxels";

  Arguments args;
  args.insertOrAssign(k_FillHoles_Key, std::make_any<BoolParameter::ValueType>(false));
  args.insertOrAssign(k_ImageGeom_Key, std::make_any<GeometrySelectionParameter::ValueType>(Constants::k_DataContainerPath));
  args.insertOrAssign(k_GoodVoxels_Key, std::make_any<ArraySelectionParameter::ValueType>(Constants::k_MaskArrayPath));

  // Preflight the filter and check result
  auto preflightResult = filter->preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

  // Execute the filter and check the result
  auto executeResult = filter->execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result)
}

//------------------------------------------------------------------------------
inline void ExecuteEbsdSegmentFeatures(DataStructure& dataStructure, const FilterList& filterList)
{
  INFO(fmt::format("Error creating Filter '{}'  ", k_EbsdSegmentFeaturesFilterHandle.getFilterName()));
  auto filter = filterList.createFilter(k_EbsdSegmentFeaturesFilterHandle);
  REQUIRE(nullptr != filter);

  // Parameter Keys
  constexpr StringLiteral k_GridGeomPath_Key = "grid_geometry_path";
  constexpr StringLiteral k_MisorientationTolerance_Key = "misorientation_tolerance";
  constexpr StringLiteral k_UseGoodVoxels_Key = "use_good_voxels";
  constexpr StringLiteral k_QuatsArrayPath_Key = "quats_array_path";
  constexpr StringLiteral k_CellPhasesArrayPath_Key = "cell_phases_array_path";
  constexpr StringLiteral k_GoodVoxelsPath_Key = "good_voxels_array_path";
  constexpr StringLiteral k_CrystalStructuresArrayPath_Key = "crystal_structures_array_path";
  constexpr StringLiteral k_FeatureIdsArrayName_Key = "feature_ids_array_name";
  constexpr StringLiteral k_CellFeatureAttributeMatrixName_Key = "cell_feature_attribute_matrix_name";
  constexpr StringLiteral k_ActiveArrayName_Key = "active_array_name";
  constexpr StringLiteral k_RandomizeFeatures_Key = "randomize_features";

  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(k_MisorientationTolerance_Key, std::make_any<float32>(5.0F));
  args.insertOrAssign(k_UseGoodVoxels_Key, std::make_any<bool>(true));
  args.insertOrAssign(k_GridGeomPath_Key, std::make_any<DataPath>(Constants::k_DataContainerPath));
  args.insertOrAssign(k_QuatsArrayPath_Key, std::make_any<DataPath>(Constants::k_QuatsArrayPath));
  args.insertOrAssign(k_CellPhasesArrayPath_Key, std::make_any<DataPath>(Constants::k_PhasesArrayPath));
  args.insertOrAssign(k_GoodVoxelsPath_Key, std::make_any<DataPath>(Constants::k_MaskArrayPath));
  args.insertOrAssign(k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(Constants::k_CrystalStructuresArrayPath));
  args.insertOrAssign(k_FeatureIdsArrayName_Key, std::make_any<std::string>(Constants::k_FeatureIds));
  args.insertOrAssign(k_CellFeatureAttributeMatrixName_Key, std::make_any<std::string>(Constants::k_Grain_Data));
  args.insertOrAssign(k_ActiveArrayName_Key, std::make_any<std::string>(Constants::k_ActiveName));
  args.insertOrAssign(k_RandomizeFeatures_Key, std::make_any<bool>(false));

  // Preflight the filter and check result
  auto preflightResult = filter->preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

  // Execute the filter and check the result
  auto executeResult = filter->execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result)
}

//------------------------------------------------------------------------------
inline void ExecuteAlignSectionsMisorientation(DataStructure& dataStructure, const FilterList& filterList, const fs::path& shiftsFile)
{
  INFO(fmt::format("Error creating Filter '{}'  ", k_AlignSectionMisorientationFilterHandle.getFilterName()));
  auto filter = filterList.createFilter(k_AlignSectionMisorientationFilterHandle);
  REQUIRE(nullptr != filter);

  // Parameter Keys
  constexpr StringLiteral k_WriteAlignmentShifts_Key = "write_alignment_shifts";
  constexpr StringLiteral k_AlignmentShiftFileName_Key = "alignment_shift_file_name";

  constexpr StringLiteral k_MisorientationTolerance_Key = "misorientation_tolerance";

  constexpr StringLiteral k_GoodVoxels_Key = "use_good_voxels";
  constexpr StringLiteral k_GoodVoxelsArrayPath_Key = "good_voxels_array_path";

  constexpr StringLiteral k_QuatsArrayPath_Key = "quats_array_path";
  constexpr StringLiteral k_CellPhasesArrayPath_Key = "cell_phases_array_path";
  constexpr StringLiteral k_CrystalStructuresArrayPath_Key = "crystal_structures_array_path";

  constexpr StringLiteral k_SelectedImageGeometry_Key = "selected_image_geometry_path";

  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(k_WriteAlignmentShifts_Key, std::make_any<bool>(true));
  args.insertOrAssign(k_AlignmentShiftFileName_Key, std::make_any<FileSystemPathParameter::ValueType>(shiftsFile));

  args.insertOrAssign(k_MisorientationTolerance_Key, std::make_any<float32>(5.0F));

  args.insertOrAssign(k_GoodVoxels_Key, std::make_any<bool>(true));
  args.insertOrAssign(k_GoodVoxelsArrayPath_Key, std::make_any<DataPath>(Constants::k_MaskArrayPath));

  args.insertOrAssign(k_QuatsArrayPath_Key, std::make_any<DataPath>(Constants::k_QuatsArrayPath));
  args.insertOrAssign(k_CellPhasesArrayPath_Key, std::make_any<DataPath>(Constants::k_PhasesArrayPath));

  args.insertOrAssign(k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(Constants::k_CrystalStructuresArrayPath));

  args.insertOrAssign(k_SelectedImageGeometry_Key, std::make_any<DataPath>(Constants::k_DataContainerPath));

  // Preflight the filter and check result
  auto preflightResult = filter->preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

  // Execute the filter and check the result
  auto executeResult = filter->execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result)
}

//------------------------------------------------------------------------------
inline void ExecuteAlignSectionsFeatureCentroid(DataStructure& dataStructure, const FilterList& filterList, const fs::path& shiftsFile)
{
  INFO(fmt::format("Error creating Filter '{}'  ", k_AlignSectionsFeatureCentroidFilterHandle.getFilterName()));
  auto filter = filterList.createFilter(k_AlignSectionsFeatureCentroidFilterHandle);
  REQUIRE(nullptr != filter);

  // Parameter Keys
  constexpr StringLiteral k_WriteAlignmentShifts_Key = "write_alignment_shifts";
  constexpr StringLiteral k_AlignmentShiftFileName_Key = "alignment_shift_file_name";
  constexpr StringLiteral k_UseReferenceSlice_Key = "use_reference_slice";
  constexpr StringLiteral k_ReferenceSlice_Key = "reference_slice";
  constexpr StringLiteral k_GoodVoxelsArrayPath_Key = "good_voxels_array_path";
  constexpr StringLiteral k_SelectedImageGeometry_Key = "selected_image_geometry_path";
  constexpr StringLiteral k_SelectedCellDataGroup_Key = "selected_cell_data_path";

  Arguments args;
  // Create default Parameters for the filter.
  args.insertOrAssign(k_WriteAlignmentShifts_Key, std::make_any<bool>(true));
  args.insertOrAssign(k_AlignmentShiftFileName_Key, std::make_any<FileSystemPathParameter::ValueType>(shiftsFile));
  args.insertOrAssign(k_UseReferenceSlice_Key, std::make_any<bool>(true));
  args.insertOrAssign(k_ReferenceSlice_Key, std::make_any<int32>(0));
  args.insertOrAssign(k_GoodVoxelsArrayPath_Key, std::make_any<DataPath>(Constants::k_MaskArrayPath));
  args.insertOrAssign(k_SelectedImageGeometry_Key, std::make_any<DataPath>(Constants::k_DataContainerPath));
  args.insertOrAssign(k_SelectedCellDataGroup_Key, std::make_any<DataPath>(Constants::k_CellAttributeMatrix));

  // Preflight the filter and check result
  auto preflightResult = filter->preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

  // Execute the filter and check the result
  auto executeResult = filter->execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result)
}

//------------------------------------------------------------------------------
inline void ExecuteBadDataNeighborOrientationCheck(DataStructure& dataStructure, const FilterList& filterList)
{

  INFO(fmt::format("Error creating Filter '{}'  ", k_BadDataNeighborOrientationCheckFilterHandle.getFilterName()));
  auto filter = filterList.createFilter(k_BadDataNeighborOrientationCheckFilterHandle);
  REQUIRE(nullptr != filter);
  // Parameter Keys
  constexpr StringLiteral k_MisorientationTolerance_Key = "misorientation_tolerance";
  constexpr StringLiteral k_NumberOfNeighbors_Key = "number_of_neighbors";
  constexpr StringLiteral k_ImageGeometryPath_Key = "image_geometry_path";
  constexpr StringLiteral k_QuatsArrayPath_Key = "quats_array_path";
  constexpr StringLiteral k_GoodVoxelsArrayPath_Key = "good_voxels_array_path";
  constexpr StringLiteral k_CellPhasesArrayPath_Key = "cell_phases_array_path";
  constexpr StringLiteral k_CrystalStructuresArrayPath_Key = "crystal_structures_array_path";

  Arguments args;
  // Create default Parameters for the filter.
  args.insertOrAssign(k_MisorientationTolerance_Key, std::make_any<float32>(5.0f));
  args.insertOrAssign(k_NumberOfNeighbors_Key, std::make_any<int32>(4));
  args.insertOrAssign(k_ImageGeometryPath_Key, std::make_any<DataPath>(Constants::k_DataContainerPath));
  args.insertOrAssign(k_QuatsArrayPath_Key, std::make_any<DataPath>(Constants::k_QuatsArrayPath));
  args.insertOrAssign(k_GoodVoxelsArrayPath_Key, std::make_any<DataPath>(Constants::k_MaskArrayPath));
  args.insertOrAssign(k_CellPhasesArrayPath_Key, std::make_any<DataPath>(Constants::k_PhasesArrayPath));
  args.insertOrAssign(k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(Constants::k_CrystalStructuresArrayPath));

  // Preflight the filter and check result
  auto preflightResult = filter->preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

  // Execute the filter and check the result
  auto executeResult = filter->execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result)
}

//------------------------------------------------------------------------------
inline void ExecuteNeighborOrientationCorrelation(DataStructure& dataStructure, const FilterList& filterList)
{
  INFO(fmt::format("Error creating Filter '{}'  ", k_NeighborOrientationCorrelationFilterHandle.getFilterName()));
  auto filter = filterList.createFilter(k_NeighborOrientationCorrelationFilterHandle);
  REQUIRE(nullptr != filter);

  // Parameter Keys
  constexpr StringLiteral k_ImageGeometryPath_Key = "image_geometry_path";
  constexpr StringLiteral k_MinConfidence_Key = "min_confidence";
  constexpr StringLiteral k_MisorientationTolerance_Key = "misorientation_tolerance";
  constexpr StringLiteral k_Level_Key = "level";
  constexpr StringLiteral k_ConfidenceIndexArrayPath_Key = "confidence_index_array_path";
  constexpr StringLiteral k_CellPhasesArrayPath_Key = "cell_phases_array_path";
  constexpr StringLiteral k_QuatsArrayPath_Key = "quats_array_path";
  constexpr StringLiteral k_CrystalStructuresArrayPath_Key = "crystal_structures_array_path";
  constexpr StringLiteral k_IgnoredDataArrayPaths_Key = "ignored_data_array_paths";

  Arguments args;
  // Create default Parameters for the filter.
  args.insertOrAssign(k_ImageGeometryPath_Key, std::make_any<DataPath>(Constants::k_DataContainerPath));
  args.insertOrAssign(k_MinConfidence_Key, std::make_any<float32>(0.2f));
  args.insertOrAssign(k_MisorientationTolerance_Key, std::make_any<float32>(5.0f));
  args.insertOrAssign(k_Level_Key, std::make_any<int32>(2));
  args.insertOrAssign(k_ConfidenceIndexArrayPath_Key, std::make_any<DataPath>(Constants::k_ConfidenceIndexArrayPath));
  args.insertOrAssign(k_CellPhasesArrayPath_Key, std::make_any<DataPath>(Constants::k_PhasesArrayPath));
  args.insertOrAssign(k_QuatsArrayPath_Key, std::make_any<DataPath>(Constants::k_QuatsArrayPath));
  args.insertOrAssign(k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(Constants::k_CrystalStructuresArrayPath));
  args.insertOrAssign(k_IgnoredDataArrayPaths_Key, std::make_any<std::vector<DataPath>>());

  // Preflight the filter and check result
  auto preflightResult = filter->preflight(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

  // Execute the filter and check the result
  auto executeResult = filter->execute(dataStructure, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResult.result)
}
} // namespace SmallIn100
