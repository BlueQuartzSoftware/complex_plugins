#pragma once

#include <map>
#include <string>
// clang-format off
namespace complex
{
  static const std::map<complex::Uuid, complex::Uuid> k_SIMPL_to_ITKImageProcessing
  {
    // syntax std::make_pair {Dream3d UUID , Dream3dnx UUID}, // dream3d-class-name
    {complex::Uuid::FromString("09f45c29-1cfb-566c-b3ae-d832b4f95905").value(), complex::Uuid::FromString("e9dd12bc-f7fa-4ba2-98b0-fec3326bf620").value()}, // ITKAbsImage
    {complex::Uuid::FromString("b09ec654-87a5-5dfa-9949-aa69f1fbfdd1").value(), complex::Uuid::FromString("e7411c44-95ab-4623-8bf4-59b63d2d08c5").value()}, // ITKAcosImage
    {complex::Uuid::FromString("2d5a7599-5e01-5489-a107-23b704d2b5eb").value(), complex::Uuid::FromString("ea3e7439-8327-4190-8ff7-49ecc321718f").value()}, // ITKAdaptiveHistogramEqualizationImage
    {complex::Uuid::FromString("79509ab1-24e1-50e4-9351-c5ce7cd87a72").value(), complex::Uuid::FromString("1b463492-041f-4680-abb1-0b94a3019063").value()}, // ITKAsinImage
    {complex::Uuid::FromString("e938569d-3644-5d00-a4e0-ab327937457d").value(), complex::Uuid::FromString("39933f50-088c-46ac-a421-d238f1b178fd").value()}, // ITKAtanImage
    {complex::Uuid::FromString("3c451ac9-bfef-5e41-bae9-3957a0fc26a1").value(), complex::Uuid::FromString("ed214e76-6954-49b4-817b-13f92315e722").value()}, // ITKBinaryContourImage
    {complex::Uuid::FromString("ba8a3f2e-3963-57c0-a8da-239e25de0526").value(), complex::Uuid::FromString("ba2494b0-c4f0-43ff-9d08-900395900e0c").value()}, // ITKBinaryThresholdImage
    {complex::Uuid::FromString("99a7aa3c-f945-5e77-875a-23b5231ab3f4").value(), complex::Uuid::FromString("b5ff32a8-e799-4f72-8d13-e2581f748562").value()}, // ITKClosingByReconstructionImage
    {complex::Uuid::FromString("2c2d7bf6-1e78-52e6-80aa-58b504ce0912").value(), complex::Uuid::FromString("6fe37f77-ceae-4839-9cf6-3ca7a70e14d0").value()}, // ITKCosImage
    {complex::Uuid::FromString("3aa99151-e722-51a0-90ba-71e93347ab09").value(), complex::Uuid::FromString("719df7b2-8db2-43eb-a40c-a015982eec08").value()}, // ITKGradientMagnitudeImage
    {complex::Uuid::FromString("fd688b32-d90e-5945-905b-2b7187b46265").value(), complex::Uuid::FromString("0ac5ed47-963e-4c54-b33d-967e0aa6a621").value()}, // ITKGradientMagnitudeRecursiveGaussianImage
    {complex::Uuid::FromString("54c8dd45-88c4-5d4b-8a39-e3cc595e1cf8").value(), complex::Uuid::FromString("1f1dd9e4-d361-432b-a22b-5535664ee545").value()}, // ITKGrayscaleFillholeImage
    {complex::Uuid::FromString("653b7b5c-03cb-5b32-8c3e-3637745e5ff6").value(), complex::Uuid::FromString("d72eaf98-9b1d-44c9-88f2-a5c3cf57b4f2").value()}, // ITKImageReader
    {complex::Uuid::FromString("11473711-f94d-5d96-b749-ec36a81ad338").value(), complex::Uuid::FromString("a181ee3e-1678-4133-b9c5-a9dd7bfec62f").value()}, // ITKImageWriter
    {complex::Uuid::FromString("cf7d7497-9573-5102-bedd-38f86a6cdfd4").value(), complex::Uuid::FromString("dcf980b7-ecca-46d1-af31-ac65f6e3b6bb").value()}, // ITKImportImageStack
    {complex::Uuid::FromString("c6e10fa5-5462-546b-b34b-0f0ea75a7e43").value(), complex::Uuid::FromString("9958d587-5698-4ea5-b8ea-fb71428b5d02").value()}, // ITKInvertIntensityImage
    {complex::Uuid::FromString("dbfd1a57-2a17-572d-93a7-8fd2f8e92eb0").value(), complex::Uuid::FromString("900ca377-e79d-4b54-b298-33d518238099").value()}, // ITKLog10Image
    {complex::Uuid::FromString("69aba77c-9a35-5251-a18a-e3728ddd2963").value(), complex::Uuid::FromString("4b6655ad-4e6c-4e68-a771-55ca0ae40915").value()}, // ITKLogImage
    {complex::Uuid::FromString("97102d65-9c32-576a-9177-c59d958bad10").value(), complex::Uuid::FromString("d3138266-3f34-4d6e-8e21-904c94351293").value()}, // ITKMaskImage
    {complex::Uuid::FromString("cc27ee9a-9946-56ad-afd4-6e98b71f417d").value(), complex::Uuid::FromString("a60ca165-59ac-486b-b4b4-0f0c24d80af8").value()}, // ITKMedianImage
    {complex::Uuid::FromString("b2248340-a371-5899-90a2-86047950f0a2").value(), complex::Uuid::FromString("f70337e5-4435-41f7-aecc-d79b4b1faccd").value()}, // ITKMorphologicalWatershedImage
    {complex::Uuid::FromString("5b905619-c46b-5690-b6fa-8e97cf4537b8").value(), complex::Uuid::FromString("9d8ce30e-c75e-4ca8-b6be-0b11baa7e6ce").value()}, // ITKNormalizeImage
    {complex::Uuid::FromString("ca04004f-fb11-588d-9f77-d00b3ee9ad2a").value(), complex::Uuid::FromString("c4225a23-0b23-4782-b509-296fb39a672b").value()}, // ITKOpeningByReconstructionImage
    {complex::Uuid::FromString("6e66563a-edcf-5e11-bc1d-ceed36d8493f").value(), complex::Uuid::FromString("30f37bcd-701f-4e64-aa9d-1181469d3fb5").value()}, // ITKOtsuMultipleThresholdsImage
    {complex::Uuid::FromString("bb15d42a-3077-582a-be1a-76b2bae172e9").value(), complex::Uuid::FromString("e81f72d3-e806-4afe-ab4c-795c6a3f526f").value()}, // ITKSignedMaurerDistanceMapImage
    {complex::Uuid::FromString("1eb4b4f7-1704-58e4-9f78-8726a5c8c302").value(), complex::Uuid::FromString("06c76c7a-c384-44be-bd01-6fd58070cd65").value()}, // ITKSinImage
    {complex::Uuid::FromString("8087dcad-68f2-598b-9670-d0f57647a445").value(), complex::Uuid::FromString("05c7c812-4e33-4e9a-bf27-d4c17f5dff68").value()}, // ITKSqrtImage
    {complex::Uuid::FromString("672810d9-5ec0-59c1-a209-8fb56c7a018a").value(), complex::Uuid::FromString("7cf3c08e-1af1-4540-aa08-4488a74923fc").value()}, // ITKTanImage
    {complex::Uuid::FromString("7c2a7c4e-4582-52a6-92de-16b626d43fbf").value(), complex::Uuid::FromString("f6bcb859-3cb1-4fab-a520-8fcc21be9a0a").value()}, // ApplyDewarpParameters
    {complex::Uuid::FromString("30687f44-9c10-5617-bcb8-4109cbd6e55e").value(), complex::Uuid::FromString("367f8491-1189-48fa-b297-db170fb87259").value()}, // AxioVisionV4ToTileConfiguration
    {complex::Uuid::FromString("5394f60c-b0f0-5f98-85da-3b5730109060").value(), complex::Uuid::FromString("bd12a81d-730c-41d6-8dad-05d782d2e2d7").value()}, // CalcDewarpParameters
    {complex::Uuid::FromString("a48f7a51-0ca9-584f-a0ca-4bfebdc41d7c").value(), complex::Uuid::FromString("95866cba-a92e-4d87-b7f6-8f03928ee633").value()}, // IlluminationCorrection
    {complex::Uuid::FromString("411b008c-006f-51b2-ba05-99e51a01af3c").value(), complex::Uuid::FromString("697f306e-9acd-4aba-b2c9-486acda269d4").value()}, // ImportAxioVisionV4Montage
    {complex::Uuid::FromString("c5474cd1-bea9-5a33-a0df-516e5735bab4").value(), complex::Uuid::FromString("290bb407-f685-4b93-858f-7bcfcdfb679b").value()}, // ImportVectorImageStack
    {complex::Uuid::FromString("774ec947-eed6-5eb2-a01b-ee6470e61521").value(), complex::Uuid::FromString("e11e4cb3-5e21-4130-873b-3f14d0c2c4a3").value()}, // ImportZenInfoMontage
    {complex::Uuid::FromString("5878723b-cc16-5486-ac5f-ff0107107e74").value(), complex::Uuid::FromString("b3a528aa-3029-4f9f-956b-b1d647e620a8").value()}, // ITKImportFijiMontage
    {complex::Uuid::FromString("cdb130af-3616-57b1-be59-fe18113b2621").value(), complex::Uuid::FromString("56b6e862-13f4-4c90-87e9-ec76db1c584b").value()}, // ITKImportRoboMetMontage
    {complex::Uuid::FromString("49b5feb1-ec05-5a26-af25-00053151d944").value(), complex::Uuid::FromString("6cf74e33-0a8d-402d-b3ee-b3b2f3cda07e").value()}, // ITKRefineTileCoordinates
    {complex::Uuid::FromString("fa4efd40-f4a6-5524-9fc6-e1f8bbb2c42f").value(), complex::Uuid::FromString("0417e934-1294-4387-9ef5-53c19d7d40b0").value()}, // ITKStitchMontage
    {complex::Uuid::FromString("066712e3-0378-566e-8236-1796c88d5e02").value(), complex::Uuid::FromString("6ccb887c-79d6-4e20-a09b-c79b0bb6a875").value()}, // ITKApproximateSignedDistanceMapImage
    {complex::Uuid::FromString("18ab754c-3219-59c8-928e-5fb4a09174e0").value(), complex::Uuid::FromString("2c2351bc-f39f-43d7-ae02-2ec14be1dada").value()}, // ITKBilateralImage
    {complex::Uuid::FromString("0cf0698d-65eb-58bb-9f89-51e875432197").value(), complex::Uuid::FromString("ba8535b3-3762-44a3-b147-45642c5634fc").value()}, // ITKBinaryClosingByReconstructionImage
    {complex::Uuid::FromString("f86167ad-a1a1-557b-97ea-92a3618baa8f").value(), complex::Uuid::FromString("83b8f253-dfa8-4195-9349-6fb89539ea1a").value()}, // ITKBinaryDilateImage
    {complex::Uuid::FromString("522c5249-c048-579a-98dd-f7aadafc5578").value(), complex::Uuid::FromString("fcb447ba-c875-4c74-8341-89ba9657a772").value()}, // ITKBinaryErodeImage
    {complex::Uuid::FromString("1d8deea7-c6d0-5fa1-95cb-b14f19df97e8").value(), complex::Uuid::FromString("abb27e0c-b049-4f60-8355-178d86bb1de4").value()}, // ITKBinaryMorphologicalClosingImage
    {complex::Uuid::FromString("704c801a-7549-54c4-9def-c4bb58d07fd1").value(), complex::Uuid::FromString("61e849cd-6781-4949-9edf-893145404130").value()}, // ITKBinaryMorphologicalOpeningImage
    {complex::Uuid::FromString("bd1c2353-0a39-52c0-902b-ee64721994c7").value(), complex::Uuid::FromString("b288d716-5763-4461-b020-22c1f6dbf49a").value()}, // ITKBinaryOpeningByReconstructionImage
    {complex::Uuid::FromString("606c3700-f793-5852-9a0f-3123bd212447").value(), complex::Uuid::FromString("f0568385-071f-4667-9ce1-0b7e497836d7").value()}, // ITKBinaryProjectionImage
    {complex::Uuid::FromString("dcceeb50-5924-5eae-88ea-34793cf545a9").value(), complex::Uuid::FromString("8c9d384f-2900-45b8-85a4-4b2af64a79f4").value()}, // ITKBinaryThinningImage
    {complex::Uuid::FromString("4f51765f-ee36-52d0-96b6-f2aca3c24e7c").value(), complex::Uuid::FromString("cb5c5a3c-7cf8-48e4-8d50-bbd2566bd055").value()}, // ITKBinomialBlurImage
    {complex::Uuid::FromString("e26e7359-f72c-5924-b42e-dd5dd454a794").value(), complex::Uuid::FromString("79427abf-1d77-423b-8985-85d3be32926a").value()}, // ITKBlackTopHatImage
    {complex::Uuid::FromString("17f60a64-de93-59aa-a5e2-063e0aadafb7").value(), complex::Uuid::FromString("5da5d0a4-35da-42e9-8604-7f318e2713fa").value()}, // ITKBoundedReciprocalImage
    {complex::Uuid::FromString("bf554dd5-a927-5969-b651-1c47d386afce").value(), complex::Uuid::FromString("57f68c9b-d2c4-4241-8b10-9b2c46b37797").value()}, // ITKConnectedComponentImage
    {complex::Uuid::FromString("009fb2d0-6f65-5406-bb2a-4a883d0bc18c").value(), complex::Uuid::FromString("ae839936-b2a1-4911-911b-54d2e18d873f").value()}, // ITKCurvatureAnisotropicDiffusionImage
    {complex::Uuid::FromString("653f26dd-a5ef-5c75-b6f6-bc096268f25e").value(), complex::Uuid::FromString("3f47b7a4-dfc0-4f3c-b696-ce5dc50f536f").value()}, // ITKCurvatureFlowImage
    {complex::Uuid::FromString("53d5b289-a716-559b-89d9-5ebb34f714ca").value(), complex::Uuid::FromString("4e761f2f-c14c-4309-9610-cb140f352cd1").value()}, // ITKDanielssonDistanceMapImage
    {complex::Uuid::FromString("dbf29c6d-461c-55e7-a6c4-56477d9da55b").value(), complex::Uuid::FromString("559a01c2-7202-43db-9a5c-5b82c4592985").value()}, // ITKDilateObjectMorphologyImage
    {complex::Uuid::FromString("7fcfa231-519e-58da-bf8f-ad0f728bf0fe").value(), complex::Uuid::FromString("3b0dcf6b-6e81-44ff-876d-2430461809cd").value()}, // ITKDoubleThresholdImage
    {complex::Uuid::FromString("caea0698-4253-518b-ab3f-8ebc140d92ea").value(), complex::Uuid::FromString("b8fd081c-e3ff-4559-b5fa-75d25f713d26").value()}, // ITKErodeObjectMorphologyImage
    {complex::Uuid::FromString("a6fb3f3a-6c7a-5dfc-a4f1-75ff1d62c32f").value(), complex::Uuid::FromString("a07e0cd3-1601-40df-9d2c-55011ba2b618").value()}, // ITKExpImage
    {complex::Uuid::FromString("634c2306-c1ee-5a45-a55c-f8286e36999a").value(), complex::Uuid::FromString("d928e692-3cb8-4026-a576-ec5724d42ac3").value()}, // ITKExpNegativeImage
    {complex::Uuid::FromString("98d0b50b-9639-53a0-9e30-2fae6f7ab869").value(), complex::Uuid::FromString("faf31fd6-952b-4846-aa5c-78a599e6bda4").value()}, // ITKGradientAnisotropicDiffusionImage
    {complex::Uuid::FromString("66cec151-2950-51f8-8a02-47d3516d8721").value(), complex::Uuid::FromString("798dc1cf-fe06-4702-a87c-29ea45481d7d").value()}, // ITKGrayscaleDilateImage
    {complex::Uuid::FromString("aef4e804-3f7a-5dc0-911c-b1f16a393a69").value(), complex::Uuid::FromString("fdf31421-7081-48fc-8cc0-3b2020428fad").value()}, // ITKGrayscaleErodeImage
    {complex::Uuid::FromString("d910551f-4eec-55c9-b0ce-69c2277e61bd").value(), complex::Uuid::FromString("d4cccf34-ad72-4b5e-a805-e5d6f0a6b2e6").value()}, // ITKGrayscaleGrindPeakImage
    {complex::Uuid::FromString("849a1903-5595-5029-bbde-6f4b68b2a25c").value(), complex::Uuid::FromString("4c95856b-5d79-47d7-b8d7-9e5cad00a98d").value()}, // ITKGrayscaleMorphologicalClosingImage
    {complex::Uuid::FromString("c88ac42b-9477-5088-9ec0-862af1e0bb56").value(), complex::Uuid::FromString("fdcb0dd8-9103-4f9d-920e-9f8059d22fa0").value()}, // ITKGrayscaleMorphologicalOpeningImage
    {complex::Uuid::FromString("8bc34707-04c0-5e83-8583-48ee19306a1d").value(), complex::Uuid::FromString("691d1953-f161-417c-9c4e-207ddf63d6aa").value()}, // ITKHConvexImage
    {complex::Uuid::FromString("932a6df4-212e-53a1-a2ab-c29bd376bb7b").value(), complex::Uuid::FromString("fc8c9fb6-b297-4b30-907f-14b5d5c2b336").value()}, // ITKHMaximaImage
    {complex::Uuid::FromString("f1d7cf59-9b7c-53cb-b71a-76cf91c86e8f").value(), complex::Uuid::FromString("2aed53ed-3875-4230-aeb8-448cc924c080").value()}, // ITKHMinimaImage
    {complex::Uuid::FromString("4faf4c59-6f29-53af-bc78-5aecffce0e37").value(), complex::Uuid::FromString("c291a322-f9be-4675-a24d-dfcc20f41eba").value()}, // ITKIntensityWindowingImage
    {complex::Uuid::FromString("e9952e0e-97e4-53aa-8c6c-115fd18f0b89").value(), complex::Uuid::FromString("fc68ef8d-ab37-4b1d-b336-0a9b12fb81e3").value()}, // ITKIsoContourDistanceImage
    {complex::Uuid::FromString("668f0b90-b504-5fba-b648-7c9677e1f452").value(), complex::Uuid::FromString("c681e527-0e80-4ec6-af1f-fea84cb6e454").value()}, // ITKLabelContourImage
    {complex::Uuid::FromString("9677659d-b08c-58a4-ac4d-fba7d9093454").value(), complex::Uuid::FromString("05718743-53e8-4992-afda-6ac521ab687a").value()}, // ITKLaplacianRecursiveGaussianImage
    {complex::Uuid::FromString("c4963181-c788-5efc-8560-d005a5e01eea").value(), complex::Uuid::FromString("25e0f394-0a80-4401-9560-9eae84b00c1b").value()}, // ITKLaplacianSharpeningImage
    {complex::Uuid::FromString("b2cb7ad7-6f62-51c4-943d-54d19c64e7be").value(), complex::Uuid::FromString("26f6de2f-2bac-4b82-a2cd-318e0198b620").value()}, // ITKMaximumProjectionImage
    {complex::Uuid::FromString("85c061bc-3ad7-5abc-8fc7-140678311706").value(), complex::Uuid::FromString("ffc5fe81-f046-4bab-ba35-48db20b9e0e4").value()}, // ITKMeanProjectionImage
    {complex::Uuid::FromString("1a7e8483-5838-585c-8d71-e79f836133c4").value(), complex::Uuid::FromString("dadf3a63-73ca-44f2-bb1b-6de5680e42ae").value()}, // ITKMedianProjectionImage
    {complex::Uuid::FromString("bd9bdf46-a229-544a-b158-151920261a63").value(), complex::Uuid::FromString("cc1f2f12-dfd4-4d25-b269-0c08a088b290").value()}, // ITKMinMaxCurvatureFlowImage
    {complex::Uuid::FromString("6394a737-4a31-5593-9bb5-28492129ccf7").value(), complex::Uuid::FromString("dbb9aa27-1f2b-451b-9874-934c8146dd8f").value()}, // ITKMinimumProjectionImage
    {complex::Uuid::FromString("12c83608-c4c5-5c72-b22f-a7696e3f5448").value(), complex::Uuid::FromString("bf889097-f4ad-47f1-b297-432fb818cf15").value()}, // ITKMorphologicalGradientImage
    {complex::Uuid::FromString("81308863-094b-511d-9aa9-865e02e2b8d5").value(), complex::Uuid::FromString("8082ca87-790e-417f-92dd-7df187765d4d").value()}, // ITKMorphologicalWatershedFromMarkersImage
    {complex::Uuid::FromString("001dd629-7032-56a9-99ec-ffaf2caf2ab0").value(), complex::Uuid::FromString("f8e0029c-886b-4c51-9b13-2937602e37d3").value()}, // ITKNormalizeToConstantImage
    {complex::Uuid::FromString("c8362fb9-d3ab-55c0-902b-274cc27d9bb8").value(), complex::Uuid::FromString("6ec430f4-cadf-40db-a1db-a6272e6b4ad7").value()}, // ITKNotImage
    {complex::Uuid::FromString("9af89118-2d15-54ca-9590-75df8be33317").value(), complex::Uuid::FromString("7471c31b-cbab-45ed-8173-5a229d6e035d").value()}, // ITKRegionalMaximaImage
    {complex::Uuid::FromString("f8ed09ae-1f84-5668-a4ad-d770e264f01e").value(), complex::Uuid::FromString("23611690-605e-4d8a-8090-f13d1879fa94").value()}, // ITKRegionalMinimaImage
    {complex::Uuid::FromString("4398d76d-c9aa-5161-bb48-92dd9daaa352").value(), complex::Uuid::FromString("3f971837-a967-40b6-b3dd-f5c2bea5fe19").value()}, // ITKRelabelComponentImage
    {complex::Uuid::FromString("602c270d-ec55-580c-9108-785ba8530964").value(), complex::Uuid::FromString("90e69ba1-5480-49af-835d-ae2082a70247").value()}, // ITKSaltAndPepperNoiseImage
    {complex::Uuid::FromString("31d325fa-e605-5415-85ab-ab93e8cbf32f").value(), complex::Uuid::FromString("80acb013-503c-4e9d-a8c0-98f9f1b5ede2").value()}, // ITKShiftScaleImage
    {complex::Uuid::FromString("97f20f54-276b-54f3-87c9-5eaf16e6c4df").value(), complex::Uuid::FromString("db92bb50-1e38-4c7c-ad8f-15654f224eb2").value()}, // ITKShotNoiseImage
    {complex::Uuid::FromString("e6675be7-e98d-5e0f-a088-ba15cc301038").value(), complex::Uuid::FromString("78e8203e-e46e-4600-89d8-1546a7b075b2").value()}, // ITKSigmoidImage
    {complex::Uuid::FromString("fc192fa8-b6b7-539c-9763-f683724da626").value(), complex::Uuid::FromString("35bc9242-6594-4dee-8527-06ef3571f243").value()}, // ITKSignedDanielssonDistanceMapImage
    {complex::Uuid::FromString("0fd06492-06b1-5044-964c-e0555c556327").value(), complex::Uuid::FromString("0ba7776e-0ba6-4baa-8b2b-5368083db51a").value()}, // ITKSmoothingRecursiveGaussianImage
    {complex::Uuid::FromString("764085a4-6ecb-5fb7-891d-2fda208ba5d8").value(), complex::Uuid::FromString("bf5e8e22-2caa-4ef5-9f2c-a45b7cc279c1").value()}, // ITKSpeckleNoiseImage
    {complex::Uuid::FromString("f092420e-14a0-5dc0-91f8-de0082103aef").value(), complex::Uuid::FromString("a212bd72-b688-4550-9064-2c1af5e74d74").value()}, // ITKSquareImage
    {complex::Uuid::FromString("89b327a7-c6a0-5965-b8aa-9d8bfcedcc76").value(), complex::Uuid::FromString("32e157de-c864-49c9-adec-5dc1d9496690").value()}, // ITKStandardDeviationProjectionImage
    {complex::Uuid::FromString("40714670-b3bd-554c-badb-787d8bab7568").value(), complex::Uuid::FromString("1649703a-0e21-4dc8-96f6-a683114fbc7d").value()}, // ITKSumProjectionImage
    {complex::Uuid::FromString("5845ee06-5c8a-5a74-80fb-c820bd8dfb75").value(), complex::Uuid::FromString("1d73ff42-d952-451f-9394-28304db2ae46").value()}, // ITKThresholdImage
    {complex::Uuid::FromString("2a531add-79fd-5c07-98a6-f875d2ecef4e").value(), complex::Uuid::FromString("97008e55-96cc-49ea-98ff-768c0a964145").value()}, // ITKThresholdMaximumConnectedComponentsImage
    {complex::Uuid::FromString("10aff542-81c5-5f09-9797-c7171c40b6a0").value(), complex::Uuid::FromString("5afeb9d9-9d6f-46e6-a6a5-ad3efa5e21a5").value()}, // ITKValuedRegionalMaximaImage
    {complex::Uuid::FromString("739a0908-cb60-50f7-a484-b2157d023093").value(), complex::Uuid::FromString("8aa6b919-f403-416a-8eea-894d71ea8b64").value()}, // ITKValuedRegionalMinimaImage
    {complex::Uuid::FromString("02e059f7-8055-52b4-9d48-915b67d1e39a").value(), complex::Uuid::FromString("d9768249-608c-4e68-acf7-697db263a362").value()}, // ITKWhiteTopHatImage
    {complex::Uuid::FromString("0259fa1a-4706-5df1-8418-95ffc7b932dd").value(), complex::Uuid::FromString("a182975d-5371-4360-bb94-a7686f1f0f4c").value()}, // ITKZeroCrossingImage
    // {complex::Uuid::FromString(insert DREAM3D UUID string here).value(), complex::Uuid::FromString(insert DREAM3DNX UUID string here).value()}, // dream3d-class-name
  };

} // namespace complex
// clang-format on
