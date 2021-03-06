#include "StatsToolboxPlugin.hpp"
#include "StatsToolbox/StatsToolbox_filter_registration.hpp"

using namespace complex;

namespace
{
// This maps previous filters from DREAM.3D Version 6.x to DREAM.3D Version 7.x
std::map<complex::Uuid, complex::Uuid> k_SimplToComplexFilterMapping = {
    {Uuid::FromString("{289f0d8c-29ab-5fbc-91bd-08aac01e37c5}").value(), Uuid::FromString("289f0d8c-29ab-5fbc-91bd-08aac01e37c5").value()}, /* CalculateArrayHistogram */
    {Uuid::FromString("{27a132b2-a592-519a-8cb7-38599a7f28ec}").value(), Uuid::FromString("27a132b2-a592-519a-8cb7-38599a7f28ec").value()}, /* ComputeMomentInvariants2D */
    {Uuid::FromString("{cd91b8fd-0383-5803-ad26-9a47d6c309b0}").value(), Uuid::FromString("cd91b8fd-0383-5803-ad26-9a47d6c309b0").value()}, /* FindAvgScalarValueForFeatures */
    {Uuid::FromString("{6357243e-41a6-52c4-be2d-2f6894c39fcc}").value(), Uuid::FromString("6357243e-41a6-52c4-be2d-2f6894c39fcc").value()}, /* FindBoundaryElementFractions */
    {Uuid::FromString("{29086169-20ce-52dc-b13e-824694d759aa}").value(), Uuid::FromString("29086169-20ce-52dc-b13e-824694d759aa").value()}, /* FindDifferenceMap */
    {Uuid::FromString("{933e4b2d-dd61-51c3-98be-00548ba783a3}").value(), Uuid::FromString("933e4b2d-dd61-51c3-98be-00548ba783a3").value()}, /* FindEuclideanDistMap */
    {Uuid::FromString("{a1e9cf6d-2d1b-573e-98b8-0314c993d2b6}").value(), Uuid::FromString("a1e9cf6d-2d1b-573e-98b8-0314c993d2b6").value()}, /* FindFeatureClustering */
    {Uuid::FromString("{9f77b4a9-6416-5220-a688-115f4e14c90d}").value(), Uuid::FromString("9f77b4a9-6416-5220-a688-115f4e14c90d").value()}, /* FindLargestCrossSections */
    {Uuid::FromString("{697ed3de-db33-5dd1-a64b-04fb71e7d63e}").value(), Uuid::FromString("697ed3de-db33-5dd1-a64b-04fb71e7d63e").value()}, /* FindNeighborhoods */
    {Uuid::FromString("{97cf66f8-7a9b-5ec2-83eb-f8c4c8a17bac}").value(), Uuid::FromString("97cf66f8-7a9b-5ec2-83eb-f8c4c8a17bac").value()}, /* FindNeighbors */
    {Uuid::FromString("{529743cf-d5d5-5d5a-a79f-95c84a5ddbb5}").value(), Uuid::FromString("529743cf-d5d5-5d5a-a79f-95c84a5ddbb5").value()}, /* FindNumFeatures */
    {Uuid::FromString("{3b0ababf-9c8d-538d-96af-e40775c4f0ab}").value(), Uuid::FromString("3b0ababf-9c8d-538d-96af-e40775c4f0ab").value()}, /* FindShapes */
    {Uuid::FromString("{656f144c-a120-5c3b-bee5-06deab438588}").value(), Uuid::FromString("656f144c-a120-5c3b-bee5-06deab438588").value()}, /* FindSizes */
    {Uuid::FromString("{5d586366-6b59-566e-8de1-57aa9ae8a91c}").value(), Uuid::FromString("5d586366-6b59-566e-8de1-57aa9ae8a91c").value()}, /* FindSurfaceAreaToVolume */
    {Uuid::FromString("{68246a67-7f32-5c80-815a-bec82008d7bc}").value(), Uuid::FromString("68246a67-7f32-5c80-815a-bec82008d7bc").value()}, /* FindVolFractions */
    {Uuid::FromString("{6c255fc4-1692-57cf-be55-71dc4e05ec83}").value(), Uuid::FromString("6c255fc4-1692-57cf-be55-71dc4e05ec83").value()}, /* FitFeatureData */
    {Uuid::FromString("{19a1cb76-6b46-528d-b629-1af5f1d6344c}").value(), Uuid::FromString("19a1cb76-6b46-528d-b629-1af5f1d6344c").value()}, /* GenerateEnsembleStatistics */
};
// Plugin Uuid
constexpr AbstractPlugin::IdType k_ID = *Uuid::FromString("b2e819c6-314c-56da-ab25-0684cd22cb2c");
} // namespace

StatsToolboxPlugin::StatsToolboxPlugin()
: AbstractPlugin(k_ID, "StatsToolbox", "<<--Description was not read-->>", "BlueQuartz Software, LLC")
{
  std::vector<::FilterCreationFunc> filterFuncs = ::GetPluginFilterList();
  for(const auto& filterFunc : filterFuncs)
  {
    addFilter(filterFunc);
  }
}

StatsToolboxPlugin::~StatsToolboxPlugin() = default;

std::vector<complex::H5::IDataFactory*> StatsToolboxPlugin::getDataFactories() const
{
  return {};
}

COMPLEX_DEF_PLUGIN(StatsToolboxPlugin)
