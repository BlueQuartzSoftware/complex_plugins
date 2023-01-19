#include "AlignSectionsMutualInformation.hpp"

#include "complex/Common/Constants.hpp"
#include "complex/DataStructure/AttributeMatrix.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/Utilities/FilterUtilities.hpp"
#include "complex/Utilities/StringUtilities.hpp"

#include "EbsdLib/LaueOps/LaueOps.h"

#include <random>
#include <vector>

using namespace complex;

// -----------------------------------------------------------------------------
AlignSectionsMutualInformation::AlignSectionsMutualInformation(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                                               AlignSectionsMutualInformationInputValues* inputValues)
: AlignSections(dataStructure, shouldCancel, mesgHandler)
, m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
AlignSectionsMutualInformation::~AlignSectionsMutualInformation() noexcept = default;

// -----------------------------------------------------------------------------
Result<> AlignSectionsMutualInformation::operator()()
{
  const auto& imageGeom = m_DataStructure.getDataRefAs<ImageGeom>(m_InputValues->ImageGeometryPath);

  Result<> result = execute(imageGeom.getDimensions());
  if(result.invalid())
  {
    return result;
  }
  if(m_Result.invalid())
  {
    return m_Result;
  }
  return {};
}

// -----------------------------------------------------------------------------
std::vector<DataPath> AlignSectionsMutualInformation::getSelectedDataPaths() const
{
  const auto& imageGeom = m_DataStructure.getDataRefAs<ImageGeom>(m_InputValues->ImageGeometryPath);
  auto cellDataGroupPath = m_InputValues->ImageGeometryPath.createChildPath(imageGeom.getCellData()->getName());
  auto& cellDataGroup = m_DataStructure.getDataRefAs<AttributeMatrix>(cellDataGroupPath);
  std::vector<DataPath> selectedCellArrays;

  // Create the vector of selected cell DataPaths
  for(const auto& child : cellDataGroup)
  {
    selectedCellArrays.push_back(cellDataGroupPath.createChildPath(child.second->getName()));
  }
  return selectedCellArrays;
}

// -----------------------------------------------------------------------------
Result<> AlignSectionsMutualInformation::findShifts(std::vector<int64>& xShifts, std::vector<int64>& yShifts)
{
  const auto& imageGeom = m_DataStructure.getDataRefAs<ImageGeom>(m_InputValues->ImageGeometryPath);
  const AttributeMatrix* cellData = imageGeom.getCellData();
  int64 totalPoints = cellData->getNumTuples();

  std::ofstream outFile;
  if(m_InputValues->WriteAlignmentShifts)
  {
    // Make sure any directory path is also available as the user may have just typed
    // in a path without actually creating the full path
    Result<> createDirectoriesResult = complex::CreateOutputDirectories(m_InputValues->AlignmentShiftFileName.parent_path());
    if(createDirectoriesResult.invalid())
    {
      return createDirectoriesResult;
    }

    outFile.open(m_InputValues->AlignmentShiftFileName);
    if(!outFile.is_open())
    {
      std::string message = fmt::format("Error creating output shifts file with file path {}", m_InputValues->AlignmentShiftFileName.string());
      return MakeErrorResult(-53701, message);
    }
  }

  SizeVec3 udims = imageGeom.getDimensions();
  int64 dims[3] = {
      static_cast<int64>(udims[0]),
      static_cast<int64>(udims[1]),
      static_cast<int64>(udims[2]),
  };

  std::vector<int32> miFeatureIds(totalPoints, 0);
  std::vector<int32> featureCounts(dims[2], 0);

  //  float32 disorientation = 0.0f;
  //  float32 minDisorientation = std::numeric_limits<float32>::max();
  std::vector<std::vector<float32>> mutualInfo12;
  std::vector<float32> mutualInfo1;
  std::vector<float32> mutualInfo2;

  //
  //  int32 featureCount1 = 0;
  //  int32 featureCount2 = 0;
  //  int64 newXShift = 0;
  //  int64 newYShift = 0;
  //  int64 oldXShift = 0;
  //  int64 oldYShift = 0;
  //  float32 count = 0.0f;
  //  int64 slice = 0;
  //
  //  int32 refGNum = 0;
  //  int32 curGNum = 0;
  //  int64 refPosition = 0;
  //  int64 curPosition = 0;

  formFeaturesSections(miFeatureIds, featureCounts);

  std::vector<std::vector<float32>> misorientations(dims[0]);
  for(int64 i = 0; i < dims[0]; i++)
  {
    misorientations[i].assign(dims[1], 0.0f);
  }

  for(int64 iter = 1; iter < dims[2]; iter++)
  {
    float32 prog = (static_cast<float32>(iter) / dims[2]) * 100;
    std::string progressMessage = fmt::format("Aligning Sections || Determining Shifts || {}% Complete", StringUtilities::number(static_cast<int>(prog)));
    m_MessageHandler(IFilter::Message::Type::Info, progressMessage);

    float32 minDisorientation = std::numeric_limits<float32>::max();
    int64 slice = (dims[2] - 1) - iter;
    int32 featureCount1 = featureCounts[slice];
    int32 featureCount2 = featureCounts[slice + 1];
    mutualInfo12 = std::vector<std::vector<float32>>(featureCount1, std::vector<float32>(featureCount2, 0.0f));
    mutualInfo1 = std::vector<float32>(featureCount1, 0.0f);
    mutualInfo2 = std::vector<float32>(featureCount2, 0.0f);

    int64 oldXShift = -1;
    int64 oldYShift = -1;
    int64 newXShift = 0;
    int64 newYShift = 0;
    for(int64 i = 0; i < dims[0]; i++)
    {
      for(int64 j = 0; j < dims[1]; j++)
      {
        misorientations[i][j] = 0.0F;
      }
    }
    while(newXShift != oldXShift || newYShift != oldYShift)
    {
      oldXShift = newXShift;
      oldYShift = newYShift;
      for(int32 j = -3; j < 4; j++)
      {
        for(int32 k = -3; k < 4; k++)
        {
          float32 disorientation = 0.0F;
          float32 count = 0.0F;
          if(misorientations[k + oldXShift + dims[0] / 2][j + oldYShift + dims[1] / 2] == 0 && llabs(k + oldXShift) < (dims[0] / 2) && (j + oldYShift) < (dims[1] / 2))
          {
            for(int64 dim1Index = 0; dim1Index < dims[1]; dim1Index = dim1Index + 4)
            {
              for(int64 dim0Index = 0; dim0Index < dims[0]; dim0Index = dim0Index + 4)
              {
                if((dim1Index + j + oldYShift) >= 0 && (dim1Index + j + oldYShift) < dims[1] && (dim0Index + k + oldXShift) >= 0 && (dim0Index + k + oldXShift) < dims[0])
                {
                  int64 refPosition = ((slice + 1) * dims[0] * dims[1]) + (dim1Index * dims[0]) + dim0Index;
                  int64 curPosition = (slice * dims[0] * dims[1]) + ((dim1Index + j + oldYShift) * dims[0]) + (dim0Index + k + oldXShift);
                  int32 refGNum = miFeatureIds[refPosition];
                  int32 curGNum = miFeatureIds[curPosition];
                  if(curGNum >= 0 && refGNum >= 0)
                  {
                    mutualInfo12[curGNum][refGNum]++;
                    mutualInfo1[curGNum]++;
                    mutualInfo2[refGNum]++;
                    count++;
                  }
                }
                else
                {
                  mutualInfo12[0][0]++;
                  mutualInfo1[0]++;
                  mutualInfo2[0]++;
                }
              }
            }
            float32 ha = 0.0f;
            float32 hb = 0.0f;
            float32 hab = 0.0f;
            for(int32 b = 0; b < featureCount1; b++)
            {
              mutualInfo1[b] = mutualInfo1[b] / count;
              if(mutualInfo1[b] != 0)
              {
                ha = ha + mutualInfo1[b] * logf(mutualInfo1[b]);
              }
            }
            for(int32 c = 0; c < featureCount2; c++)
            {
              mutualInfo2[c] = mutualInfo2[c] / float32(count);
              if(mutualInfo2[c] != 0)
              {
                hb = hb + mutualInfo2[c] * logf(mutualInfo2[c]);
              }
            }
            for(int32 b = 0; b < featureCount1; b++)
            {
              for(int32 c = 0; c < featureCount2; c++)
              {
                mutualInfo12[b][c] = mutualInfo12[b][c] / count;
                if(mutualInfo12[b][c] != 0)
                {
                  hab = hab + mutualInfo12[b][c] * logf(mutualInfo12[b][c]);
                }
                float32 value = 0.0f;
                if(mutualInfo1[b] > 0 && mutualInfo2[c] > 0)
                {
                  value = (mutualInfo12[b][c] / (mutualInfo1[b] * mutualInfo2[c]));
                }
                if(value != 0)
                {
                  disorientation = disorientation + (mutualInfo12[b][c] * logf(value));
                }
              }
            }
            for(int32 b = 0; b < featureCount1; b++)
            {
              for(int32 c = 0; c < featureCount2; c++)
              {
                mutualInfo12[b][c] = 0.0f;
                mutualInfo1[b] = 0.0f;
                mutualInfo2[c] = 0.0f;
              }
            }
            disorientation = 1.0f / disorientation;
            misorientations[k + oldXShift + dims[0] / 2][j + oldYShift + dims[1] / 2] = disorientation;
            if(disorientation < minDisorientation)
            {
              newXShift = k + oldXShift;
              newYShift = j + oldYShift;
              minDisorientation = disorientation;
            }
          }
        }
      }
    }
    xShifts[iter] = xShifts[iter - 1] + newXShift;
    yShifts[iter] = yShifts[iter - 1] + newYShift;
    if(m_InputValues->WriteAlignmentShifts)
    {
      outFile << slice << "	" << slice + 1 << "	" << newXShift << "	" << newYShift << "	" << xShifts[iter] << "	" << yShifts[iter] << "\n";
    }
  }

  if(m_InputValues->WriteAlignmentShifts)
  {
    outFile.close();
  }

  return {};
}

// -----------------------------------------------------------------------------
void AlignSectionsMutualInformation::formFeaturesSections(std::vector<int32>& miFeatureIds, std::vector<int32>& featureCounts)
{
  const auto& imageGeom = m_DataStructure.getDataRefAs<ImageGeom>(m_InputValues->ImageGeometryPath);

  SizeVec3 udims = imageGeom.getDimensions();
  int64 dims[3] = {
      static_cast<int64>(udims[0]),
      static_cast<int64>(udims[1]),
      static_cast<int64>(udims[2]),
  };

  auto orientationOps = LaueOps::GetAllOrientationOps();

  Float32Array& quats = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->QuatsArrayPath);
  BoolArray* goodVoxelsPtr = m_DataStructure.getDataAs<BoolArray>(m_InputValues->GoodVoxelsArrayPath);
  Int32Array& m_CellPhases = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->CellPhasesArrayPath);
  UInt32Array& m_CrystalStructures = m_DataStructure.getDataRefAs<UInt32Array>(m_InputValues->CrystalStructuresArrayPath);

  size_t initialVoxelsListSize = 1000;

  float misorientationTolerance = m_InputValues->MisorientationTolerance * complex::Constants::k_PiOver180F;

  featureCounts.resize(dims[2]);

  std::vector<int64_t> voxelList(initialVoxelsListSize, -1);
  int64_t neighborPoints[4] = {-dims[0], -1, 1, dims[0]};

  int64_t lastXIndex = 0;
  int64_t lastYIndex = 0;

  for(int64_t slice = 0; slice < dims[2]; slice++)
  {
    float32 prog = (static_cast<float32>(slice) / dims[2]) * 100;
    std::string ss = fmt::format("Aligning Sections || Identifying Features on Sections || {}% Complete", StringUtilities::number(static_cast<int>(prog)));
    m_MessageHandler(IFilter::Message::Type::Info, ss);

    int32 featureCount = 1;
    bool noSeeds = false;
    while(!noSeeds)
    {
      int64 seed = -1;
      int64 currentXPoint = lastXIndex;
      int64 currentYPoint = lastYIndex;
      for(int64_t yIndex = 0; yIndex < dims[1]; ++yIndex)
      {
        for(int64_t xIndex = 0; xIndex < dims[0]; ++xIndex)
        {
          int64 x = currentXPoint + xIndex;
          int64 y = currentYPoint + yIndex;
          int64 z = slice;
          if(x > dims[0] - 1)
          {
            x = x - dims[0];
          }
          if(y > dims[1] - 1)
          {
            y = y - dims[1];
          }
          int64 point = (z * dims[0] * dims[1]) + (y * dims[0]) + x;

          if((!m_InputValues->UseGoodVoxels || (goodVoxelsPtr != nullptr && (*goodVoxelsPtr)[point])) && miFeatureIds[point] == 0 && m_CellPhases[point] > 0)
          {
            seed = point;
            lastXIndex = x;
            lastYIndex = y;
          }
          if(seed > -1)
          {
            break;
          }
        }
        if(seed > -1)
        {
          break;
        }
      }
      if(seed == -1)
      {
        noSeeds = true;
      }
      if(seed >= 0)
      {
        usize size = 0;
        miFeatureIds[seed] = featureCount;
        voxelList[size] = seed;
        size++;
        for(size_t j = 0; j < size; ++j)
        {
          int64_t currentpoint = voxelList[j];
          int64 col = currentpoint % dims[0];
          int64 row = (currentpoint / dims[0]) % dims[1];

          auto q1TupleIndex = currentpoint * 4;
          QuatF quat1(quats[q1TupleIndex], quats[q1TupleIndex + 1], quats[q1TupleIndex + 2], quats[q1TupleIndex + 3]);
          uint32_t phase1 = m_CrystalStructures[m_CellPhases[currentpoint]];
          for(int32_t i = 0; i < 4; i++)
          {
            int64 neighbor = currentpoint + neighborPoints[i];
            if((i == 0) && row == 0)
            {
              continue;
            }
            if((i == 3) && row == (dims[1] - 1))
            {
              continue;
            }
            if((i == 1) && col == 0)
            {
              continue;
            }
            if((i == 2) && col == (dims[0] - 1))
            {
              continue;
            }
            if(miFeatureIds[neighbor] <= 0 && m_CellPhases[neighbor] > 0)
            {
              float32 angle = std::numeric_limits<float>::max();
              auto q2TupleIndex = neighbor * 4;
              QuatF quat2(quats[q2TupleIndex], quats[q2TupleIndex + 1], quats[q2TupleIndex + 2], quats[q2TupleIndex + 3]);
              uint32_t phase2 = m_CrystalStructures[m_CellPhases[neighbor]];

              if(phase1 == phase2)
              {
                OrientationF axisAngle = orientationOps[phase1]->calculateMisorientation(quat1, quat2);
                angle = axisAngle[3];
              }
              if(angle < misorientationTolerance)
              {
                miFeatureIds[neighbor] = featureCount;
                voxelList[size] = neighbor;
                size++;
                if(std::vector<int64_t>::size_type(size) >= voxelList.size())
                {
                  size = voxelList.size();
                  voxelList.resize(size + initialVoxelsListSize);
                  for(std::vector<int64_t>::size_type v = size; v < voxelList.size(); ++v)
                  {
                    voxelList[v] = -1;
                  }
                }
              }
            }
          }
        }
        voxelList.erase(std::remove(voxelList.begin(), voxelList.end(), -1), voxelList.end());
        featureCount++;
        voxelList.assign(initialVoxelsListSize, -1);
      }
    }
    featureCounts[slice] = featureCount;
  }
}
