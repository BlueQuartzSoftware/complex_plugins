#include "FindKernelAvgMisorientations.hpp"

#include "complex/Common/Numbers.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/Utilities/ParallelData3DAlgorithm.hpp"

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
  // Input Arrays / Parameter Data
  const auto& cellPhases = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->CellPhasesArrayPath);
  const auto& featureIds = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeatureIdsArrayPath);
  const auto& quats = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->QuatsArrayPath);
  const auto& crystalStructures = m_DataStructure.getDataRefAs<UInt32Array>(m_InputValues->CrystalStructuresArrayPath);
  const auto kernelSize = m_InputValues->KernelSize;

  // Output Arrays
  auto& kernelAvgMisorientations = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->KernelAverageMisorientationsArrayName);

  std::vector<LaueOps::Pointer> m_OrientationOps = LaueOps::GetAllOrientationOps();

  auto* gridGeom = m_DataStructure.getDataAs<ImageGeom>(m_InputValues->InputImageGeometry);
  SizeVec3 udims = gridGeom->getDimensions();

  auto xPoints = static_cast<int64_t>(udims[0]);
  auto yPoints = static_cast<int64_t>(udims[1]);
  auto zPoints = static_cast<int64_t>(udims[2]);

  for(int64_t col = 0; col < xPoints; col++)
  {
    for(int64_t row = 0; row < yPoints; row++)
    {
      for(int64_t plane = 0; plane < zPoints; plane++)
      {
        int64_t point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        if(featureIds[point] > 0 && cellPhases[point] > 0)
        {
          float totalMisorientation = 0.0f;
          int32 numVoxel = 0;

          size_t quatIndex = point * 4;
          QuatF q1(quats[quatIndex], quats[quatIndex + 1], quats[quatIndex + 2], quats[quatIndex + 3]);

          uint32_t phase1 = crystalStructures[cellPhases[point]];
          for(int32_t j = -kernelSize[2]; j < kernelSize[2] + 1; j++)
          {
            int64_t jStride = j * xPoints * yPoints;
            for(int32_t k = -kernelSize[1]; k < kernelSize[1] + 1; k++)
            {
              int64_t kStride = k * xPoints;
              for(int32_t l = -kernelSize[0]; l < kernelSize[0] + 1; l++)
              {
                bool good = true;
                size_t neighbor = point + (jStride) + (kStride) + (l);
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
                if(good && featureIds[point] == featureIds[neighbor])
                {
                  quatIndex = neighbor * 4;
                  QuatF q2(quats[quatIndex], quats[quatIndex + 1], quats[quatIndex + 2], quats[quatIndex + 3]);
                  OrientationF axisAngle = m_OrientationOps[phase1]->calculateMisorientation(q1, q2);
                  totalMisorientation = totalMisorientation + (axisAngle[3] * complex::numbers::k_180OverPi);
                  numVoxel++;
                }
              }
            }
          }
          kernelAvgMisorientations[point] = totalMisorientation / static_cast<float>(numVoxel);
          if(numVoxel == 0)
          {
            kernelAvgMisorientations[point] = 0.0f;
          }
        }
        if(featureIds[point] == 0 || cellPhases[point] == 0)
        {
          kernelAvgMisorientations[point] = 0.0f;
        }
      }
    }
  }
  return {};
}
