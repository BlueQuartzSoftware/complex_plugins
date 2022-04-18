#pragma once

#include "OrientationAnalysis/OrientationAnalysis_export.hpp"
#include "OrientationAnalysis/Parameters/H5EbsdReaderParameter.h"

#include "NXComponents/core/CoreMacros.hpp"
#include "NXComponents/core/NXFilter.h"
#include "NXComponents/core/NXParameter.h"

#include "complex/Filter/AnyParameter.hpp"
#include "complex/Filter/IParameter.hpp"
#include "complex/Parameters/GeneratedFileListParameter.hpp"

#include <QtCore/QStringListModel>

#include <memory>

namespace NX
{
class IFilter;
}
class ORIENTATIONANALYSIS_EXPORT NXH5EbsdReaderParameter : public NXParameter
{
  Q_OBJECT

public:
  NXH5EbsdReaderParameter(NXFilter* nxParentFilter, complex::IParameter::UniquePointer& cxParameter, const complex::Arguments& filterArguments, QObject* parent = nullptr);

  ~NXH5EbsdReaderParameter() override;

  // Disable Copying
  NXH5EbsdReaderParameter(const NXH5EbsdReaderParameter&) = delete;
  NXH5EbsdReaderParameter(NXH5EbsdReaderParameter&&) noexcept = delete;
  NXH5EbsdReaderParameter& operator=(const NXH5EbsdReaderParameter&) = delete;
  NXH5EbsdReaderParameter& operator=(NXH5EbsdReaderParameter&&) noexcept = delete;

  using ComplexParameterType = complex::GeneratedFileListParameter;

  /*************
   * These will define the "types" methods that the UI uses to group the parameters together.
   */
  NX_PARAMETER_TYPE_DECLS()

  /**************
   * These are the required overrides from the superclass
   */
  QString widgetName() const override;

  std::any anyValue() const override;

  QVariant defaultValue() const override;

  /**********
   * These properties are specific to the H5EbsdParameter Parameter and are exposed through Q_PROPERTY
   * for the QML Bindings to use. If you need to expose any specifics of this class
   * to QML then create Q_PROPERTY bindings for the QML to use.
   */

#if 0
  Q_ENUM(complex::H5EbsdReaderParameter::EulerRepresentation);

  Q_PROPERTY(QString inputFilePath READ inputFilePath NOTIFY inputFilePathChanged)
  QString inputFilePath() const;
  Q_SIGNAL void inputFilePathChanged(QString);

  Q_PROPERTY(int startSlice READ startSlice NOTIFY startSliceChanged)
  int startSlice() const;
  Q_SIGNAL void startSliceChanged(int);

  Q_PROPERTY(int endSlice READ endSlice NOTIFY endSliceChanged)
  int endSlice() const;
  Q_SIGNAL void endSliceChanged(int);

  Q_PROPERTY(complex::H5EbsdReaderParameter::EulerRepresentation eulerRepresentation READ eulerRepresentation NOTIFY eulerRepresentationChanged)
  bool eulerRepresentation() const;
  Q_SIGNAL void eulerRepresentationChanged(bool);

  Q_PROPERTY(QStringListModel* hdf5DataPaths READ hdf5DataPaths NOTIFY hdf5DataPathsChanged)
  QStringListModel* hdf5DataPaths();
  Q_SIGNAL void hdf5DataPathsChanged(QStringListModel*);

  Q_PROPERTY(bool useRecommendedTransformation READ useRecommendedTransformation NOTIFY useRecommendedTransformationChanged)
  bool useRecommendedTransformation() const;
  Q_SIGNAL void useRecommendedTransformationChanged(bool);
#endif

private:
//  complex::H5EbsdReaderParameter::UniquePointer m_CxParameter;
};

