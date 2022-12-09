#include "VisualizeGBCDPoleFigure.hpp"

#include "complex/Common/Bit.hpp"
#include "complex/Common/Constants.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/Utilities/Math/MatrixMath.hpp"

#include "EbsdLib/Core/Orientation.hpp"
#include "EbsdLib/Core/OrientationTransformation.hpp"
#include "EbsdLib/LaueOps/LaueOps.h"

#include <filesystem>

namespace fs = std::filesystem;

using namespace complex;

namespace
{
// -----------------------------------------------------------------------------
bool GetSquareCoord(float* xstl1_norm1, float* sqCoord)
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
    sqCoord[0] = (xstl1_norm1[1] / fabsf(xstl1_norm1[1])) * sqrtf(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * ((2.0f / Constants::k_SqrtPiD) * atanf(xstl1_norm1[0] / xstl1_norm1[1]));
    sqCoord[1] = (xstl1_norm1[1] / fabsf(xstl1_norm1[1])) * sqrtf(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * (Constants::k_SqrtPiD / 2.0f);
  }
  return nhCheck;
}

// -----------------------------------------------------------------------------
int32 WriteCoords(FILE* f, const char* axis, const char* type, int64 npoints, float min, float step)
{
  int32 err = 0;
  fprintf(f, "%s %lld %s\n", axis, static_cast<long long int>(npoints), type);
  std::vector<float> data(npoints);
  float d;
  for(int64 idx = 0; idx < npoints; ++idx)
  {
    d = idx * step + min;
    if(endian::COMPLEX_BYTE_ORDER == endian::little)
    {
      d = byteswap(d);
    }
    data[idx] = d;
  }
  usize totalWritten = fwrite(static_cast<void*>(data.data()), sizeof(float), static_cast<usize>(npoints), f);
  if(totalWritten != static_cast<usize>(npoints))
  {
    err = -1;
  }
  return err;
}
} // namespace

// -----------------------------------------------------------------------------
VisualizeGBCDPoleFigure::VisualizeGBCDPoleFigure(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                                 VisualizeGBCDPoleFigureInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
VisualizeGBCDPoleFigure::~VisualizeGBCDPoleFigure() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& VisualizeGBCDPoleFigure::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> VisualizeGBCDPoleFigure::operator()()
{
  auto gbcd = m_DataStructure.getDataRefAs<Float64Array>(m_InputValues->GBCDArrayPath);
  auto crystalStructures = m_DataStructure.getDataRefAs<UInt32Array>(m_InputValues->CrystalStructuresArrayPath);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  fs::path parentPath = m_InputValues->OutputFile.parent_path();
  if(!fs::exists(parentPath))
  {
    if(!fs::create_directories(parentPath))
    {
      return MakeErrorResult(-23510, fmt::format("Unable to create output directory {}", parentPath.string()));
    }
  }
  std::ofstream outStrm(m_InputValues->OutputFile, std::ios_base::out);
  if(!outStrm.is_open())
  {
    return MakeErrorResult(-23511, fmt::format("Error creating output file {}", m_InputValues->OutputFile.string()));
  }

  std::vector<float32> gbcdDeltas(5, 0);
  std::vector<float32> gbcdLimits(10, 0);
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
  gbcdLimits[3] = 0.0f;
  gbcdLimits[4] = 0.0f;
  gbcdLimits[5] = Constants::k_PiOver2D;
  gbcdLimits[6] = 1.0f;
  gbcdLimits[7] = Constants::k_PiOver2D;
  gbcdLimits[8] = 1.0f;
  gbcdLimits[9] = Constants::k_2PiD;

  // reset the 3rd and 4th dimensions using the square grid approach
  gbcdLimits[3] = -sqrtf(Constants::k_PiOver2D);
  gbcdLimits[4] = -sqrtf(Constants::k_PiOver2D);
  gbcdLimits[8] = sqrtf(Constants::k_PiOver2D);
  gbcdLimits[9] = sqrtf(Constants::k_PiOver2D);

  // get num components of GBCD
  std::vector<usize> cDims = gbcd.getComponentShape();

  gbcdSizes[0] = cDims[0];
  gbcdSizes[1] = cDims[1];
  gbcdSizes[2] = cDims[2];
  gbcdSizes[3] = cDims[3];
  gbcdSizes[4] = cDims[4];

  gbcdDeltas[0] = (gbcdLimits[5] - gbcdLimits[0]) / static_cast<float>(gbcdSizes[0]);
  gbcdDeltas[1] = (gbcdLimits[6] - gbcdLimits[1]) / static_cast<float>(gbcdSizes[1]);
  gbcdDeltas[2] = (gbcdLimits[7] - gbcdLimits[2]) / static_cast<float>(gbcdSizes[2]);
  gbcdDeltas[3] = (gbcdLimits[8] - gbcdLimits[3]) / static_cast<float>(gbcdSizes[3]);
  gbcdDeltas[4] = (gbcdLimits[9] - gbcdLimits[4]) / static_cast<float>(gbcdSizes[4]);

  float vec[3] = {0.0f, 0.0f, 0.0f};
  float vec2[3] = {0.0f, 0.0f, 0.0f};
  float rotNormal[3] = {0.0f, 0.0f, 0.0f};
  float rotNormal2[3] = {0.0f, 0.0f, 0.0f};
  float sqCoord[2] = {0.0f, 0.0f};
  float dg[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float dgt[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float dg1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float dg2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float sym1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float sym2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float sym2t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float mis_euler1[3] = {0.0f, 0.0f, 0.0f};

  float misAngle = m_InputValues->MisorientationRotation[0] * Constants::k_PiOver180D;
  float normAxis[3] = {m_InputValues->MisorientationRotation[1], m_InputValues->MisorientationRotation[2], m_InputValues->MisorientationRotation[3]};
  MatrixMath::Normalize3x1(normAxis);
  // convert axis angle to matrix representation of misorientation
  OrientationTransformation::ax2om<OrientationF, OrientationF>(OrientationF(normAxis[0], normAxis[1], normAxis[2], misAngle)).toGMatrix(dg);

  // take inverse of misorientation variable to use for switching symmetry
  MatrixMath::Transpose3x3(dg, dgt);

  // Get our LaueOps pointer for the selected crystal structure
  LaueOps::Pointer orientOps = LaueOps::GetAllOrientationOps()[crystalStructures[m_InputValues->PhaseOfInterest]];

  // get number of symmetry operators
  int32 n_sym = orientOps->getNumSymOps();

  int32 xpoints = 100;
  int32 ypoints = 100;
  int32 zpoints = 1;
  int32 xpointshalf = xpoints / 2;
  int32 ypointshalf = ypoints / 2;
  float xres = 2.0f / static_cast<float>(xpoints);
  float yres = 2.0f / static_cast<float>(ypoints);
  float zres = (xres + yres) / 2.0;
  float x = 0.0f, y = 0.0f;
  float sum = 0;
  int32 count = 0;
  bool nhCheck = false;
  int32 hemisphere = 0;

  int32 shift1 = gbcdSizes[0];
  int32 shift2 = gbcdSizes[0] * gbcdSizes[1];
  int32 shift3 = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2];
  int32 shift4 = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2] * gbcdSizes[3];

  int64 totalGBCDBins = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2] * gbcdSizes[3] * gbcdSizes[4] * 2;

  std::vector<float64> poleFigure(xpoints * ypoints, 0);

  for(int32 k = 0; k < ypoints; k++)
  {
    for(int32 l = 0; l < xpoints; l++)
    {
      // get (x,y) for stereographic projection pixel
      x = static_cast<float>(l - xpointshalf) * xres + (xres / 2.0);
      y = static_cast<float>(k - ypointshalf) * yres + (yres / 2.0);
      if((x * x + y * y) <= 1.0)
      {
        sum = 0.0f;
        count = 0;
        vec[2] = -((x * x + y * y) - 1) / ((x * x + y * y) + 1);
        vec[0] = x * (1 + vec[2]);
        vec[1] = y * (1 + vec[2]);
        MatrixMath::Multiply3x3with3x1(dgt, vec, vec2);

        // Loop over all the symetry operators in the given cystal symmetry
        for(int32 i = 0; i < n_sym; i++)
        {
          // get symmetry operator1
          orientOps->getMatSymOp(i, sym1);
          for(int32 j = 0; j < n_sym; j++)
          {
            // get symmetry operator2
            orientOps->getMatSymOp(j, sym2);
            MatrixMath::Transpose3x3(sym2, sym2t);
            // calculate symmetric misorientation
            MatrixMath::Multiply3x3with3x3(dg, sym2t, dg1);
            MatrixMath::Multiply3x3with3x3(sym1, dg1, dg2);
            // convert to euler angle
            OrientationF eu(mis_euler1, 3);
            eu = OrientationTransformation::om2eu<OrientationF, OrientationF>(OrientationF(dg2));
            if(mis_euler1[0] < Constants::k_PiOver2D && mis_euler1[1] < Constants::k_PiOver2D && mis_euler1[2] < Constants::k_PiOver2D)
            {
              mis_euler1[1] = cosf(mis_euler1[1]);
              // find bins in GBCD
              int32 location1 = int32((mis_euler1[0] - gbcdLimits[0]) / gbcdDeltas[0]);
              int32 location2 = int32((mis_euler1[1] - gbcdLimits[1]) / gbcdDeltas[1]);
              int32 location3 = int32((mis_euler1[2] - gbcdLimits[2]) / gbcdDeltas[2]);
              // find symmetric poles using the first symmetry operator
              MatrixMath::Multiply3x3with3x1(sym1, vec, rotNormal);
              // get coordinates in square projection of crystal normal parallel to boundary normal
              nhCheck = GetSquareCoord(rotNormal, sqCoord);
              // Note the switch to have theta in the 4 slot and cos(Phi) int he 3 slot
              int32 location4 = int32((sqCoord[0] - gbcdLimits[3]) / gbcdDeltas[3]);
              int32 location5 = int32((sqCoord[1] - gbcdLimits[4]) / gbcdDeltas[4]);
              if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 && location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] &&
                 location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
              {
                hemisphere = 0;
                if(!nhCheck)
                {
                  hemisphere = 1;
                }
                sum +=
                    gbcd[(m_InputValues->PhaseOfInterest * totalGBCDBins) + 2 * ((location5 * shift4) + (location4 * shift3) + (location3 * shift2) + (location2 * shift1) + location1) + hemisphere];
                count++;
              }
            }

            // again in second crystal reference frame
            // calculate symmetric misorientation
            MatrixMath::Multiply3x3with3x3(dgt, sym2, dg1);
            MatrixMath::Multiply3x3with3x3(sym1, dg1, dg2);
            // convert to euler angle
            eu = OrientationTransformation::om2eu<OrientationF, OrientationF>(OrientationF(dg2));
            if(mis_euler1[0] < Constants::k_PiOver2D && mis_euler1[1] < Constants::k_PiOver2D && mis_euler1[2] < Constants::k_PiOver2D)
            {
              mis_euler1[1] = cosf(mis_euler1[1]);
              // find bins in GBCD
              int32 location1 = int32((mis_euler1[0] - gbcdLimits[0]) / gbcdDeltas[0]);
              int32 location2 = int32((mis_euler1[1] - gbcdLimits[1]) / gbcdDeltas[1]);
              int32 location3 = int32((mis_euler1[2] - gbcdLimits[2]) / gbcdDeltas[2]);
              // find symmetric poles using the first symmetry operator
              MatrixMath::Multiply3x3with3x1(sym1, vec2, rotNormal2);
              // get coordinates in square projection of crystal normal parallel to boundary normal
              nhCheck = GetSquareCoord(rotNormal2, sqCoord);
              // Note the switch to have theta in the 4 slot and cos(Phi) int he 3 slot
              int32 location4 = int32((sqCoord[0] - gbcdLimits[3]) / gbcdDeltas[3]);
              int32 location5 = int32((sqCoord[1] - gbcdLimits[4]) / gbcdDeltas[4]);
              if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 && location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] &&
                 location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
              {
                hemisphere = 0;
                if(!nhCheck)
                {
                  hemisphere = 1;
                }
                sum +=
                    gbcd[(m_InputValues->PhaseOfInterest * totalGBCDBins) + 2 * ((location5 * shift4) + (location4 * shift3) + (location3 * shift2) + (location2 * shift1) + location1) + hemisphere];
                count++;
              }
            }
          }
        }
        if(count > 0)
        {
          poleFigure[(k * xpoints) + l] = sum / static_cast<float>(count);
        }
      }
    }
  }

  FILE* f = fopen(m_InputValues->OutputFile.string().c_str(), "wb");
  if(nullptr == f)
  {
    return MakeErrorResult(-23512, fmt::format("Error opening output file {}", m_InputValues->OutputFile.string()));
  }

  // Write the correct header
  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "data set from DREAM3D\n");
  fprintf(f, "BINARY");
  fprintf(f, "\n");
  fprintf(f, "DATASET RECTILINEAR_GRID\n");
  fprintf(f, "DIMENSIONS %d %d %d\n", xpoints + 1, ypoints + 1, zpoints + 1);

  // Write the Coords
  if(WriteCoords(f, "X_COORDINATES", "float", xpoints + 1, (-static_cast<float>(xpoints) * xres / 2.0f), xres) < 0)
  {
    fclose(f);
    return MakeErrorResult(-23513, fmt::format("Error writing binary VTK data to file {}", m_InputValues->OutputFile.string()));
  }
  if(WriteCoords(f, "Y_COORDINATES", "float", ypoints + 1, (-static_cast<float>(ypoints) * yres / 2.0f), yres) < 0)
  {
    fclose(f);
    return MakeErrorResult(-23513, fmt::format("Error writing binary VTK data to file {}", m_InputValues->OutputFile.string()));
  }
  if(WriteCoords(f, "Z_COORDINATES", "float", zpoints + 1, (-static_cast<float>(zpoints) * zres / 2.0f), zres) < 0)
  {
    fclose(f);
    return MakeErrorResult(-23513, fmt::format("Error writing binary VTK data to file {}", m_InputValues->OutputFile.string()));
  }

  int32 total = xpoints * ypoints * zpoints;
  fprintf(f, "CELL_DATA %d\n", total);

  fprintf(f, "SCALARS %s %s 1\n", "Intensity", "float");
  fprintf(f, "LOOKUP_TABLE default\n");
  {
    std::vector<float> gn(total);
    float t;
    count = 0;
    for(int32 j = 0; j < ypoints; j++)
    {
      for(int32 i = 0; i < xpoints; i++)
      {
        t = static_cast<float>(poleFigure[(j * xpoints) + i]);
        if(endian::COMPLEX_BYTE_ORDER == endian::little)
        {
          t = byteswap(t);
        }
        gn[count] = t;
        count++;
      }
    }
    usize totalWritten = fwrite(gn.data(), sizeof(float), (total), f);
    if(totalWritten != (total))
    {
      fclose(f);
      return MakeErrorResult(-23513, fmt::format("Error writing binary VTK data to file {}", m_InputValues->OutputFile.string()));
    }
  }
  fclose(f);

  return {};
}
