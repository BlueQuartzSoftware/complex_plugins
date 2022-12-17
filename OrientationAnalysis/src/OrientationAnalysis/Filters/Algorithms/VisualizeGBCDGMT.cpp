#include "VisualizeGBCDGMT.hpp"

#include "complex/Common/Constants.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/Utilities/Math/MatrixMath.hpp"

#include "EbsdLib/Core/Orientation.hpp"
#include "EbsdLib/Core/OrientationTransformation.hpp"
#include "EbsdLib/LaueOps/LaueOps.h"

#include <cmath>
#include <filesystem>

namespace fs = std::filesystem;

using namespace complex;

namespace
{
// -----------------------------------------------------------------------------
template <typename T>
bool GetSquareCoord(std::array<T, 3> crystalNormal, std::array<T, 2>& sqCoord)
{
  bool nhCheck = false;
  T adjust = 1.0;
  if(crystalNormal[2] >= 0.0)
  {
    adjust = -1.0;
    nhCheck = true;
  }
  if(fabs(crystalNormal[0]) >= fabs(crystalNormal[1]))
  {
    sqCoord[0] = (crystalNormal[0] / fabs(crystalNormal[0])) * sqrt(2.0 * 1.0 * (1.0 + (crystalNormal[2] * adjust))) * (Constants::k_SqrtPiD / 2.0);
    sqCoord[1] = (crystalNormal[0] / fabs(crystalNormal[0])) * sqrt(2.0 * 1.0 * (1.0 + (crystalNormal[2] * adjust))) * ((2.0 / Constants::k_SqrtPiD) * atanf(crystalNormal[1] / crystalNormal[0]));
  }
  else
  {
    sqCoord[0] = (crystalNormal[1] / fabs(crystalNormal[1])) * sqrtf(2.0 * 1.0 * (1.0 + (crystalNormal[2] * adjust))) * ((2.0f / Constants::k_SqrtPiD) * atanf(crystalNormal[0] / crystalNormal[1]));
    sqCoord[1] = (crystalNormal[1] / fabs(crystalNormal[1])) * sqrtf(2.0 * 1.0 * (1.0 + (crystalNormal[2] * adjust))) * (Constants::k_SqrtPiD / 2.0);
  }
  return nhCheck;
}
} // namespace

// -----------------------------------------------------------------------------
VisualizeGBCDGMT::VisualizeGBCDGMT(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, VisualizeGBCDGMTInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
VisualizeGBCDGMT::~VisualizeGBCDGMT() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& VisualizeGBCDGMT::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> VisualizeGBCDGMT::operator()()
{
  auto gbcd = m_DataStructure.getDataRefAs<Float64Array>(m_InputValues->GBCDArrayPath);
  auto crystalStructures = m_DataStructure.getDataRefAs<UInt32Array>(m_InputValues->CrystalStructuresArrayPath);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  const fs::path parentPath = m_InputValues->OutputFile.parent_path();
  if(!fs::exists(parentPath))
  {
    if(!fs::create_directories(parentPath))
    {
      return MakeErrorResult(-58320, fmt::format("Unable to create output directory {}", parentPath.string()));
    }
  }
  const std::ofstream outStrm(m_InputValues->OutputFile, std::ios_base::out);
  if(!outStrm.is_open())
  {
    return MakeErrorResult(-11021, fmt::format("Error creating output file {}", m_InputValues->OutputFile.string()));
  }

  std::vector<double> gbcdDeltas(5, 0);
  std::vector<double> gbcdLimits(10, 0);
  std::vector<int32> gbcdSizes(5, 0);

  // Original Ranges from Dave R.
  // gbcdLimits[0] = 0.0f;
  // gbcdLimits[1] = cosf(1.0f*m_pi);
  // gbcdLimits[2] = 0.0f;
  // gbcdLimits[3] = 0.0f;
  // gbcdLimits[4] = cosf(1.0f*m_pi);
  // gbcdLimits[5] = 2.0f*m_pi;
  // gbcdLimits[6] = cosf(0.0f);
  // gbcdLimits[7] = 2.0f*m_pi;
  // gbcdLimits[8] = 2.0f*m_pi;
  // gbcdLimits[9] = cosf(0.0f);

  // Greg R. Ranges
  gbcdLimits[0] = 0.0f;
  gbcdLimits[1] = 0.0f;
  gbcdLimits[2] = 0.0f;
  gbcdLimits[3] = -sqrtf(Constants::k_PiOver2D);
  gbcdLimits[4] = -sqrtf(Constants::k_PiOver2D);
  gbcdLimits[5] = Constants::k_PiD / 2.0f;
  gbcdLimits[6] = 1.0f;
  gbcdLimits[7] = Constants::k_PiD / 2.0f;
  gbcdLimits[8] = sqrtf(Constants::k_PiOver2D);
  gbcdLimits[9] = sqrtf(Constants::k_PiOver2D);

  // get num components of GBCD
  std::vector<usize> cDims = gbcd.getComponentShape();

  gbcdSizes[0] = static_cast<int>(cDims[0]);
  gbcdSizes[1] = static_cast<int>(cDims[1]);
  gbcdSizes[2] = static_cast<int>(cDims[2]);
  gbcdSizes[3] = static_cast<int>(cDims[3]);
  gbcdSizes[4] = static_cast<int>(cDims[4]);

  gbcdDeltas[0] = (gbcdLimits[5] - gbcdLimits[0]) / static_cast<double>(gbcdSizes[0]);
  gbcdDeltas[1] = (gbcdLimits[6] - gbcdLimits[1]) / static_cast<double>(gbcdSizes[1]);
  gbcdDeltas[2] = (gbcdLimits[7] - gbcdLimits[2]) / static_cast<double>(gbcdSizes[2]);
  gbcdDeltas[3] = (gbcdLimits[8] - gbcdLimits[3]) / static_cast<double>(gbcdSizes[3]);
  gbcdDeltas[4] = (gbcdLimits[9] - gbcdLimits[4]) / static_cast<double>(gbcdSizes[4]);

  float64 dg[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float64 dgt[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float64 dg1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float64 dg2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float64 sym1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float64 sym2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float64 sym2t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

  {
    const float32 misAngle = m_InputValues->MisorientationRotation[0] * Constants::k_PiOver180F;
    std::array<float32, 3> normAxis = {m_InputValues->MisorientationRotation[1], m_InputValues->MisorientationRotation[2], m_InputValues->MisorientationRotation[3]};
    MatrixMath::Normalize3x1(normAxis.data());
    // convert axis angle to matrix representation of misorientation
    OrientationTransformation::ax2om<OrientationD, OrientationD>(OrientationF(normAxis[0], normAxis[1], normAxis[2], misAngle)).toGMatrix(dg);
  }
  // take inverse of misorientation variable to use for switching symmetry
  MatrixMath::Transpose3x3(dg, dgt);

  // Get our LaueOps pointer for the selected crystal structure
  const LaueOps::Pointer orientOps = LaueOps::GetAllOrientationOps()[crystalStructures[m_InputValues->PhaseOfInterest]];

  // get number of symmetry operators
  const int32 nSym = orientOps->getNumSymOps();

  const usize thetaPoints = 120;
  const usize phiPoints = 30;
  const float64 thetaRes = 360.0 / static_cast<float64>(thetaPoints);
  const float64 phiRes = 90.0f / static_cast<float64>(phiPoints);
  const float64 degToRad = Constants::k_PiOver180D;

  const int32 shift1 = gbcdSizes[0];
  const int32 shift2 = gbcdSizes[0] * gbcdSizes[1];
  const int32 shift3 = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2];
  const int32 shift4 = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2] * gbcdSizes[3];

  const auto totalGBCDBins = (gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2] * gbcdSizes[3] * gbcdSizes[4] * 2);

  std::vector<double> gmtValues;
  gmtValues.reserve((phiPoints + 1) * (thetaPoints + 1)); // Allocate what should be needed.

  for(int32 phiPtIndex = 0; phiPtIndex < phiPoints + 1; phiPtIndex++)
  {
    for(int32 thetaPtIndex = 0; thetaPtIndex < thetaPoints + 1; thetaPtIndex++)
    {
      // get (x,y) for stereographic projection pixel
      const float64 theta = static_cast<float64>(thetaPtIndex) * thetaRes;
      const float64 phi = static_cast<float64>(phiPtIndex) * phiRes;
      const float64 thetaRad = theta * degToRad;
      const float64 phiRad = phi * degToRad;
      float64 sum = 0.0;
      int32 count = 0;

      std::array<double, 3> vec = {std::sin(phiRad) * std::cos(thetaRad), std::sin(phiRad) * std::sin(thetaRad), std::cos(phiRad)};
      std::array<double, 3> vec2 = {0.0, 0.0, 0.0};
      MatrixMath::Multiply3x3with3x1(dgt, vec.data(), vec2.data());

      // Loop over all the symmetry operators in the given crystal symmetry
      for(int32 i = 0; i < nSym; i++)
      {
        // get symmetry operator1
        orientOps->getMatSymOp(i, sym1);
        for(int32 j = 0; j < nSym; j++)
        {
          // get symmetry operator2
          orientOps->getMatSymOp(j, sym2);
          MatrixMath::Transpose3x3(sym2, sym2t);
          // calculate symmetric misorientation
          MatrixMath::Multiply3x3with3x3(dg, sym2t, dg1);
          MatrixMath::Multiply3x3with3x3(sym1, dg1, dg2);
          // convert to euler angle
          // OrientationD mEuler(misEuler1, 3);
          auto misEuler1 = OrientationTransformation::om2eu<OrientationD, OrientationD>(OrientationD(dg2));

          if(misEuler1[0] < Constants::k_PiOver2D && misEuler1[1] < Constants::k_PiOver2D && misEuler1[2] < Constants::k_PiOver2D)
          {
            misEuler1[1] = std::cos(misEuler1[1]);
            // find bins in GBCD
            const auto location1 = static_cast<int32>((misEuler1[0] - gbcdLimits[0]) / gbcdDeltas[0]);
            const auto location2 = static_cast<int32>((misEuler1[1] - gbcdLimits[1]) / gbcdDeltas[1]);
            const auto location3 = static_cast<int32>((misEuler1[2] - gbcdLimits[2]) / gbcdDeltas[2]);
            // find symmetric poles using the first symmetry operator
            std::array<double, 3> rotNormal = {0.0, 0.0, 0.0};
            MatrixMath::Multiply3x3with3x1(sym1, vec.data(), rotNormal.data());
            // get coordinates in square projection of crystal normal parallel to boundary normal
            // This section of code is in here so that we can essentially remove the tiny error out in the
            // 16th decimal place of the double values. This will essentially guarantee
            // the both x86_64 and ARM64 will end up returning the same square coordinates.
            const std::array<float32, 3> rotNormalF = {static_cast<float32>(rotNormal[0]), static_cast<float32>(rotNormal[1]), static_cast<float32>(rotNormal[2])};
            std::array<float32, 2> sqCoordF = {0.0F, 0.0F};

            const bool nhCheck = GetSquareCoord(rotNormalF, sqCoordF); // Result goes into the sqCoordF variable
            // Now copy the square coordinate back to the double version of the values
            // so that we can keep doing double precision calculations
            std::array<double, 3> sqCoord = {static_cast<float64>(sqCoordF[0]), static_cast<float64>(sqCoordF[1])};

            // Note the switch to have theta in the 4 slot and cos(Phi) in the 3 slot
            auto location4 = static_cast<int32>((sqCoord[0] - gbcdLimits[3]) / gbcdDeltas[3]);
            if(std::isnan(sqCoord[0]))
            {
              location4 = std::numeric_limits<int32>::min();
            }
            auto location5 = static_cast<int32>((sqCoord[1] - gbcdLimits[4]) / gbcdDeltas[4]);
            if(std::isnan(sqCoord[1]))
            {
              location5 = std::numeric_limits<int32>::min();
            }
            if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 && location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] &&
               location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
            {
              int32 hemisphere = 0;
              if(!nhCheck)
              {
                hemisphere = 1;
              }
              sum += gbcd[(m_InputValues->PhaseOfInterest * totalGBCDBins) + 2 * ((location5 * shift4) + (location4 * shift3) + (location3 * shift2) + (location2 * shift1) + location1) + hemisphere];
              count++;
            }
          }

          // again in second crystal reference frame
          // calculate symmetric misorientation
          MatrixMath::Multiply3x3with3x3(dgt, sym2, dg1);
          MatrixMath::Multiply3x3with3x3(sym1, dg1, dg2);
          // convert to euler angle
          misEuler1 = OrientationTransformation::om2eu<OrientationD, OrientationD>(OrientationD(dg2));
          if(misEuler1[0] < Constants::k_PiOver2D && misEuler1[1] < Constants::k_PiOver2D && misEuler1[2] < Constants::k_PiOver2D)
          {
            misEuler1[1] = std::cos(misEuler1[1]);
            // find bins in GBCD
            const auto location1 = static_cast<int32>((misEuler1[0] - gbcdLimits[0]) / gbcdDeltas[0]);
            const auto location2 = static_cast<int32>((misEuler1[1] - gbcdLimits[1]) / gbcdDeltas[1]);
            const auto location3 = static_cast<int32>((misEuler1[2] - gbcdLimits[2]) / gbcdDeltas[2]);
            // find symmetric poles using the first symmetry operator
            std::array<double, 3> rotNormal2 = {0.0, 0.0, 0.0};

            MatrixMath::Multiply3x3with3x1(sym1, vec2.data(), rotNormal2.data());
            // get coordinates in square projection of crystal normal parallel to boundary normal
            const std::array<float32, 3> rotNormalF = {static_cast<float32>(rotNormal2[0]), static_cast<float32>(rotNormal2[1]), static_cast<float32>(rotNormal2[2])};
            std::array<float32, 2> sqCoordF = {0.0F, 0.0F};

            const bool nhCheck = GetSquareCoord(rotNormalF, sqCoordF);
            std::array<double, 3> sqCoord = {static_cast<float64>(sqCoordF[0]), static_cast<float64>(sqCoordF[1])};

            // Note the switch to have theta in the 4 slot and cos(Phi) in the 3 slot
            auto location4 = static_cast<int32>((sqCoord[0] - gbcdLimits[3]) / gbcdDeltas[3]);
            if(std::isnan(sqCoord[0]))
            {
              location4 = std::numeric_limits<int32>::min();
            }
            auto location5 = static_cast<int32>((sqCoord[1] - gbcdLimits[4]) / gbcdDeltas[4]);
            if(std::isnan(sqCoord[1]))
            {
              location5 = std::numeric_limits<int32>::min();
            }

            if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 && location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] &&
               location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
            {
              int32 hemisphere = 0;
              if(!nhCheck)
              {
                hemisphere = 1;
              }
              sum += gbcd[(m_InputValues->PhaseOfInterest * totalGBCDBins) + 2 * ((location5 * shift4) + (location4 * shift3) + (location3 * shift2) + (location2 * shift1) + location1) + hemisphere];
              count++;
            }
          }
        }
      }
      gmtValues.push_back(theta);
      gmtValues.push_back((90.0 - phi));
      gmtValues.push_back(sum / float32(count));
    }
  }

  FILE* gmtFilePtr = fopen(m_InputValues->OutputFile.string().c_str(), "wb");
  if(nullptr == gmtFilePtr)
  {
    return MakeErrorResult(-11022, fmt::format("Error opening output file {}", m_InputValues->OutputFile.string()));
  }

  // Remember to use the original Angle in Degrees!!!!
  fprintf(gmtFilePtr, "%.1f %.1f %.1f %.1f\n", m_InputValues->MisorientationRotation[1], m_InputValues->MisorientationRotation[2], m_InputValues->MisorientationRotation[3],
          m_InputValues->MisorientationRotation[0]);
  const size_t size = gmtValues.size() / 3;

  for(size_t i = 0; i < size; i++)
  {
    fprintf(gmtFilePtr, "%f %f %f\n", gmtValues[3 * i], gmtValues[3 * i + 1], gmtValues[3 * i + 2]);
  }
  fclose(gmtFilePtr);

  return {};
}
