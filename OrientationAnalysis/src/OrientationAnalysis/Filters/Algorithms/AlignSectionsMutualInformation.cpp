#include "AlignSectionsMutualInformation.hpp"

#include "complex/Common/Constants.hpp"
#include "complex/DataStructure/AttributeMatrix.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/Utilities/StringUtilities.hpp"

#include "EbsdLib/LaueOps/LaueOps.h"

#include <chrono>
#include <random>

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
const std::atomic_bool& AlignSectionsMutualInformation::getCancel()
{
  return m_ShouldCancel;
}

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
  SizeVec3 udims = imageGeom.getDimensions();

  std::ofstream outFile;
  if(m_InputValues->WriteAlignmentShifts)
  {
    outFile.open(m_InputValues->AlignmentShiftFileName);
  }

  int64 dims[3] = {
      static_cast<int64>(udims[0]),
      static_cast<int64>(udims[1]),
      static_cast<int64>(udims[2]),
  };

  std::vector<int32> miFeatureIds(totalPoints, 0);
  std::vector<int32> featureCounts(dims[2], 0);

  float32 disorientation = 0.0f;
  float32 minDisorientation = std::numeric_limits<float32>::max();
  float32** mutualInfo12 = nullptr;
  float32* mutualInfo1 = nullptr;
  float32* mutualInfo2 = nullptr;
  int32 featureCount1 = 0, featureCount2 = 0;
  int64 newXShift = 0;
  int64 newYShift = 0;
  int64 oldXShift = 0;
  int64 oldYShift = 0;
  float32 count = 0.0f;
  int64 slice = 0;

  int32 refGNum = 0, curGNum = 0;
  int64 refPosition = 0;
  int64 curPosition = 0;

  formFeaturesSections(miFeatureIds, featureCounts);

  std::vector<std::vector<float32>> misorients;
  misorients.resize(dims[0]);
  for(int64 a = 0; a < dims[0]; a++)
  {
    misorients[a].assign(dims[1], 0.0f);
  }

  for(int64 iter = 1; iter < dims[2]; iter++)
  {
    float32 prog = (static_cast<float32>(iter) / dims[2]) * 100;
    std::string ss = fmt::format("Aligning Sections || Determining Shifts || {}% Complete", StringUtilities::number(static_cast<int>(prog)));
    m_MessageHandler(IFilter::Message::Type::Info, ss);

    minDisorientation = std::numeric_limits<float32>::max();
    slice = (dims[2] - 1) - iter;
    featureCount1 = featureCounts[slice];
    featureCount2 = featureCounts[slice + 1];
    mutualInfo12 = new float32*[featureCount1];
    mutualInfo1 = new float32[featureCount1];
    mutualInfo2 = new float32[featureCount2];

    for(int32 a = 0; a < featureCount1; a++)
    {
      mutualInfo1[a] = 0.0f;
      mutualInfo12[a] = new float[featureCount2];
      for(int32 b = 0; b < featureCount2; b++)
      {
        mutualInfo12[a][b] = 0.0f;
        mutualInfo2[b] = 0.0f;
      }
    }
    oldXShift = -1;
    oldYShift = -1;
    newXShift = 0;
    newYShift = 0;
    for(int64 a = 0; a < dims[0]; a++)
    {
      for(int64 b = 0; b < dims[1]; b++)
      {
        misorients[a][b] = 0;
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
          disorientation = 0;
          count = 0;
          if(misorients[k + oldXShift + dims[0] / 2][j + oldYShift + dims[1] / 2] == 0 && llabs(k + oldXShift) < (dims[0] / 2) && (j + oldYShift) < (dims[1] / 2))
          {
            for(int64 l = 0; l < dims[1]; l = l + 4)
            {
              for(int64 n = 0; n < dims[0]; n = n + 4)
              {
                if((l + j + oldYShift) >= 0 && (l + j + oldYShift) < dims[1] && (n + k + oldXShift) >= 0 && (n + k + oldXShift) < dims[0])
                {
                  refPosition = ((slice + 1) * dims[0] * dims[1]) + (l * dims[0]) + n;
                  curPosition = (slice * dims[0] * dims[1]) + ((l + j + oldYShift) * dims[0]) + (n + k + oldXShift);
                  refGNum = miFeatureIds[refPosition];
                  curGNum = miFeatureIds[curPosition];
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
            misorients[k + oldXShift + dims[0] / 2][j + oldYShift + dims[1] / 2] = disorientation;
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
    delete[] mutualInfo1;
    delete[] mutualInfo2;
    for(int32 i = 0; i < featureCount1; i++)
    {
      delete mutualInfo12[i];
    }
    delete[] mutualInfo12;
    mutualInfo1 = nullptr;
    mutualInfo2 = nullptr;
    mutualInfo12 = nullptr;
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

  int64 point = 0;
  int64 seed = 0;
  bool noSeeds = false;
  int32 featureCount = 1;
  int64 neighbor = 0;

  std::random_device randomDevice;           // Will be used to obtain a seed for the random number engine
  std::mt19937_64 generator(randomDevice()); // Standard mersenne_twister_engine seeded with rd()
  generator.seed(static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()));
  std::uniform_real_distribution<> distribution(0, 1);

  Float32Array& quats = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->QuatsArrayPath);
  BoolArray& goodVoxels = m_DataStructure.getDataRefAs<BoolArray>(m_InputValues->GoodVoxelsArrayPath);
  Int32Array& cellPhases = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->CellPhasesArrayPath);
  UInt32Array& crystalStructures = m_DataStructure.getDataRefAs<UInt32Array>(m_InputValues->CrystalStructuresArrayPath);

  auto orientationOps = LaueOps::GetAllOrientationOps();

  float32 w = 0.0f;
  int64 randX = 0;
  int64 randY = 0;
  bool good = false;
  int64 x = 0, y = 0, z = 0;
  int64 col = 0, row = 0;
  usize size = 0;
  usize initialVoxelsListSize = 1000;
  float32 misorientationTolerance = m_InputValues->MisorientationTolerance * Constants::k_PiF / 180.0f;

  std::vector<int64> voxelsList(initialVoxelsListSize, -1);
  int64 neighPoints[4] = {0, 0, 0, 0};
  neighPoints[0] = -dims[0];
  neighPoints[1] = -1;
  neighPoints[2] = 1;
  neighPoints[3] = dims[0];

  uint32 phase1 = 0, phase2 = 0;
  for(int64 slice = 0; slice < dims[2]; slice++)
  {
    float32 prog = (static_cast<float32>(slice) / dims[2]) * 100;
    std::string ss = fmt::format("Aligning Sections || Identifying Features on Sections || {}% Complete", StringUtilities::number(static_cast<int>(prog)));
    m_MessageHandler(IFilter::Message::Type::Info, ss);
    featureCount = 1;
    noSeeds = false;
    while(!noSeeds)
    {
      seed = -1;
      randX = static_cast<int64>(float32(distribution(generator)) * float32(dims[0]));
      randY = static_cast<int64>(float32(distribution(generator)) * float32(dims[1]));
      for(int64 j = 0; j < dims[1]; ++j)
      {
        for(int64 i = 0; i < dims[0]; ++i)
        {
          x = randX + i;
          y = randY + j;
          z = slice;
          if(x > dims[0] - 1)
          {
            x = x - dims[0];
          }
          if(y > dims[1] - 1)
          {
            y = y - dims[1];
          }
          point = (z * dims[0] * dims[1]) + (y * dims[0]) + x;
          if((!m_InputValues->UseGoodVoxels || goodVoxels[point]) && miFeatureIds[point] == 0 && cellPhases[point] > 0)
          {
            seed = point;
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
        size = 0;
        miFeatureIds[seed] = featureCount;
        voxelsList[size] = seed;
        size++;
        for(usize j = 0; j < size; ++j)
        {
          int64 currentpoint = voxelsList[j];
          col = currentpoint % dims[0];
          row = (currentpoint / dims[0]) % dims[1];

          QuatF q1(quats[currentpoint], quats[currentpoint + 1], quats[currentpoint + 2], quats[currentpoint + 3]);
          phase1 = crystalStructures[cellPhases[currentpoint]];
          for(int32 i = 0; i < 4; i++)
          {
            good = true;
            neighbor = currentpoint + neighPoints[i];
            if((i == 0) && row == 0)
            {
              good = false;
            }
            if((i == 3) && row == (dims[1] - 1))
            {
              good = false;
            }
            if((i == 1) && col == 0)
            {
              good = false;
            }
            if((i == 2) && col == (dims[0] - 1))
            {
              good = false;
            }
            if(good && miFeatureIds[neighbor] <= 0 && cellPhases[neighbor] > 0)
            {
              w = std::numeric_limits<float32>::max();

              QuatF q2(quats[neighbor], quats[neighbor + 1], quats[neighbor + 2], quats[neighbor + 3]);
              phase2 = crystalStructures[cellPhases[neighbor]];
              if(phase1 == phase2)
              {
                OrientationF axisAngle = orientationOps[phase1]->calculateMisorientation(q1, q2);
                w = axisAngle[3];
              }
              if(w < misorientationTolerance)
              {
                miFeatureIds[neighbor] = featureCount;
                voxelsList[size] = neighbor;
                size++;
                if(std::vector<int64>::size_type(size) >= voxelsList.size())
                {
                  size = voxelsList.size();
                  voxelsList.resize(size + initialVoxelsListSize);
                  for(std::vector<int64>::size_type v = size; v < voxelsList.size(); ++v)
                  {
                    voxelsList[v] = -1;
                  }
                }
              }
            }
          }
        }
        voxelsList.erase(std::remove(voxelsList.begin(), voxelsList.end(), -1), voxelsList.end());
        featureCount++;
        voxelsList.assign(initialVoxelsListSize, -1);
      }
    }
    featureCounts[slice] = featureCount;
  }
}