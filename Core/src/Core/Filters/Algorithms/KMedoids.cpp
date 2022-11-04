#include "KMedoids.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/Utilities/FilterUtilities.hpp"

#include "Core/Utilities/ClusteringAlgorithms/KMedoidsTemplate.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
KMedoids::KMedoids(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, KMedoidsInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
KMedoids::~KMedoids() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& KMedoids::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> KMedoids::operator()()
{
  if(m_InputValues->UseMask)
  {
    ExecuteDataFunction(KMedoidsTemplate{}, m_InDataPtr.getDataType(), m_InDataPtr, m_MedoidsArrayPtr, m_MaskPtr, m_InitClusters, m_FeatureIdsPtr, m_DistanceMetric)
  }
  else
  {
    size_t numTuples = m_InDataPtr->getNumberOfTuples();
    std::vector<bool> tmpMask(numTuples, true);
    ExecuteDataFunction(KMedoidsTemplate{},  m_InDataPtr.getDataType(), m_InDataPtr, this, m_InDataPtr, m_MedoidsArrayPtr, tmpMask, m_InitClusters, m_FeatureIdsPtr, m_DistanceMetric)
  }


  return {};
}
