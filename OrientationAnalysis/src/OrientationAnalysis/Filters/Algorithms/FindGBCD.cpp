#include "FindGBCD.hpp"

#include "complex/Common/Constants.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/Utilities/Math/MatrixMath.hpp"
#include "complex/Utilities/ParallelDataAlgorithm.hpp"
#include "complex/Utilities/TimeUtilities.hpp"

#include "EbsdLib/Core/Orientation.hpp"
#include "EbsdLib/Core/OrientationTransformation.hpp"
#include "EbsdLib/Core/Quaternion.hpp"
#include "EbsdLib/LaueOps/LaueOps.h"

using LaueOpsShPtrType = std::shared_ptr<LaueOps>;
using LaueOpsContainer = std::vector<LaueOpsShPtrType>;

using namespace complex;

/**
 * @brief The CalculateGBCDImpl class implements a threaded algorithm that calculates the
 * grain boundary character distribution (GBCD) for a surface mesh
 */
class CalculateGBCDImpl
{
  usize m_StartOffset;
  usize m_NumEntriesPerTri;
  Int32Array& m_LabelsArray;
  Float64Array& m_NormalsArray;
  Int32Array& m_PhasesArray;
  Float32Array& m_EulersArray;
  UInt32Array& m_CrystalStructuresArray;

  SizeGBCD& m_SizeGBCD;
  LaueOpsContainer m_OrientationOps;

public:
  CalculateGBCDImpl() = delete;
  CalculateGBCDImpl(const CalculateGBCDImpl&) = default;

  CalculateGBCDImpl(usize i, usize numMisoReps, Int32Array& labels, Float64Array& normals, Float32Array& eulers, Int32Array& phases, UInt32Array& crystalStructures, SizeGBCD& sizeGBCD)
  : m_StartOffset(i)
  , m_NumEntriesPerTri(numMisoReps)
  , m_LabelsArray(labels)
  , m_NormalsArray(normals)
  , m_PhasesArray(phases)
  , m_EulersArray(eulers)
  , m_CrystalStructuresArray(crystalStructures)
  , m_SizeGBCD(sizeGBCD)
  {
    m_OrientationOps = LaueOps::GetAllOrientationOps();
  }

  CalculateGBCDImpl(CalculateGBCDImpl&&) = default;                // Move Constructor Not Implemented
  CalculateGBCDImpl& operator=(const CalculateGBCDImpl&) = delete; // Copy Assignment Not Implemented
  CalculateGBCDImpl& operator=(CalculateGBCDImpl&&) = delete;      // Move Assignment Not Implemented
  virtual ~CalculateGBCDImpl() = default;

  void generate(size_t start, size_t end) const
  {
    int32_t j = 0; //, j4;
    int32_t k = 0; //, k4;
    int32_t m = 0;
    int32_t temp = 0;
    // bool gbcd_indices_good;
    int32_t feature1 = 0, feature2 = 0;
    int32_t inversion = 1;
    float g1ea[3] = {0.0f, 0.0f, 0.0f}, g2ea[3] = {0.0f, 0.0f, 0.0f};
    float g1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, g2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float g1s[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, g2s[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float sym1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, sym2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float g2t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, dg[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float euler_mis[3] = {0.0f, 0.0f, 0.0f};
    float normal[3] = {0.0f, 0.0f, 0.0f};
    float xstl1_norm1[3] = {0.0f, 0.0f, 0.0f};
    int32_t gbcd_index = 0;
    float sqCoord[2] = {0.0f, 0.0f}, sqCoordInv[2] = {0.0f, 0.0f};
    bool nhCheck = false, nhCheckInv = true;
    int32_t SYMcounter = 0;
    auto TRIcounter = static_cast<int64_t>(start - m_StartOffset);
    int64_t TRIcounterShift = 0;

    for(size_t i = start; i < end; i++)
    {
      SYMcounter = 0;
      feature1 = m_LabelsArray[2 * i];
      feature2 = m_LabelsArray[2 * i + 1];
      normal[0] = m_NormalsArray[3 * i];
      normal[1] = m_NormalsArray[3 * i + 1];
      normal[2] = m_NormalsArray[3 * i + 2];

      if(feature1 < 0 || feature2 < 0)
      {
        continue;
      }

      if(m_PhasesArray[feature1] == m_PhasesArray[feature2] && m_PhasesArray[feature1] > 0)
      {
        TRIcounterShift = (TRIcounter * m_NumEntriesPerTri);
        uint32_t cryst = m_CrystalStructuresArray[m_PhasesArray[feature1]];
        for(int32_t q = 0; q < 2; q++)
        {
          if(q == 1)
          {
            temp = feature1;
            feature1 = feature2;
            feature2 = temp;
            normal[0] = -normal[0];
            normal[1] = -normal[1];
            normal[2] = -normal[2];
          }
          for(m = 0; m < 3; m++)
          {
            g1ea[m] = m_EulersArray[3 * feature1 + m];
            g2ea[m] = m_EulersArray[3 * feature2 + m];
          }

          OrientationTransformation::eu2om<OrientationF, OrientationF>(OrientationF(g1ea, 3)).toGMatrix(g1);

          OrientationTransformation::eu2om<OrientationF, OrientationF>(OrientationF(g2ea, 3)).toGMatrix(g2);

          int32_t nsym = m_OrientationOps[cryst]->getNumSymOps();
          for(j = 0; j < nsym; j++)
          {
            // rotate g1 by symOp
            m_OrientationOps[cryst]->getMatSymOp(j, sym1);
            MatrixMath::Multiply3x3with3x3(sym1, g1, g1s);
            // get the crystal directions along the triangle normals
            MatrixMath::Multiply3x3with3x1(g1s, normal, xstl1_norm1);
            // get coordinates in square projection of crystal normal parallel to boundary normal
            nhCheck = getSquareCoord(xstl1_norm1, sqCoord);
            if(inversion == 1)
            {
              sqCoordInv[0] = -sqCoord[0];
              sqCoordInv[1] = -sqCoord[1];
              nhCheckInv = !nhCheck;
            }

            for(k = 0; k < nsym; k++)
            {
              // calculate the symmetric misorienation
              m_OrientationOps[cryst]->getMatSymOp(k, sym2);
              // rotate g2 by symOp
              MatrixMath::Multiply3x3with3x3(sym2, g2, g2s);
              // transpose rotated g2
              MatrixMath::Transpose3x3(g2s, g2t);
              // calculate delta g
              MatrixMath::Multiply3x3with3x3(g1s, g2t, dg);
              // translate matrix to euler angles
              OrientationF om(dg);

              OrientationF eu(euler_mis, 3);
              eu = OrientationTransformation::om2eu<OrientationF, OrientationF>(om);

              if(euler_mis[0] < Constants::k_PiOver2D && euler_mis[1] < Constants::k_PiOver2D && euler_mis[2] < Constants::k_PiOver2D)
              {
                // PHI euler angle is stored in GBCD as cos(PHI)
                euler_mis[1] = cosf(euler_mis[1]);
                // get the indexes that this point would be in the GBCD histogram
                gbcd_index = GBCDIndex(m_SizeGBCD.m_GbcdDeltas, m_SizeGBCD.m_GbcdSizes, m_SizeGBCD.m_GbcdLimits, euler_mis, sqCoord);
                if(gbcd_index != -1)
                {
                  m_SizeGBCD.m_GbcdHemiCheck[TRIcounterShift + SYMcounter] = nhCheck;
                  m_SizeGBCD.m_GbcdBins[TRIcounterShift + SYMcounter] = gbcd_index;
                }
                SYMcounter++;
                if(inversion == 1)
                {
                  gbcd_index = GBCDIndex(m_SizeGBCD.m_GbcdDeltas, m_SizeGBCD.m_GbcdSizes, m_SizeGBCD.m_GbcdLimits, euler_mis, sqCoordInv);
                  if(gbcd_index != -1)
                  {
                    m_SizeGBCD.m_GbcdHemiCheck[TRIcounterShift + SYMcounter] = nhCheckInv;
                    m_SizeGBCD.m_GbcdBins[TRIcounterShift + SYMcounter] = gbcd_index;
                  }
                  SYMcounter++;
                }
              }
              else
              {
                SYMcounter += 2;
              }
            }
          }
        }
      }
      TRIcounter++;
    }
  }

  void operator()(const Range& range) const
  {
    generate(range.min(), range.max());
  }

  int32_t GBCDIndex(const std::vector<float32>& gbcddelta, const std::vector<int32>& gbcdsz, const std::vector<float32>& gbcdlimits, const float* eulerN, const float* sqCoord) const
  {
    int32_t gbcd_index;
    int32_t i = 0, index[5] = {0, 0, 0, 0, 0};
    int32_t n1 = 0, n1n2 = 0, n1n2n3 = 0, n1n2n3n4 = 0;
    int32_t flag_good = 1;
    float mis_eulerNorm[5] = {0, 0, 0, 0, 0};

    // concatonate the normalized euler angles and normalized spherical corrdinate normal
    for(i = 0; i < 3; i++)
    {
      mis_eulerNorm[i] = eulerN[i];
    }
    for(i = 0; i < 2; i++)
    {
      mis_eulerNorm[i + 3] = sqCoord[i];
    }
    // Check for a valid point in the GBCD space
    for(i = 0; i < 5; i++)
    {
      if(mis_eulerNorm[i] < gbcdlimits[i])
      {
        flag_good = 0;
      }
      if(mis_eulerNorm[i] > gbcdlimits[i + 5])
      {
        flag_good = 0;
      }
    }

    if(flag_good == 0)
    {
      return -1;
    } // does not fit in the gbcd space

    n1 = gbcdsz[0];
    n1n2 = n1 * (gbcdsz[1]);
    n1n2n3 = n1n2 * (gbcdsz[2]);
    n1n2n3n4 = n1n2n3 * (gbcdsz[3]);

    // determine the bin that the point should go into.
    for(i = 0; i < 5; i++)
    {
      index[i] = (int32_t)((mis_eulerNorm[i] - gbcdlimits[i]) / gbcddelta[i]);
      if(index[i] > (gbcdsz[i] - 1))
      {
        index[i] = (gbcdsz[i] - 1);
      }
      if(index[i] < 0)
      {
        index[i] = 0;
      }
    }

    gbcd_index = index[0] + n1 * index[1] + n1n2 * index[2] + n1n2n3 * index[3] + n1n2n3n4 * index[4];

    return gbcd_index;
  }

  bool getSquareCoord(float* xstl1_norm1, float* sqCoord) const
  {
    bool nhCheck = false;
    float adjust = 1.0;
    if(xstl1_norm1[2] >= 0.0)
    {
      adjust = -1.0;
      nhCheck = true;
    }
    if(fabsf(xstl1_norm1[0]) >= fabsf(xstl1_norm1[1]))
    {
      sqCoord[0] = (xstl1_norm1[0] / fabsf(xstl1_norm1[0])) * sqrtf(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * (Constants::k_SqrtPiD / 2.0f);
      sqCoord[1] = (xstl1_norm1[0] / fabsf(xstl1_norm1[0])) * sqrtf(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * ((2.0f / Constants::k_SqrtPiD) * atanf(xstl1_norm1[1] / xstl1_norm1[0]));
    }
    else
    {
      sqCoord[0] = (xstl1_norm1[1] / fabsf(xstl1_norm1[1])) * sqrtf(2.0 * 1.0 * (1.0 + (xstl1_norm1[2] * adjust))) * ((2.0f / Constants::k_SqrtPiD) * atanf(xstl1_norm1[0] / xstl1_norm1[1]));
      sqCoord[1] = (xstl1_norm1[1] / fabsf(xstl1_norm1[1])) * sqrtf(2.0 * 1.0 * (1.0 + (xstl1_norm1[2] * adjust))) * (Constants::k_SqrtPiD / 2.0f);
    }
    return nhCheck;
  }
};

SizeGBCD::SizeGBCD(usize faceChunkSize, usize numMisoReps, float32 gbcdRes)
: m_FaceChunkSize(faceChunkSize)
, m_NumMisoReps(numMisoReps)
{
  m_GbcdDeltas = std::vector<float32>(5, 0);
  m_GbcdLimits = std::vector<float32>(10, 0);
  m_GbcdSizes = std::vector<int32>(5, 0);
  initializeBinsWithValue(0);
  m_GbcdHemiCheck = std::vector<bool>(faceChunkSize * numMisoReps, false);

  // Original Ranges from Dave R.
  // m_GBCDlimits[0] = 0.0f;
  // m_GBCDlimits[1] = cosf(1.0f*m_pi);
  // m_GBCDlimits[2] = 0.0f;
  // m_GBCDlimits[3] = 0.0f;
  // m_GBCDlimits[4] = cosf(1.0f*m_pi);
  // m_GBCDlimits[5] = 2.0f*m_pi;
  // m_GBCDlimits[6] = cosf(0.0f);
  // m_GBCDlimits[7] = 2.0f*m_pi;
  // m_GBCDlimits[8] = 2.0f*m_pi;
  // m_GBCDlimits[9] = cosf(0.0f);

  // Greg's Ranges
  m_GbcdLimits[0] = 0.0f;
  m_GbcdLimits[1] = 0.0f;
  m_GbcdLimits[2] = 0.0f;
  m_GbcdLimits[3] = 0.0f;
  m_GbcdLimits[4] = 0.0f;
  m_GbcdLimits[5] = Constants::k_PiOver2D;
  m_GbcdLimits[6] = 1.0f;
  m_GbcdLimits[7] = Constants::k_PiOver2D;
  m_GbcdLimits[8] = 1.0f;
  m_GbcdLimits[9] = Constants::k_2PiD;

  float binsize = gbcdRes * Constants::k_PiOver180D;
  float binsize2 = binsize * (2.0 / Constants::k_PiD);
  m_GbcdDeltas[0] = binsize;
  m_GbcdDeltas[1] = binsize2;
  m_GbcdDeltas[2] = binsize;
  m_GbcdDeltas[3] = binsize2;
  m_GbcdDeltas[4] = binsize;

  m_GbcdSizes[0] = int32_t(0.5 + (m_GbcdLimits[5] - m_GbcdLimits[0]) / m_GbcdDeltas[0]);
  m_GbcdSizes[1] = int32_t(0.5 + (m_GbcdLimits[6] - m_GbcdLimits[1]) / m_GbcdDeltas[1]);
  m_GbcdSizes[2] = int32_t(0.5 + (m_GbcdLimits[7] - m_GbcdLimits[2]) / m_GbcdDeltas[2]);
  m_GbcdSizes[3] = int32_t(0.5 + (m_GbcdLimits[8] - m_GbcdLimits[3]) / m_GbcdDeltas[3]);
  m_GbcdSizes[4] = int32_t(0.5 + (m_GbcdLimits[9] - m_GbcdLimits[4]) / m_GbcdDeltas[4]);

  // reset the 3rd and 4th dimensions using the square grid approach
  float totalNormalBins = m_GbcdSizes[3] * m_GbcdSizes[4];
  m_GbcdSizes[3] = int32_t(sqrtf(totalNormalBins) + 0.5f);
  m_GbcdSizes[4] = int32_t(sqrtf(totalNormalBins) + 0.5f);
  m_GbcdLimits[3] = -sqrtf(Constants::k_PiOver2D);
  m_GbcdLimits[4] = -sqrtf(Constants::k_PiOver2D);
  m_GbcdLimits[8] = sqrtf(Constants::k_PiOver2D);
  m_GbcdLimits[9] = sqrtf(Constants::k_PiOver2D);
  m_GbcdDeltas[3] = (m_GbcdLimits[8] - m_GbcdLimits[3]) / float(m_GbcdSizes[3]);
  m_GbcdDeltas[4] = (m_GbcdLimits[9] - m_GbcdLimits[4]) / float(m_GbcdSizes[4]);
}

void SizeGBCD::initializeBinsWithValue(int32 value)
{
  m_GbcdBins = std::vector<int32>(m_FaceChunkSize * m_NumMisoReps, value);
}

// -----------------------------------------------------------------------------
FindGBCD::FindGBCD(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, FindGBCDInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
FindGBCD::~FindGBCD() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& FindGBCD::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> FindGBCD::operator()()
{
  auto& eulerAngles = m_DataStructure.getDataRefAs<Float32Array>(m_InputValues->FeatureEulerAnglesArrayPath);
  auto& phases = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->FeaturePhasesArrayPath);
  auto& crystalStructures = m_DataStructure.getDataRefAs<UInt32Array>(m_InputValues->CrystalStructuresArrayPath);
  auto& faceLabels = m_DataStructure.getDataRefAs<Int32Array>(m_InputValues->SurfaceMeshFaceLabelsArrayPath);
  auto& faceNormals = m_DataStructure.getDataRefAs<Float64Array>(m_InputValues->SurfaceMeshFaceNormalsArrayPath);
  auto& faceAreas = m_DataStructure.getDataRefAs<Float64Array>(m_InputValues->SurfaceMeshFaceAreasArrayPath);

  auto& gbcd = m_DataStructure.getDataRefAs<Float64Array>(m_InputValues->GBCDArrayName);

  usize totalPhases = crystalStructures.getNumberOfTuples();
  usize totalFaces = faceLabels.getNumberOfTuples();
  usize faceChunkSize = 50000;
  usize numMisoReps = 576 * 4;
  if(totalFaces < faceChunkSize)
  {
    faceChunkSize = totalFaces;
  }
  // call the sizeGBCD function with proper chunkSize and numMisoReps to get Bins array set up properly
  SizeGBCD sizeGbcd(faceChunkSize, numMisoReps, m_InputValues->GBCDRes);
  int32 totalGBCDBins = sizeGbcd.m_GbcdSizes[0] * sizeGbcd.m_GbcdSizes[1] * sizeGbcd.m_GbcdSizes[2] * sizeGbcd.m_GbcdSizes[3] * sizeGbcd.m_GbcdSizes[4] * 2;

  auto millis = std::chrono::steady_clock::now();
  auto currentMillis = millis;
  auto startMillis = millis;
  uint64 estimatedTime = 0;
  float timeDiff = 0.0f;
  startMillis = std::chrono::steady_clock::now();
  int32 hemisphere = 0;

  // create an array to hold the total face area for each phase and initialize the array to 0.0
  std::vector<double> totalFaceArea(totalPhases, 0.0);
  std::string ss = fmt::format("Calculating GBCD || 0/{} Completed", totalFaces);
  m_MessageHandler({IFilter::Message::Type::Info, ss});
  for(usize i = 0; i < totalFaces; i = i + faceChunkSize)
  {
    if(getCancel())
    {
      return {};
    }
    if(i + faceChunkSize >= totalFaces)
    {
      faceChunkSize = totalFaces - i;
    }
    sizeGbcd.initializeBinsWithValue(-1);

    ParallelDataAlgorithm parallelTask;
    parallelTask.setRange(i, i + faceChunkSize);
    parallelTask.execute(CalculateGBCDImpl(i, numMisoReps, faceLabels, faceNormals, eulerAngles, phases, crystalStructures, sizeGbcd));

    currentMillis = std::chrono::steady_clock::now();
    if(std::chrono::duration_cast<std::chrono::milliseconds>(currentMillis - millis).count() > 1000)
    {
      ss = fmt::format("Calculating GBCD || Triangles {}/{} Completed", i, totalFaces);
      timeDiff = static_cast<float>(i) / static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(currentMillis - startMillis).count());
      estimatedTime = (float)(totalFaces - i) / timeDiff;
      ss = ss + fmt::format(" || Est. Time Remain: {}", ConvertMillisToHrsMinSecs(estimatedTime));
      millis = std::chrono::steady_clock::now();
      m_MessageHandler({IFilter::Message::Type::Info, ss});
    }

    if(getCancel())
    {
      return {};
    }

    int32_t phase = 0;
    int32_t feature = 0;
    double area = 0.0;
    for(size_t j = 0; j < faceChunkSize; j++)
    {
      area = faceAreas[i + j];
      feature = faceLabels[2 * (i + j)];
      if(feature < 0)
      {
        continue;
      }
      phase = phases[feature];
      for(size_t k = 0; k < numMisoReps; k++)
      {
        if(sizeGbcd.m_GbcdBins[(j * numMisoReps) + (k)] >= 0)
        {
          hemisphere = 0;
          if(!sizeGbcd.m_GbcdHemiCheck[(j * numMisoReps) + k])
          {
            hemisphere = 1;
          }
          size_t gbcdBinIdx = (j * numMisoReps) + k;
          size_t gbcdIdx = (phase * totalGBCDBins) + (2 * sizeGbcd.m_GbcdBins[gbcdBinIdx] + hemisphere);
          gbcd[gbcdIdx] += area;
          totalFaceArea[phase] += area;
        }
      }
    }
  }

  m_MessageHandler({IFilter::Message::Type::Info, "Starting GBCD Normalization"});

  for(int32_t i = 0; i < totalPhases; i++)
  {
    size_t phaseShift = i * totalGBCDBins;
    double MRDfactor = double(totalGBCDBins) / totalFaceArea[i];
    for(int32_t j = 0; j < totalGBCDBins; j++)
    {
      gbcd[phaseShift + j] *= MRDfactor;
    }
  }

  return {};
}
