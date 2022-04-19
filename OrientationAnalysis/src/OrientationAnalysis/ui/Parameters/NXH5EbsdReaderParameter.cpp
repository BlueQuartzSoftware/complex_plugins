#include "NXH5EbsdReaderParameter.h"

#include "NXComponents/core/NXParameterManager.h"
#include "NXComponents/interfaces/IFilter.h"
#include "NXComponents/utils/NXQtUtils.hpp"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <filesystem>

using namespace NX;

#if 0
// -----------------------------------------------------------------------------
void NXH5EbsdReaderParameter::initialize(complex::GeneratedFileListParameter::ValueType const& v)
{
  m_startIndex = v.startIndex;
  m_endIndex = v.endIndex;
  m_incrementIndex = v.incrementIndex;
  m_paddingDigits = v.paddingDigits;
  m_inputPath = QString::fromStdString(v.inputPath);
  m_filePrefix = QString::fromStdString(v.filePrefix);
  m_fileSuffix = QString::fromStdString(v.fileSuffix);
  m_fileExtension = QString::fromStdString(v.fileExtension);
  m_sortAscending = (v.ordering == complex::GeneratedFileListParameter::Ordering::LowToHigh) ? true : false;

}

// -----------------------------------------------------------------------------
std::any NXH5EbsdReaderParameter::anyValue()
{
  complex::GeneratedFileListParameter::ValueType complexValue;
  complexValue.startIndex = m_startIndex;
  complexValue.endIndex = m_endIndex;
  complexValue.incrementIndex = m_incrementIndex;
  complexValue.paddingDigits = m_paddingDigits;
  complexValue.inputPath = m_inputPath.toStdString();
  complexValue.filePrefix = m_filePrefix.toStdString();
  complexValue.fileSuffix = m_fileSuffix.toStdString();
  complexValue.fileExtension = m_fileExtension.toStdString();

  complexValue.ordering = ((m_sortAscending == true) ? complex::GeneratedFileListParameter::Ordering::LowToHigh :
                                                       complex::GeneratedFileListParameter::Ordering::HighToLow);

  return std::make_any<complex::GeneratedFileListParameter::ValueType>(complexValue);
}
#endif

// -----------------------------------------------------------------------------
NXH5EbsdReaderParameter::NXH5EbsdReaderParameter(NXFilter* nxParentFilter, complex::IParameter::UniquePointer& cxParameter, const complex::Arguments& filterArgs, QObject* parent)
: NXParameter(nxParentFilter, cxParameter, filterArgs, parent)
{
  complex::H5EbsdReaderParameter* complexParameter = dynamic_cast<complex::H5EbsdReaderParameter*>(getComplexParameter());
  // Check if the dynamic_cast<> worked;
  if(nullptr == complexParameter)
  {
    throw std::runtime_error("Unable to dynamic_cast<> from IParameter* to NXH5EbsdReaderParameter*");
  }
  // Check if we have an argument coming from a pipeline file or we are just using the default value
  // assigned to the Parameter by the filter itself.
  std::string paramName = complexParameter->name();
  if(filterArgs.contains(paramName))
  {
    m_DefaultValue = filterArgs.at(paramName);
  }
  else
  {
    m_DefaultValue = complexParameter->defaultValue();
  }
  // Now set the actual value of the Parameter from the Default Value
  //  NXH5EbsdReaderParameter nxValueType;
  //  nxValueType.initialize(std::any_cast<complex::GeneratedFileListParameter::ValueType>(m_DefaultValue));
  //  m_Value = QVariant::fromValue(nxValueType);
}

// -----------------------------------------------------------------------------
NXH5EbsdReaderParameter::~NXH5EbsdReaderParameter() = default;

// -----------------------------------------------------------------------------
NX_PARAMETER_TYPE_DEFS(NXH5EbsdReaderParameter, false, false, false, false)

// -----------------------------------------------------------------------------
QString NXH5EbsdReaderParameter::widgetName() const
{
  return "NXH5EbsdReaderWidget";
}

// -----------------------------------------------------------------------------
std::any NXH5EbsdReaderParameter::anyValue() const
{
  complex::H5EbsdReaderParameter::ValueType parameterValue;

  return std::make_any<complex::H5EbsdReaderParameter::ValueType>(parameterValue);
}

// -----------------------------------------------------------------------------
QVariant NXH5EbsdReaderParameter::defaultValue() const
{
  //  complex::H5EbsdReaderParameter* complexParameter = dynamic_cast<complex::H5EbsdReaderParameter*>(getComplexParameter());
  //  NXH5EbsdReaderParameter nxValueType;
  //  nxValueType.initialize(std::any_cast<complex::H5EbsdReaderParameter::ValueType>(complexParameter->defaultValue()));
  //  return QVariant::fromValue(nxValueType);

  return {};
}

#if 0
// -----------------------------------------------------------------------------
bool NXH5EbsdReaderParameter::inputFilePath() const
{
  return m_InputFilePath;
}

// -----------------------------------------------------------------------------
QString NXH5EbsdReaderParameter::startSlice() const
{
  return m_StartSlice;
}

// -----------------------------------------------------------------------------
int NXH5EbsdReaderParameter::endSlice() const
{
  return m_EndSlice;
}


// -----------------------------------------------------------------------------
int NXH5EbsdReaderParameter::endSlice() const
{
  return m_EndSlice;
}


// -----------------------------------------------------------------------------
complex::H5EbsdReaderParameter::EulerRepresentation NXH5EbsdReaderParameter::eulerRepresentation() const
{
  return m_EulerRepresentation;
}


// -----------------------------------------------------------------------------
QStringListModel* NXH5EbsdReaderParameter::hdf5DataPaths() const
{
  return m_Hdf5DataPaths;
}

// -----------------------------------------------------------------------------
bool NXH5EbsdReaderParameter::useRecommendedTransformation() const
{
  return m_UseRecommendedTransformation;
}
#endif
