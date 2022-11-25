#pragma once

#include "Core/Core_export.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Filter/IFilter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"


/**
* This is example code to put in the Execute Method of the filter.
  ImportDeformKeyFilev12InputValues inputValues;

  inputValues.DEFORMInputFile = filterArgs.value<FileSystemPathParameter::ValueType>(k_DEFORMInputFile_Key);
  inputValues.VerboseOutput = filterArgs.value<bool>(k_VerboseOutput_Key);
  inputValues.DataContainerName = filterArgs.value<StringParameter::ValueType>(k_DataContainerName_Key);
  inputValues.VertexAttributeMatrixName = filterArgs.value<StringParameter::ValueType>(k_VertexAttributeMatrixName_Key);
  inputValues.CellAttributeMatrixName = filterArgs.value<StringParameter::ValueType>(k_CellAttributeMatrixName_Key);

  return ImportDeformKeyFilev12(dataStructure, messageHandler, shouldCancel, &inputValues)();
*/

namespace complex
{

struct CORE_EXPORT ImportDeformKeyFilev12InputValues
{
  FileSystemPathParameter::ValueType DEFORMInputFile;
  bool VerboseOutput;
  StringParameter::ValueType DataContainerName;
  StringParameter::ValueType VertexAttributeMatrixName;
  StringParameter::ValueType CellAttributeMatrixName;

};

/**
 * @class ConditionalSetValue
 * @brief This filter replaces values in the target array with a user specified value
 * where a bool mask array specifies.
 */

class CORE_EXPORT ImportDeformKeyFilev12
{
public:
  ImportDeformKeyFilev12(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, ImportDeformKeyFilev12InputValues* inputValues);
  ~ImportDeformKeyFilev12() noexcept;

  ImportDeformKeyFilev12(const ImportDeformKeyFilev12&) = delete;
  ImportDeformKeyFilev12(ImportDeformKeyFilev12&&) noexcept = delete;
  ImportDeformKeyFilev12& operator=(const ImportDeformKeyFilev12&) = delete;
  ImportDeformKeyFilev12& operator=(ImportDeformKeyFilev12&&) noexcept = delete;

  Result<> operator()();

  const std::atomic_bool& getCancel();

private:
  DataStructure& m_DataStructure;
  const ImportDeformKeyFilev12InputValues* m_InputValues = nullptr;
  const std::atomic_bool& m_ShouldCancel;
  const IFilter::MessageHandler& m_MessageHandler;
};

} // namespace complex
