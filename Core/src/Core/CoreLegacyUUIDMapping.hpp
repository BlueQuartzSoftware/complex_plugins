#pragma once

#include <map>
#include <string>

namespace complex
{
  // Regex to grep UUIDs : /{\"([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})\",\s\"([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}(?:\s,\s[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})*)\"},\s\/\/\s(\w+)
  static const std::map<std::string, std::string> k_SIMPL_to_Core
  {
    // syntax std::make_pair {Dream3d UUID , Dream3dnx UUID}, // dream3d-class-name
    {"886f8b46-51b6-5682-a289-6febd10b7ef0", "b83f9bae-9ccf-4932-96c3-7f2fdb091452"}, // AlignSectionsFeatureCentroid
    {"289f0d8c-29ab-5fbc-91bd-08aac01e37c5", "c6b6d9e5-301d-4767-abf7-530f5ef5007d"}, // CalculateArrayHistogram
    {"eb5a89c4-4e71-59b1-9719-d10a652d961e", "d938a2aa-fee2-4db9-aa2f-2c34a9736580"}, // ConvertColorToGrayScale
    {"f4ba5fa4-bb5c-5dd1-9429-0dd86d0ecb37", "6dc586cc-59fb-4ee8-90ff-2d3587da12f5"}, // ConvertData
    {"9ac220b9-14f9-581a-9bac-5714467589cc", "24768170-5b90-4a9d-82ac-9aeecd9f892e"}, // CreateGeometry
    {"3adfe077-c3c9-5cd0-ad74-cf5f8ff3d254", "7f2f7378-580e-4337-8c04-a29e7883db0b"}, // ErodeDilateBadData
    {"737b8d5a-8622-50f9-9a8a-bfdb57608891", "d734293f-3017-4699-b458-c05aca078b96"}, // FeatureDataCSVWriter
    {"30ae0a1e-3d94-5dab-b279-c5727ab5d7ff", "a59eb864-9e6b-40bb-9292-e5281b0b4f3e"}, // FillBadData
    {"933e4b2d-dd61-51c3-98be-00548ba783a3", "ba9ae8f6-443e-41d3-bb45-a08a139325c1"}, // FindEuclideanDistMap
    {"6f8ca36f-2995-5bd3-8672-6b0b80d5b2ca", "c6875ac7-8bdd-4f69-b6ce-82ac09bd3421"}, // FindFeatureCentroids
    {"697ed3de-db33-5dd1-a64b-04fb71e7d63e", "924c10e3-2f39-4c08-9d7a-7fe029f74f6d"}, // FindNeighborhoods
    {"3b0ababf-9c8d-538d-96af-e40775c4f0ab", "036b17d5-23bb-4a24-9187-c4a8dd918792"}, // FindShapes
    {"5d586366-6b59-566e-8de1-57aa9ae8a91c", "94e83e4f-797d-4594-b130-3819b7676f01"}, // FindSurfaceAreaToVolume
    {"a8463056-3fa7-530b-847f-7f4cb78b8602", "6e8cc6ec-8b9b-402e-9deb-85bd1cdba743"}, // RemoveFlaggedFeatures
    {"1966e540-759c-5798-ae26-0c6a3abc65c0", "9783ea2c-4cf7-46de-ab21-b40d91a48c5b"}, // ResampleImageGeom
    {"e25d9b4c-2b37-578c-b1de-cf7032b5ef19", "d2451dc1-a5a1-4ac2-a64d-7991669dcffc"}, // RotateSampleRefFrame
    {"5ecf77f4-a38a-52ab-b4f6-0fb8a9c5cb9c", "55da791f-4d1c-4413-8673-742f27d2b22b"}, // SplitAttributeArray
    {"5fbf9204-2c6c-597b-856a-f4612adbac38", "06c8bfe8-2b42-4956-aca3-580bc0620716"}, // WriteASCIIData
    // {insert DREAM3D UUID here, insert DREAM3DNX UUID here}, // dream3d-class-name
  };

  static const std::map<std::string, std::string> k_Core_to_SIMPL
  {
    // syntax std::make_pair {Dream3dnx UUID , Dream3d UUID}, // dream3dnx-class-name
    {"b83f9bae-9ccf-4932-96c3-7f2fdb091452", "886f8b46-51b6-5682-a289-6febd10b7ef0"}, // AlignSectionsFeatureCentroidFilter
    {"c6b6d9e5-301d-4767-abf7-530f5ef5007d", "289f0d8c-29ab-5fbc-91bd-08aac01e37c5"}, // CalculateArrayHistogramFilter
    {"d938a2aa-fee2-4db9-aa2f-2c34a9736580", "eb5a89c4-4e71-59b1-9719-d10a652d961e"}, // ConvertColorToGrayScaleFilter
    {"6dc586cc-59fb-4ee8-90ff-2d3587da12f5", "f4ba5fa4-bb5c-5dd1-9429-0dd86d0ecb37"}, // ConvertDataFilter
    {"24768170-5b90-4a9d-82ac-9aeecd9f892e", "9ac220b9-14f9-581a-9bac-5714467589cc"}, // CreateGeometryFilter
    {"7f2f7378-580e-4337-8c04-a29e7883db0b", "3adfe077-c3c9-5cd0-ad74-cf5f8ff3d254"}, // ErodeDilateBadDataFilter
    {"d734293f-3017-4699-b458-c05aca078b96", "737b8d5a-8622-50f9-9a8a-bfdb57608891"}, // FeatureDataCSVWriterFilter
    {"a59eb864-9e6b-40bb-9292-e5281b0b4f3e", "30ae0a1e-3d94-5dab-b279-c5727ab5d7ff"}, // FillBadDataFilter
    {"ba9ae8f6-443e-41d3-bb45-a08a139325c1", "933e4b2d-dd61-51c3-98be-00548ba783a3"}, // FindEuclideanDistMapFilter
    {"c6875ac7-8bdd-4f69-b6ce-82ac09bd3421", "6f8ca36f-2995-5bd3-8672-6b0b80d5b2ca"}, // FindFeatureCentroidsFilter
    {"924c10e3-2f39-4c08-9d7a-7fe029f74f6d", "697ed3de-db33-5dd1-a64b-04fb71e7d63e"}, // FindNeighborhoodsFilter
    {"036b17d5-23bb-4a24-9187-c4a8dd918792", "3b0ababf-9c8d-538d-96af-e40775c4f0ab"}, // FindShapesFilter
    {"94e83e4f-797d-4594-b130-3819b7676f01", "5d586366-6b59-566e-8de1-57aa9ae8a91c"}, // FindSurfaceAreaToVolumeFilter
    {"6e8cc6ec-8b9b-402e-9deb-85bd1cdba743", "a8463056-3fa7-530b-847f-7f4cb78b8602"}, // RemoveFlaggedFeaturesFilter
    {"9783ea2c-4cf7-46de-ab21-b40d91a48c5b", "1966e540-759c-5798-ae26-0c6a3abc65c0"}, // ResampleImageGeomFilter
    {"d2451dc1-a5a1-4ac2-a64d-7991669dcffc", "e25d9b4c-2b37-578c-b1de-cf7032b5ef19"}, // RotateSampleRefFrameFilter
    {"55da791f-4d1c-4413-8673-742f27d2b22b", "5ecf77f4-a38a-52ab-b4f6-0fb8a9c5cb9c"}, // SplitAttributeArrayFilter
    {"06c8bfe8-2b42-4956-aca3-580bc0620716", "5fbf9204-2c6c-597b-856a-f4612adbac38"}, // WriteASCIIDataFilter
    // {insert DREAM3DNX UUID here, insert DREAM3D UUID here}, // dream3dnx-class-name
  };

} // namespace complex