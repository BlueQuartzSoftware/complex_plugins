#pragma once

#include <map>
#include <string>
// clang-format off
namespace complex
{
  static const std::map<complex::Uuid, complex::Uuid> k_SIMPL_to_OrientationAnalysis
  {
    // syntax std::make_pair {Dream3d UUID , Dream3dnx UUID}, // dream3d-class-name
    {complex::Uuid::FromString("4fb2b9de-3124-534b-b914-dbbbdbc14604").value(), complex::Uuid::FromString("8df2135c-7079-49f4-9756-4f3c028a5ced").value()}, // AlignSectionsMisorientation
    {complex::Uuid::FromString("f4a7c2df-e9b0-5da9-b745-a862666d6c99").value(), complex::Uuid::FromString("3f342977-aea1-49e1-a9c2-f73760eba0d3").value()}, // BadDataNeighborOrientationCheck
    {complex::Uuid::FromString("e5629880-98c4-5656-82b8-c9fe2b9744de").value(), complex::Uuid::FromString("501e54e6-a66f-4eeb-ae37-00e649c00d4b").value()}, // ConvertOrientations
    {complex::Uuid::FromString("7861c691-b821-537b-bd25-dc195578e0ea").value(), complex::Uuid::FromString("1810c2c7-63e3-41db-b204-a5821e6271c0").value()}, // EBSDSegmentFeatures
    {complex::Uuid::FromString("bf7036d8-25bd-540e-b6de-3a5ab0e42c5f").value(), complex::Uuid::FromString("086ddb9a-928f-46ab-bad6-b1498270d71e").value()}, // FindAvgOrientations
    {complex::Uuid::FromString("428e1f5b-e6d8-5e8b-ad68-56ff14ee0e8c").value(), complex::Uuid::FromString("24b54daf-3bf5-4331-93f6-03a49f719bf1").value()}, // FindFeatureReferenceMisorientations
    {complex::Uuid::FromString("88d332c1-cf6c-52d3-a38d-22f6eae19fa6").value(), complex::Uuid::FromString("61cfc9c1-aa0e-452b-b9ef-d3b9e6268035").value()}, // FindKernelAvgMisorientations
    {complex::Uuid::FromString("286dd493-4fea-54f4-b59e-459dd13bbe57").value(), complex::Uuid::FromString("0b68fe25-b5ef-4805-ae32-20acb8d4e823").value()}, // FindMisorientations
    {complex::Uuid::FromString("e67ca06a-176f-58fc-a676-d6ee5553511a").value(), complex::Uuid::FromString("b4681855-0a3d-4237-97f2-5aec509115c4").value()}, // FindSchmids
    {complex::Uuid::FromString("9a6677a6-b9e5-5fee-afa2-27e868cab8ca").value(), complex::Uuid::FromString("8b651407-08a9-4c25-967a-d86444eca87f").value()}, // GenerateFZQuaternions
    {complex::Uuid::FromString("a50e6532-8075-5de5-ab63-945feb0de7f7").value(), complex::Uuid::FromString("64cb4f27-6e5e-4dd2-8a03-0c448cb8f5e6").value()}, // GenerateIPFColors
    {complex::Uuid::FromString("c9af506e-9ea1-5ff5-a882-fa561def5f52").value(), complex::Uuid::FromString("f173786a-50cd-4c3c-9518-48ef6fc2bac9").value()}, // MergeTwins
    {complex::Uuid::FromString("6427cd5e-0ad2-5a24-8847-29f8e0720f4f").value(), complex::Uuid::FromString("4625c192-7e46-4333-a294-67a2eb64cb37").value()}, // NeighborOrientationCorrelation
    {complex::Uuid::FromString("b8e128a8-c2a3-5e6c-a7ad-e4fb864e5d40").value(), complex::Uuid::FromString("5b062816-79ac-47ce-93cb-e7966896bcbd").value()}, // ReadAngData
    {complex::Uuid::FromString("d1df969c-0428-53c3-b61d-99ea2bb6da28").value(), complex::Uuid::FromString("7751923c-afb9-4032-8372-8078325c69a4").value()}, // ReadCtfData
    {complex::Uuid::FromString("4ef7f56b-616e-5a80-9e68-1da8f35ad235").value(), complex::Uuid::FromString("db291b6c-9aaf-432d-9547-430c865a539c").value()}, // ReadH5Ebsd
    {complex::Uuid::FromString("ef9420b2-8c46-55f3-8ae4-f53790639de4").value(), complex::Uuid::FromString("0458edcd-3655-4465-adc8-b036d76138b5").value()}, // RotateEulerRefFrame
    {complex::Uuid::FromString("e1343abe-e5ad-5eb1-a89d-c209e620e4de").value(), complex::Uuid::FromString("fcdb311f-a70c-4bf8-9e83-9275b36bc57f").value()}, // ConvertHexGridToSquareGrid
    {complex::Uuid::FromString("439e31b7-3198-5d0d-aef6-65a9e9c1a016").value(), complex::Uuid::FromString("d6b51fb2-279d-41cd-ad1a-c7a5a3f4b1cc").value()}, // ConvertQuaternion
    {complex::Uuid::FromString("2a0bfcd3-2517-5117-b164-964dfac8fe22").value(), complex::Uuid::FromString("8ce3d70c-49fe-4812-a1eb-7ce4c962a59d").value()}, // CreateEnsembleInfo
    {complex::Uuid::FromString("c4398303-db7d-506e-81ea-08f253895ccb").value(), complex::Uuid::FromString("916e2881-4c1c-47bd-99b0-6fb183ecdcea").value()}, // CreateLambertSphere
    {complex::Uuid::FromString("6e332fca-0475-5fec-821e-e01f668ec1d3").value(), complex::Uuid::FromString("2d05ca72-0a1b-4aec-b9b0-bc470845c448").value()}, // EbsdToH5Ebsd
    {complex::Uuid::FromString("b78d8825-d3ac-5351-be20-172f07fd2aec").value(), complex::Uuid::FromString("74478e86-ce29-40b8-8c17-d20009195f91").value()}, // EMsoftSO3Sampler
    {complex::Uuid::FromString("33a37a47-d002-5c18-b270-86025881fe1e").value(), complex::Uuid::FromString("ecf1ec45-bc27-4ddb-b2c0-3d465b8c182a").value()}, // EnsembleInfoReader
    {complex::Uuid::FromString("c5a9a96c-7570-5279-b383-cc25ebae0046").value(), complex::Uuid::FromString("453cdb58-7bbb-4576-ad5e-f75a1c54d348").value()}, // FindAvgCAxes
    {complex::Uuid::FromString("8071facb-8905-5699-b345-105ae4ac33ff").value(), complex::Uuid::FromString("f39707f9-0411-4736-a591-1bfbd099bb71").value()}, // FindBoundaryStrengths
    {complex::Uuid::FromString("68ae7b7e-b9f7-5799-9f82-ce21d0ccd55e").value(), complex::Uuid::FromString("a51c257a-ddc1-499a-9b21-f2d25a19d098").value()}, // FindCAxisLocations
    {complex::Uuid::FromString("94f986fc-1295-5e32-9808-752855fa658a").value(), complex::Uuid::FromString("dbd14c09-f447-4086-b270-90919fce3f2e").value()}, // FindDistsToCharactGBs
    {complex::Uuid::FromString("cdd50b83-ea09-5499-b008-4b253cf4c246").value(), complex::Uuid::FromString("636ee030-9f07-4f16-a4f3-592eff8ef1ee").value()}, // FindFeatureNeighborCAxisMisalignments
    {complex::Uuid::FromString("1a0848da-2edd-52c0-b111-62a4dc6d2886").value(), complex::Uuid::FromString("16c487d2-8f99-4fb5-a4df-d3f70a8e6b25").value()}, // FindFeatureReferenceCAxisMisorientations
    {complex::Uuid::FromString("6e97ff50-48bf-5403-a049-1d271bd72df9").value(), complex::Uuid::FromString("0cc592b7-a3d3-4c6a-8894-1533670a31b9").value()}, // FindGBCD
    {complex::Uuid::FromString("d67e9f28-2fe5-5188-b0f8-323a7e603de6").value(), complex::Uuid::FromString("fc0d695a-e381-4f11-a8fd-26d5b5cda30a").value()}, // FindGBCDMetricBased
    {complex::Uuid::FromString("00d20627-5b88-56ba-ac7a-fc2a4b337903").value(), complex::Uuid::FromString("8267878b-d23a-4666-83a0-f1a7711861ce").value()}, // FindGBPDMetricBased
    {complex::Uuid::FromString("97523038-5fb2-5e82-9177-ed3e8b24b4bd").value(), complex::Uuid::FromString("7569d075-d05a-4e07-8660-5cca3e78ee49").value()}, // FindSlipTransmissionMetrics
    {complex::Uuid::FromString("a10124f3-05d0-5f49-93a0-e93926f5b48b").value(), complex::Uuid::FromString("7e6e8432-0fe0-42aa-a103-a47ece44cf66").value()}, // FindTwinBoundaries
    {complex::Uuid::FromString("b0e30e6d-912d-5a7e-aeed-750134aba86b").value(), complex::Uuid::FromString("19ad8363-40b5-45aa-829a-4406f35b00ce").value()}, // FindTwinBoundarySchmidFactors
    {complex::Uuid::FromString("0a121e03-3922-5c29-962d-40d88653f4b6").value(), complex::Uuid::FromString("30759600-7c02-4650-b5ca-e7036d6b568e").value()}, // GenerateFaceIPFColoring
    {complex::Uuid::FromString("7cd30864-7bcf-5c10-aea7-d107373e2d40").value(), complex::Uuid::FromString("f3473af9-db77-43db-bd25-60df7230ea73").value()}, // GenerateFaceMisorientationColoring
    {complex::Uuid::FromString("ec58f4fe-8e51-527e-9536-8b6f185684be").value(), complex::Uuid::FromString("bf7cc5a9-42b6-4147-8c56-de234cd6b949").value()}, // GenerateOrientationMatrixTranspose
    {complex::Uuid::FromString("630d7486-75ea-5e04-874c-894460cd7c4d").value(), complex::Uuid::FromString("e1653c62-3894-4b95-8449-605400038110").value()}, // GenerateQuaternionConjugate
    {complex::Uuid::FromString("179b0c7a-4e62-5070-ba49-ae58d5ccbfe8").value(), complex::Uuid::FromString("53b4d0b2-1aa3-4725-93df-f2857e491931").value()}, // ImportEbsdMontage
    {complex::Uuid::FromString("8abdea7d-f715-5a24-8165-7f946bbc2fe9").value(), complex::Uuid::FromString("694a0d54-b492-41bc-993b-ce0f5006e927").value()}, // ImportH5EspritData
    {complex::Uuid::FromString("3ff4701b-3a0c-52e3-910a-fa927aa6584c").value(), complex::Uuid::FromString("4ad3d47c-b1e1-4429-bc65-5e021be62ba0").value()}, // ImportH5OimData
    {complex::Uuid::FromString("27c724cc-8b69-5ebe-b90e-29d33858a032").value(), complex::Uuid::FromString("a5a37eb3-c143-4b38-a89b-e0641a20dc7e").value()}, // INLWriter
    {complex::Uuid::FromString("5af9c1e6-ed6f-5672-9ae0-2b931344d729").value(), complex::Uuid::FromString("cf06dcb7-fa92-4606-9716-28ede2cd1c62").value()}, // OrientationUtility
    {complex::Uuid::FromString("17410178-4e5f-58b9-900e-8194c69200ab").value(), complex::Uuid::FromString("65128c53-d3be-4a69-a559-32a48d603884").value()}, // ReplaceElementAttributesWithNeighborValues
    {complex::Uuid::FromString("a2b62395-1a7d-5058-a840-752d8f8e2430").value(), complex::Uuid::FromString("3704a3c6-dd09-4b91-9eda-2fbb4cb9bf99").value()}, // RodriguesConvertor
    {complex::Uuid::FromString("3630623e-724b-5154-a060-a5fca4ecfff5").value(), complex::Uuid::FromString("42ddcd69-dde3-4b08-b90c-6002079540eb").value()}, // Stereographic3D
    {complex::Uuid::FromString("a10bb78e-fcff-553d-97d6-830a43c85385").value(), complex::Uuid::FromString("00cbb97e-a5c2-43e6-9a35-17a0f9ce26ed").value()}, // WritePoleFigure
    {complex::Uuid::FromString("a4952f40-22dd-54ec-8c38-69c3fcd0e6f7").value(), complex::Uuid::FromString("aa6d399b-715e-44f1-9902-f1bd18faf1c5").value()}, // WriteStatsGenOdfAngleFile
    // {complex::Uuid::FromString(insert DREAM3D UUID string here).value(), complex::Uuid::FromString(insert DREAM3DNX UUID string here).value()}, // dream3d-class-name
  };

} // namespace complex
// clang-format on
