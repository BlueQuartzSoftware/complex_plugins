#pragma once

#include <map>
#include <string>

namespace complex
{
  // Regex to grep UUIDs : {\"([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})\",\s\"([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}(?:\s,\s[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})*)\"},\s\/\/\s(\w+)
  static const std::map<std::string, std::string> k_SIMPL_to_OrientationAnalysis
  {
    // syntax std::make_pair {Dream3d UUID , Dream3dnx UUID}, // dream3d-class-name
    {"4fb2b9de-3124-534b-b914-dbbbdbc14604", "8df2135c-7079-49f4-9756-4f3c028a5ced"}, // AlignSectionsMisorientation
    {"f4a7c2df-e9b0-5da9-b745-a862666d6c99", "3f342977-aea1-49e1-a9c2-f73760eba0d3"}, // BadDataNeighborOrientationCheck
    {"e5629880-98c4-5656-82b8-c9fe2b9744de", "501e54e6-a66f-4eeb-ae37-00e649c00d4b"}, // ConvertOrientations
    {"7861c691-b821-537b-bd25-dc195578e0ea", "1810c2c7-63e3-41db-b204-a5821e6271c0"}, // EBSDSegmentFeatures
    {"bf7036d8-25bd-540e-b6de-3a5ab0e42c5f", "086ddb9a-928f-46ab-bad6-b1498270d71e"}, // FindAvgOrientations
    {"428e1f5b-e6d8-5e8b-ad68-56ff14ee0e8c", "24b54daf-3bf5-4331-93f6-03a49f719bf1"}, // FindFeatureReferenceMisorientations
    {"88d332c1-cf6c-52d3-a38d-22f6eae19fa6", "61cfc9c1-aa0e-452b-b9ef-d3b9e6268035"}, // FindKernelAvgMisorientations
    {"286dd493-4fea-54f4-b59e-459dd13bbe57", "0b68fe25-b5ef-4805-ae32-20acb8d4e823"}, // FindMisorientations
    {"e67ca06a-176f-58fc-a676-d6ee5553511a", "b4681855-0a3d-4237-97f2-5aec509115c4"}, // FindSchmids
    {"9a6677a6-b9e5-5fee-afa2-27e868cab8ca", "8b651407-08a9-4c25-967a-d86444eca87f"}, // GenerateFZQuaternions
    {"a50e6532-8075-5de5-ab63-945feb0de7f7", "64cb4f27-6e5e-4dd2-8a03-0c448cb8f5e6"}, // GenerateIPFColors
    {"c9af506e-9ea1-5ff5-a882-fa561def5f52", "f173786a-50cd-4c3c-9518-48ef6fc2bac9"}, // MergeTwins
    {"6427cd5e-0ad2-5a24-8847-29f8e0720f4f", "4625c192-7e46-4333-a294-67a2eb64cb37"}, // NeighborOrientationCorrelation
    {"b8e128a8-c2a3-5e6c-a7ad-e4fb864e5d40", "5b062816-79ac-47ce-93cb-e7966896bcbd"}, // ReadAngData
    {"d1df969c-0428-53c3-b61d-99ea2bb6da28", "7751923c-afb9-4032-8372-8078325c69a4"}, // ReadCtfData
    {"4ef7f56b-616e-5a80-9e68-1da8f35ad235", "db291b6c-9aaf-432d-9547-430c865a539c"}, // ReadH5Ebsd
    {"ef9420b2-8c46-55f3-8ae4-f53790639de4", "0458edcd-3655-4465-adc8-b036d76138b5"}, // RotateEulerRefFrame
    {"e1343abe-e5ad-5eb1-a89d-c209e620e4de", "fcdb311f-a70c-4bf8-9e83-9275b36bc57f"}, // ConvertHexGridToSquareGrid
    {"439e31b7-3198-5d0d-aef6-65a9e9c1a016", "d6b51fb2-279d-41cd-ad1a-c7a5a3f4b1cc"}, // ConvertQuaternion
    {"2a0bfcd3-2517-5117-b164-964dfac8fe22", "8ce3d70c-49fe-4812-a1eb-7ce4c962a59d"}, // CreateEnsembleInfo
    {"c4398303-db7d-506e-81ea-08f253895ccb", "916e2881-4c1c-47bd-99b0-6fb183ecdcea"}, // CreateLambertSphere
    {"6e332fca-0475-5fec-821e-e01f668ec1d3", "2d05ca72-0a1b-4aec-b9b0-bc470845c448"}, // EbsdToH5Ebsd
    {"b78d8825-d3ac-5351-be20-172f07fd2aec", "74478e86-ce29-40b8-8c17-d20009195f91"}, // EMsoftSO3Sampler
    {"33a37a47-d002-5c18-b270-86025881fe1e", "ecf1ec45-bc27-4ddb-b2c0-3d465b8c182a"}, // EnsembleInfoReader
    {"c5a9a96c-7570-5279-b383-cc25ebae0046", "453cdb58-7bbb-4576-ad5e-f75a1c54d348"}, // FindAvgCAxes
    {"8071facb-8905-5699-b345-105ae4ac33ff", "f39707f9-0411-4736-a591-1bfbd099bb71"}, // FindBoundaryStrengths
    {"68ae7b7e-b9f7-5799-9f82-ce21d0ccd55e", "a51c257a-ddc1-499a-9b21-f2d25a19d098"}, // FindCAxisLocations
    {"94f986fc-1295-5e32-9808-752855fa658a", "dbd14c09-f447-4086-b270-90919fce3f2e"}, // FindDistsToCharactGBs
    {"cdd50b83-ea09-5499-b008-4b253cf4c246", "636ee030-9f07-4f16-a4f3-592eff8ef1ee"}, // FindFeatureNeighborCAxisMisalignments
    {"1a0848da-2edd-52c0-b111-62a4dc6d2886", "16c487d2-8f99-4fb5-a4df-d3f70a8e6b25"}, // FindFeatureReferenceCAxisMisorientations
    {"6e97ff50-48bf-5403-a049-1d271bd72df9", "0cc592b7-a3d3-4c6a-8894-1533670a31b9"}, // FindGBCD
    {"d67e9f28-2fe5-5188-b0f8-323a7e603de6", "fc0d695a-e381-4f11-a8fd-26d5b5cda30a"}, // FindGBCDMetricBased
    {"00d20627-5b88-56ba-ac7a-fc2a4b337903", "8267878b-d23a-4666-83a0-f1a7711861ce"}, // FindGBPDMetricBased
    {"97523038-5fb2-5e82-9177-ed3e8b24b4bd", "7569d075-d05a-4e07-8660-5cca3e78ee49"}, // FindSlipTransmissionMetrics
    {"a10124f3-05d0-5f49-93a0-e93926f5b48b", "7e6e8432-0fe0-42aa-a103-a47ece44cf66"}, // FindTwinBoundaries
    {"b0e30e6d-912d-5a7e-aeed-750134aba86b", "19ad8363-40b5-45aa-829a-4406f35b00ce"}, // FindTwinBoundarySchmidFactors
    {"0a121e03-3922-5c29-962d-40d88653f4b6", "30759600-7c02-4650-b5ca-e7036d6b568e"}, // GenerateFaceIPFColoring
    {"7cd30864-7bcf-5c10-aea7-d107373e2d40", "f3473af9-db77-43db-bd25-60df7230ea73"}, // GenerateFaceMisorientationColoring
    {"ec58f4fe-8e51-527e-9536-8b6f185684be", "bf7cc5a9-42b6-4147-8c56-de234cd6b949"}, // GenerateOrientationMatrixTranspose
    {"630d7486-75ea-5e04-874c-894460cd7c4d", "e1653c62-3894-4b95-8449-605400038110"}, // GenerateQuaternionConjugate
    {"179b0c7a-4e62-5070-ba49-ae58d5ccbfe8", "53b4d0b2-1aa3-4725-93df-f2857e491931"}, // ImportEbsdMontage
    {"8abdea7d-f715-5a24-8165-7f946bbc2fe9", "694a0d54-b492-41bc-993b-ce0f5006e927"}, // ImportH5EspritData
    {"3ff4701b-3a0c-52e3-910a-fa927aa6584c", "4ad3d47c-b1e1-4429-bc65-5e021be62ba0"}, // ImportH5OimData
    {"27c724cc-8b69-5ebe-b90e-29d33858a032", "a5a37eb3-c143-4b38-a89b-e0641a20dc7e"}, // INLWriter
    {"5af9c1e6-ed6f-5672-9ae0-2b931344d729", "cf06dcb7-fa92-4606-9716-28ede2cd1c62"}, // OrientationUtility
    {"17410178-4e5f-58b9-900e-8194c69200ab", "65128c53-d3be-4a69-a559-32a48d603884"}, // ReplaceElementAttributesWithNeighborValues
    {"a2b62395-1a7d-5058-a840-752d8f8e2430", "3704a3c6-dd09-4b91-9eda-2fbb4cb9bf99"}, // RodriguesConvertor
    {"3630623e-724b-5154-a060-a5fca4ecfff5", "42ddcd69-dde3-4b08-b90c-6002079540eb"}, // Stereographic3D
    {"a10bb78e-fcff-553d-97d6-830a43c85385", "00cbb97e-a5c2-43e6-9a35-17a0f9ce26ed"}, // WritePoleFigure
    {"a4952f40-22dd-54ec-8c38-69c3fcd0e6f7", "aa6d399b-715e-44f1-9902-f1bd18faf1c5"}, // WriteStatsGenOdfAngleFile
    // {insert DREAM3D UUID here, insert DREAM3DNX UUID here}, // dream3d-class-name
  };

  static const std::map<std::string, std::string> k_OrientationAnalysis_to_SIMPL
  {
    // syntax std::make_pair {Dream3dnx UUID , Dream3d UUID}, // dream3dnx-class-name
    {"8df2135c-7079-49f4-9756-4f3c028a5ced", "4fb2b9de-3124-534b-b914-dbbbdbc14604"}, // AlignSectionsMisorientationFilter
    {"3f342977-aea1-49e1-a9c2-f73760eba0d3", "f4a7c2df-e9b0-5da9-b745-a862666d6c99"}, // BadDataNeighborOrientationCheckFilter
    {"501e54e6-a66f-4eeb-ae37-00e649c00d4b", "e5629880-98c4-5656-82b8-c9fe2b9744de"}, // ConvertOrientations
    {"1810c2c7-63e3-41db-b204-a5821e6271c0", "7861c691-b821-537b-bd25-dc195578e0ea"}, // EBSDSegmentFeaturesFilter
    {"086ddb9a-928f-46ab-bad6-b1498270d71e", "bf7036d8-25bd-540e-b6de-3a5ab0e42c5f"}, // FindAvgOrientationsFilter
    {"24b54daf-3bf5-4331-93f6-03a49f719bf1", "428e1f5b-e6d8-5e8b-ad68-56ff14ee0e8c"}, // FindFeatureReferenceMisorientationsFilter
    {"61cfc9c1-aa0e-452b-b9ef-d3b9e6268035", "88d332c1-cf6c-52d3-a38d-22f6eae19fa6"}, // FindKernelAvgMisorientationsFilter
    {"0b68fe25-b5ef-4805-ae32-20acb8d4e823", "286dd493-4fea-54f4-b59e-459dd13bbe57"}, // FindMisorientationsFilter
    {"b4681855-0a3d-4237-97f2-5aec509115c4", "e67ca06a-176f-58fc-a676-d6ee5553511a"}, // FindSchmidsFilter
    {"8b651407-08a9-4c25-967a-d86444eca87f", "9a6677a6-b9e5-5fee-afa2-27e868cab8ca"}, // GenerateFZQuaternions
    {"64cb4f27-6e5e-4dd2-8a03-0c448cb8f5e6", "a50e6532-8075-5de5-ab63-945feb0de7f7"}, // GenerateIPFColorsFilter
    {"f173786a-50cd-4c3c-9518-48ef6fc2bac9", "c9af506e-9ea1-5ff5-a882-fa561def5f52"}, // MergeTwinsFilter
    {"4625c192-7e46-4333-a294-67a2eb64cb37", "6427cd5e-0ad2-5a24-8847-29f8e0720f4f"}, // NeighborOrientationCorrelationFilter
    {"5b062816-79ac-47ce-93cb-e7966896bcbd", "b8e128a8-c2a3-5e6c-a7ad-e4fb864e5d40"}, // ReadAngDataFilter
    {"7751923c-afb9-4032-8372-8078325c69a4", "d1df969c-0428-53c3-b61d-99ea2bb6da28"}, // ReadCtfDataFilter
    {"db291b6c-9aaf-432d-9547-430c865a539c", "4ef7f56b-616e-5a80-9e68-1da8f35ad235"}, // ReadH5EbsdFilter
    {"0458edcd-3655-4465-adc8-b036d76138b5", "ef9420b2-8c46-55f3-8ae4-f53790639de4"}, // RotateEulerRefFrameFilter
    {"fcdb311f-a70c-4bf8-9e83-9275b36bc57f", "e1343abe-e5ad-5eb1-a89d-c209e620e4de"}, // ConvertHexGridToSquareGrid
    {"d6b51fb2-279d-41cd-ad1a-c7a5a3f4b1cc", "439e31b7-3198-5d0d-aef6-65a9e9c1a016"}, // ConvertQuaternion
    {"8ce3d70c-49fe-4812-a1eb-7ce4c962a59d", "2a0bfcd3-2517-5117-b164-964dfac8fe22"}, // CreateEnsembleInfo
    {"916e2881-4c1c-47bd-99b0-6fb183ecdcea", "c4398303-db7d-506e-81ea-08f253895ccb"}, // CreateLambertSphere
    {"2d05ca72-0a1b-4aec-b9b0-bc470845c448", "6e332fca-0475-5fec-821e-e01f668ec1d3"}, // EbsdToH5Ebsd
    {"74478e86-ce29-40b8-8c17-d20009195f91", "b78d8825-d3ac-5351-be20-172f07fd2aec"}, // EMsoftSO3Sampler
    {"ecf1ec45-bc27-4ddb-b2c0-3d465b8c182a", "33a37a47-d002-5c18-b270-86025881fe1e"}, // EnsembleInfoReader
    {"453cdb58-7bbb-4576-ad5e-f75a1c54d348", "c5a9a96c-7570-5279-b383-cc25ebae0046"}, // FindAvgCAxes
    {"f39707f9-0411-4736-a591-1bfbd099bb71", "8071facb-8905-5699-b345-105ae4ac33ff"}, // FindBoundaryStrengths
    {"a51c257a-ddc1-499a-9b21-f2d25a19d098", "68ae7b7e-b9f7-5799-9f82-ce21d0ccd55e"}, // FindCAxisLocations
    {"dbd14c09-f447-4086-b270-90919fce3f2e", "94f986fc-1295-5e32-9808-752855fa658a"}, // FindDistsToCharactGBs
    {"636ee030-9f07-4f16-a4f3-592eff8ef1ee", "cdd50b83-ea09-5499-b008-4b253cf4c246"}, // FindFeatureNeighborCAxisMisalignments
    {"16c487d2-8f99-4fb5-a4df-d3f70a8e6b25", "1a0848da-2edd-52c0-b111-62a4dc6d2886"}, // FindFeatureReferenceCAxisMisorientations
    {"0cc592b7-a3d3-4c6a-8894-1533670a31b9", "6e97ff50-48bf-5403-a049-1d271bd72df9"}, // FindGBCD
    {"fc0d695a-e381-4f11-a8fd-26d5b5cda30a", "d67e9f28-2fe5-5188-b0f8-323a7e603de6"}, // FindGBCDMetricBased
    {"8267878b-d23a-4666-83a0-f1a7711861ce", "00d20627-5b88-56ba-ac7a-fc2a4b337903"}, // FindGBPDMetricBased
    {"7569d075-d05a-4e07-8660-5cca3e78ee49", "97523038-5fb2-5e82-9177-ed3e8b24b4bd"}, // FindSlipTransmissionMetrics
    {"7e6e8432-0fe0-42aa-a103-a47ece44cf66", "a10124f3-05d0-5f49-93a0-e93926f5b48b"}, // FindTwinBoundaries
    {"19ad8363-40b5-45aa-829a-4406f35b00ce", "b0e30e6d-912d-5a7e-aeed-750134aba86b"}, // FindTwinBoundarySchmidFactors
    {"30759600-7c02-4650-b5ca-e7036d6b568e", "0a121e03-3922-5c29-962d-40d88653f4b6"}, // GenerateFaceIPFColoring
    {"f3473af9-db77-43db-bd25-60df7230ea73", "7cd30864-7bcf-5c10-aea7-d107373e2d40"}, // GenerateFaceMisorientationColoring
    {"bf7cc5a9-42b6-4147-8c56-de234cd6b949", "ec58f4fe-8e51-527e-9536-8b6f185684be"}, // GenerateOrientationMatrixTranspose
    {"e1653c62-3894-4b95-8449-605400038110", "630d7486-75ea-5e04-874c-894460cd7c4d"}, // GenerateQuaternionConjugate
    {"53b4d0b2-1aa3-4725-93df-f2857e491931", "179b0c7a-4e62-5070-ba49-ae58d5ccbfe8"}, // ImportEbsdMontage
    {"694a0d54-b492-41bc-993b-ce0f5006e927", "8abdea7d-f715-5a24-8165-7f946bbc2fe9"}, // ImportH5EspritData
    {"4ad3d47c-b1e1-4429-bc65-5e021be62ba0", "3ff4701b-3a0c-52e3-910a-fa927aa6584c"}, // ImportH5OimData
    {"a5a37eb3-c143-4b38-a89b-e0641a20dc7e", "27c724cc-8b69-5ebe-b90e-29d33858a032"}, // INLWriter
    {"cf06dcb7-fa92-4606-9716-28ede2cd1c62", "5af9c1e6-ed6f-5672-9ae0-2b931344d729"}, // OrientationUtility
    {"65128c53-d3be-4a69-a559-32a48d603884", "17410178-4e5f-58b9-900e-8194c69200ab"}, // ReplaceElementAttributesWithNeighborValues
    {"3704a3c6-dd09-4b91-9eda-2fbb4cb9bf99", "a2b62395-1a7d-5058-a840-752d8f8e2430"}, // RodriguesConvertor
    {"42ddcd69-dde3-4b08-b90c-6002079540eb", "3630623e-724b-5154-a060-a5fca4ecfff5"}, // Stereographic3D
    {"00cbb97e-a5c2-43e6-9a35-17a0f9ce26ed", "a10bb78e-fcff-553d-97d6-830a43c85385"}, // WritePoleFigure
    {"aa6d399b-715e-44f1-9902-f1bd18faf1c5", "a4952f40-22dd-54ec-8c38-69c3fcd0e6f7"}, // WriteStatsGenOdfAngleFile
    // {insert DREAM3DNX UUID here, insert DREAM3D UUID here}, // dream3dnx-class-name
  };

} // namespace complex