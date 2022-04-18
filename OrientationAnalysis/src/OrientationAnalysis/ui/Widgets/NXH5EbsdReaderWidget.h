#pragma once

#include "OrientationAnalysis/OrientationAnalysis_export.hpp"
#include "OrientationAnalysis/Parameters/H5EbsdReaderParameter.h"
#include "OrientationAnalysis/ui/Parameters/NXH5EbsdReaderParameter.h"


#include "NXComponents/ui/Parameters/NXBaseParameterWidget.h"


#include <QtCore/QSharedPointer>

#include <QtWidgets/QFrame>

namespace Ui
{
class NXH5EbsdReaderWidget;
}

namespace NX
{
class IFilter;
class IParameter;
} // namespace NX

class ORIENTATIONANALYSIS_EXPORT NXH5EbsdReaderWidget : public NXBaseParameterWidget
{
  Q_OBJECT

public:
  NXH5EbsdReaderWidget(NX::IFilter* filter, NX::IParameter* parameter, QFrame* parent = nullptr);
  ~NXH5EbsdReaderWidget() override;

  NXH5EbsdReaderWidget(const NXH5EbsdReaderWidget&) = delete;            // Copy Constructor Not Implemented
  NXH5EbsdReaderWidget(NXH5EbsdReaderWidget&&) = delete;                 // Move Constructor Not Implemented
  NXH5EbsdReaderWidget& operator=(const NXH5EbsdReaderWidget&) = delete; // Copy Assignment Not Implemented
  NXH5EbsdReaderWidget& operator=(NXH5EbsdReaderWidget&&) = delete;      // Move Assignment Not Implemented

  Q_SLOT void valueUpdated(const QString& text);

protected:
  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  void setupGui();

private:
  QSharedPointer<Ui::NXH5EbsdReaderWidget> m_Ui;
  NXH5EbsdReaderParameter* m_NxParameter = nullptr;
};
