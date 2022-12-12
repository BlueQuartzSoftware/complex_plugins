#pragma once

#include "OrientationAnalysis/OrientationAnalysis_export.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Filter/IFilter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

namespace complex
{

struct ORIENTATIONANALYSIS_EXPORT VisualizeGBCDPoleFigureInputValues
{
  int32 PhaseOfInterest;
  VectorFloat32Parameter::ValueType MisorientationRotation;
  FileSystemPathParameter::ValueType OutputFile;
  DataPath GBCDArrayPath;
  DataPath CrystalStructuresArrayPath;
  int32 OutputImageDimension;
  DataPath ImageGeometryPath;
  std::string CellAttributeMatrixName;
  std::string CellIntensityArrayName;
};

/**
 * @class ConditionalSetValue
 * @brief This filter replaces values in the target array with a user specified value
 * where a bool mask array specifies.
 */

class ORIENTATIONANALYSIS_EXPORT VisualizeGBCDPoleFigure
{
public:
  VisualizeGBCDPoleFigure(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, VisualizeGBCDPoleFigureInputValues* inputValues);
  ~VisualizeGBCDPoleFigure() noexcept;

  VisualizeGBCDPoleFigure(const VisualizeGBCDPoleFigure&) = delete;
  VisualizeGBCDPoleFigure(VisualizeGBCDPoleFigure&&) noexcept = delete;
  VisualizeGBCDPoleFigure& operator=(const VisualizeGBCDPoleFigure&) = delete;
  VisualizeGBCDPoleFigure& operator=(VisualizeGBCDPoleFigure&&) noexcept = delete;

  Result<> operator()();

  const std::atomic_bool& getCancel();

private:
  DataStructure& m_DataStructure;
  const VisualizeGBCDPoleFigureInputValues* m_InputValues = nullptr;
  const std::atomic_bool& m_ShouldCancel;
  const IFilter::MessageHandler& m_MessageHandler;
};

} // namespace complex
