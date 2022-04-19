#include "NXH5EbsdReaderWidget.h"

#include "NXComponents/interfaces/IFilter.h"
#include "NXComponents/interfaces/IParameter.h"

//-- UIC generated Header
#include "ui_NXH5EbsdReaderWidget.h"

#include <sstream>

// -----------------------------------------------------------------------------
NXH5EbsdReaderWidget::NXH5EbsdReaderWidget(NX::IFilter* filter, NX::IParameter* parameter, QFrame* parent)
: NXBaseParameterWidget(filter, parameter, parent)
, m_Ui(new Ui::NXH5EbsdReaderWidget)
, m_NxParameter(dynamic_cast<NXH5EbsdReaderParameter*>(parameter))
{
  // Sanity check that we have the proper NXParameter subclass coming in...
  if(nullptr == m_NxParameter)
  {
    std::stringstream ss;
    ss << "'" << parameter->humanName().toStdString() << "' is being interpreted as a H5EbsdReaderParameter but is not that type of parameter.";
    throw std::runtime_error(ss.str());
  }
  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
NXH5EbsdReaderWidget::~NXH5EbsdReaderWidget() = default;

// -----------------------------------------------------------------------------
void NXH5EbsdReaderWidget::setupGui()
{
}
// -----------------------------------------------------------------------------
void NXH5EbsdReaderWidget::valueUpdated(const QString& text)
{
}
