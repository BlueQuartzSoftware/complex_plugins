#pragma once

#include "OrientationAnalysis/OrientationAnalysis_export.hpp"

#include "EbsdLib/Core/EbsdLibConstants.h"

#include "complex/Filter/ParameterTraits.hpp"
#include "complex/Filter/ValueParameter.hpp"
#include "complex/Utilities/FilePathGenerator.hpp"

#include <string>

namespace complex
{
class ORIENTATIONANALYSIS_EXPORT EbsdToH5EbsdParameter : public ValueParameter
{
public:
  struct ValueType
  {
    std::string humanLabel;
    std::string propertyName;
    std::string fileExtension = {"h5ebsd"};
    std::string fileType = QString("");
    std::vector<int> groupIndices = {};
  };

  EbsdToH5EbsdParameter() = delete;
  EbsdToH5EbsdParameter(const std::string& name, const std:; string & humanName, const std::string& helpText, const ValueType& defaultValue);
  ~EbsdToH5EbsdParameter() override = default;

  EbsdToH5EbsdParameter(const EbsdToH5EbsdParameter&) = delete;
  EbsdToH5EbsdParameter(EbsdToH5EbsdParameter&&) noexcept = delete;

  EbsdToH5EbsdParameter& operator=(const EbsdToH5EbsdParameter&) = delete;
  EbsdToH5EbsdParameter& operator=(EbsdToH5EbsdParameter&&) noexcept = delete;

  /**
   * @brief
   * @return
   */
  Uuid uuid() const override;

  /**
   * @brief
   * @return
   */
  AcceptedTypes acceptedTypes() const override;

  /**
   * @brief
   * @param value
   */
  nlohmann::json toJson(const std::any& value) const override;

  /**
   * @brief
   * @return
   */
  Result<std::any> fromJson(const nlohmann::json& json) const override;

  /**
   * @brief
   * @return
   */
  UniquePointer clone() const override;

  /**
   * @brief
   * @return
   */
  std::any defaultValue() const override;

  /**
   * @brief
   * @param value
   * @return
   */
  Result<> validate(const std::any& value) const override;

private:
  ValueType m_DefaultValue = {};
};
} // namespace complex

COMPLEX_DEF_PARAMETER_TRAITS(complex::EbsdToH5EbsdParameter, "fbc15aa6-b367-508e-bf73-94ab6be0058b");
