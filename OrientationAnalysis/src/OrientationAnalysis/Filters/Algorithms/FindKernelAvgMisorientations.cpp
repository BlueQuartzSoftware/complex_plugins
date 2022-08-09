#include "FindKernelAvgMisorientations.hpp"

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"

#include "EbsdLib/LaueOps/LaueOps.h"


using namespace complex;

// -----------------------------------------------------------------------------
FindKernelAvgMisorientations::FindKernelAvgMisorientations(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                                           FindKernelAvgMisorientationsInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
FindKernelAvgMisorientations::~FindKernelAvgMisorientations() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& FindKernelAvgMisorientations::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> FindKernelAvgMisorientations::operator()()
{

  // Input Arrays
  const auto& cellPhases = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->CellPhasesArrayPath);
  const auto& featureIds = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureIdsArrayPath);
  const auto& quats = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->QuatsArrayPath);

  const auto& avgQuats = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->AvgQuatsArrayPath);

  const auto& crystalStructures = m_DataStructure.getDataRefAs<UInt32Array>(m_InputValues->CrystalStructuresArrayPath);


  const auto kernelSize = m_InputValues->KernelSize;

    std::vector<LaueOps::Pointer> m_OrientationOps = LaueOps::GetAllOrientationOps();

    int32_t numVoxel = 0; // number of voxels in the feature...
    bool good = false;

    float totalmisorientation = 0.0f;

    uint32_t phase1 = EbsdLib::CrystalStructure::UnknownCrystalStructure;
    uint32_t phase2 = EbsdLib::CrystalStructure::UnknownCrystalStructure;
    SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

    int64_t xPoints = static_cast<int64_t>(udims[0]);
    int64_t yPoints = static_cast<int64_t>(udims[1]);
    int64_t zPoints = static_cast<int64_t>(udims[2]);
    int64_t point = 0;
    size_t neighbor = 0;
    int64_t jStride = 0;
    int64_t kStride = 0;
    float* currentQuatPtr = nullptr;
    for(int64_t col = 0; col < xPoints; col++)
    {
      for(int64_t row = 0; row < yPoints; row++)
      {
        for(int64_t plane = 0; plane < zPoints; plane++)
        {
          point = (plane * xPoints * yPoints) + (row * xPoints) + col;
          if(m_FeatureIds[point] > 0 && m_CellPhases[point] > 0)
          {
            totalmisorientation = 0.0f;
            numVoxel = 0;
            currentQuatPtr = quatPtr->getTuplePointer(point);
            QuatF q1(currentQuatPtr[0], currentQuatPtr[1], currentQuatPtr[2], currentQuatPtr[3]);

            phase1 = crystalStructures[m_CellPhases[point]];
            for(int32_t j = -kernelSize[2]; j < kernelSize[2] + 1; j++)
            {
              jStride = j * xPoints * yPoints;
              for(int32_t k = -kernelSize[1]; k < kernelSize[1] + 1; k++)
              {
                kStride = k * xPoints;
                for(int32_t l = -kernelSize[0]; l < kernelSize[0] + 1; l++)
                {
                  good = true;
                  neighbor = point + (jStride) + (kStride) + (l);
                  if(plane + j < 0)
                  {
                    good = false;
                  }
                  else if(plane + j > zPoints - 1)
                  {
                    good = false;
                  }
                  else if(row + k < 0)
                  {
                    good = false;
                  }
                  else if(row + k > yPoints - 1)
                  {
                    good = false;
                  }
                  else if(col + l < 0)
                  {
                    good = false;
                  }
                  else if(col + l > xPoints - 1)
                  {
                    good = false;
                  }
                  if(good && m_FeatureIds[point] == m_FeatureIds[neighbor])
                  {
                    currentQuatPtr = quatPtr->getTuplePointer(neighbor);

                    QuatF q2(currentQuatPtr[0], currentQuatPtr[1], currentQuatPtr[2], currentQuatPtr[3]);
                    phase2 = crystalStructures[m_CellPhases[neighbor]];
                    OrientationF axisAngle = m_OrientationOps[phase1]->calculateMisorientation(q1, q2);

                    totalmisorientation = totalmisorientation + (axisAngle[3] * SIMPLib::Constants::k_180OverPiD);
                    numVoxel++;
                  }
                }
              }
            }
            m_KernelAverageMisorientations[point] = totalmisorientation / (float)numVoxel;
            if(numVoxel == 0)
            {
              m_KernelAverageMisorientations[point] = 0.0f;
            }
          }
          if(m_FeatureIds[point] == 0 || m_CellPhases[point] == 0)
          {
            m_KernelAverageMisorientations[point] = 0.0f;
          }
        }
      }
    }
  return {};
}
