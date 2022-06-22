#include "AlignSectionsMisorientation.hpp"

#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/Geometry/AbstractGeometryGrid.hpp"
#include "complex/Common/Numbers.hpp"
#include "complex/Utilities/DataArrayUtilities.hpp"


#include "EbsdLib/LaueOps/LaueOps.h"


#include <iostream>
#include <fstream>

using namespace complex;

// -----------------------------------------------------------------------------
AlignSectionsMisorientation::AlignSectionsMisorientation(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                                         AlignSectionsMisorientationInputValues* inputValues)
: AlignSections(dataStructure, shouldCancel, mesgHandler)
, m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
AlignSectionsMisorientation::~AlignSectionsMisorientation() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& AlignSectionsMisorientation::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> AlignSectionsMisorientation::operator()()
{
  auto gridGeom = m_DataStructure.getDataAs<AbstractGeometryGrid>(m_InputValues->inputImageGeometry);

  return execute(gridGeom);
}

// -----------------------------------------------------------------------------
std::vector<DataPath> AlignSectionsMisorientation::getSelectedDataPaths() const
{
  auto cellDataGroupPath = m_InputValues->cellDataGroupPath;
  auto& cellDataGroup = m_DataStructure.getDataRefAs<DataGroup>(cellDataGroupPath);
  std::vector<DataPath> selectedCellArrays;

  // Create the vector of selected cell DataPaths
  for(DataGroup::Iterator child = cellDataGroup.begin(); child != cellDataGroup.end(); ++child)
  {
    selectedCellArrays.push_back(m_InputValues->cellDataGroupPath.createChildPath(child->second->getName()));
  }
  return selectedCellArrays;
}




// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::find_shifts(std::vector<int64_t>& xshifts, std::vector<int64_t>& yshifts)
{
  std::ofstream outFile(m_InputValues->alignmentShiftFileName, std::ios_base::out);
  if(!outFile.is_open())
  {
    return;
  }

  auto gridGeom = m_DataStructure.getDataAs<AbstractGeometryGrid>(m_InputValues->inputImageGeometry);

  const auto& cellPhases = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->cellPhasesArrayPath);
  const auto& quats = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->quatsArrayPath);
  const auto& crystalStructures = m_DataStructure.getDataRefAs<UInt32Array>(m_InputValues->crystalStructuresArrayPath);

  SizeVec3 udims = gridGeom->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]),
      static_cast<int64_t>(udims[1]),
      static_cast<int64_t>(udims[2]),
  };

  std::shared_ptr<MaskCompare> maskCompare = InstantiateMaskCompare(m_DataStructure, m_InputValues->goodVoxelsArrayPath);

  std::vector<LaueOps::Pointer> orientationOps = LaueOps::GetAllOrientationOps();

  float disorientation = 0.0f;
  float minDisorientation = std::numeric_limits<float>::max();
  int64_t newxshift = 0;
  int64_t newyshift = 0;
  int64_t oldxshift = 0;
  int64_t oldyshift = 0;
  float count = 0.0f;
  int64_t slice = 0;
  float w = 0.0f;

  int64_t refposition = 0;
  int64_t curposition = 0;

  uint32_t phase1 = 0, phase2 = 0;
  int32_t progInt = 0;

  // Allocate a 2D Array which will be reused from slice to slice
  std::vector<bool> misorients(dims[0] * dims[1], false);

  int64_t idx = 0; // This will be used to compute the index into the flat array
  int64_t xIdx = 0;
  int64_t yIdx = 0;

  const int64_t halfDim0 = static_cast<int64_t>(dims[0] * 0.5f);
  const int64_t halfDim1 = static_cast<int64_t>(dims[1] * 0.5f);

  double deg2Rad = (complex::numbers::pi / 180.0);
  // Loop over the Z Direction
  for(int64_t iter = 1; iter < dims[2]; iter++)
  {
    progInt = static_cast<float>(iter) / static_cast<float>(dims[2]) * 100.0f;
    std::string message = fmt::format("Determining Shifts || {}% Complete", progInt);
    m_MessageHandler(complex::IFilter::ProgressMessage{complex::IFilter::Message::Type::Info, message, progInt});

    if(getCancel())
    {
      return;
    }
    minDisorientation = std::numeric_limits<float>::max();
    slice = (dims[2] - 1) - iter;
    oldxshift = -1;
    oldyshift = -1;
    newxshift = 0;
    newyshift = 0;

    // Initialize everything to false
    std::fill(misorients. begin(), misorients. end(), false);

    float misorientationTolerance = m_InputValues->misorientationTolerance * deg2Rad;

    while(newxshift != oldxshift || newyshift != oldyshift)
    {
      oldxshift = newxshift;
      oldyshift = newyshift;
      for(int32_t j = -3; j < 4; j++)
      {
        for(int32_t k = -3; k < 4; k++)
        {
          disorientation = 0.0f;
          count = 0.0f;
          xIdx = k + oldxshift + halfDim0;
          yIdx = j + oldyshift + halfDim1;
          idx = (dims[0] * yIdx) + xIdx;
          if(!misorients[idx] && llabs(k + oldxshift) < halfDim0 && llabs(j + oldyshift) < halfDim1)
          {
            for(int64_t l = 0; l < dims[1]; l = l + 4)
            {
              for(int64_t n = 0; n < dims[0]; n = n + 4)
              {
                if((l + j + oldyshift) >= 0 && (l + j + oldyshift) < dims[1] && (n + k + oldxshift) >= 0 && (n + k + oldxshift) < dims[0])
                {
                  count++;
                  refposition = ((slice + 1) * dims[0] * dims[1]) + (l * dims[0]) + n;
                  curposition = (slice * dims[0] * dims[1]) + ((l + j + oldyshift) * dims[0]) + (n + k + oldxshift);
                  if(!m_InputValues->useGoodVoxels || maskCompare->bothTrue(refposition, curposition) )
                  {
                    w = std::numeric_limits<float>::max();
                    if(cellPhases[refposition] > 0 && cellPhases[curposition] > 0)
                    {
                      QuatF q1(quats[refposition * 4], quats[refposition * 4 + 1], quats[refposition * 4 + 2], quats[refposition * 4 + 3]); // Makes a copy into voxQuat!!!!
                      phase1 = crystalStructures[cellPhases[refposition]];
                      QuatF q2(quats[curposition * 4], quats[curposition * 4 + 1], quats[curposition * 4 + 2], quats[curposition * 4 + 3]); // Makes a copy into voxQuat!!!!
                      phase2 = crystalStructures[cellPhases[curposition]];
                      if(phase1 == phase2 && phase1 < static_cast<uint32_t>(orientationOps.size()))
                      {
                        OrientationF axisAngle = orientationOps[phase1]->calculateMisorientation(q1, q2);
                        w = axisAngle[3];
                      }
                    }
                    if(w > misorientationTolerance)
                    {
                      disorientation++;
                    }
                  }
                  if(m_InputValues->useGoodVoxels)
                  {
                    if(!maskCompare->bothTrue(refposition, curposition))
                    {
                      disorientation++;
                    }
                  }
                }
              }
            }
            disorientation = disorientation / count;
            xIdx = k + oldxshift + halfDim0;
            yIdx = j + oldyshift + halfDim1;
            idx = (dims[0] * yIdx) + xIdx;
            misorients[idx] = true;
            if(disorientation < minDisorientation || (disorientation == minDisorientation && ((llabs(k + oldxshift) < llabs(newxshift)) || (llabs(j + oldyshift) < llabs(newyshift)))))
            {
              newxshift = k + oldxshift;
              newyshift = j + oldyshift;
              minDisorientation = disorientation;
            }
          }
        }
      }
    }
    xshifts[iter] = xshifts[iter - 1] + newxshift;
    yshifts[iter] = yshifts[iter - 1] + newyshift;
    if(m_InputValues->writeAlignmentShifts)
    {
      outFile << slice << "	" << slice + 1 << "	" << newxshift << "	" << newyshift << "	" << xshifts[iter] << "	" << yshifts[iter] << "\n";
    }
  }
  if(m_InputValues->writeAlignmentShifts)
  {
    outFile.close();
  }
}
