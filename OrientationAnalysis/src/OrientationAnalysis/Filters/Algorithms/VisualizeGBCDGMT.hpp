#pragma once

#include "OrientationAnalysis/OrientationAnalysis_export.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Filter/IFilter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

namespace complex
{

struct ORIENTATIONANALYSIS_EXPORT VisualizeGBCDGMTInputValues
{
  int32 PhaseOfInterest;
  VectorFloat32Parameter::ValueType MisorientationRotation;
  FileSystemPathParameter::ValueType OutputFile;
  DataPath GBCDArrayPath;
  DataPath CrystalStructuresArrayPath;
};

/**
 * @class ConditionalSetValue
 * @brief This filter replaces values in the target array with a user specified value
 * where a bool mask array specifies.
 */

class ORIENTATIONANALYSIS_EXPORT VisualizeGBCDGMT
{
public:
  VisualizeGBCDGMT(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, VisualizeGBCDGMTInputValues* inputValues);
  ~VisualizeGBCDGMT() noexcept;

  VisualizeGBCDGMT(const VisualizeGBCDGMT&) = delete;
  VisualizeGBCDGMT(VisualizeGBCDGMT&&) noexcept = delete;
  VisualizeGBCDGMT& operator=(const VisualizeGBCDGMT&) = delete;
  VisualizeGBCDGMT& operator=(VisualizeGBCDGMT&&) noexcept = delete;

  Result<> operator()();

  const std::atomic_bool& getCancel();

private:
  DataStructure& m_DataStructure;
  const VisualizeGBCDGMTInputValues* m_InputValues = nullptr;
  const std::atomic_bool& m_ShouldCancel;
  const IFilter::MessageHandler& m_MessageHandler;
};

} // namespace complex
