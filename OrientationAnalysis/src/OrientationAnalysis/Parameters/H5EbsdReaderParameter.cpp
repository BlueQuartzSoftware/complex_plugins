#include "H5EbsdReaderParameter.h"

#include "complex/Common/StringLiteral.hpp"

#include <filesystem>
#include <type_traits>

#include <fmt/core.h>

#include <nlohmann/json.hpp>

#include "complex/Common/Any.hpp"
#include "complex/Common/TypeTraits.hpp"

namespace fs = std::filesystem;

namespace complex
{
namespace
{
using EulerRepresentationUnderLyingType = std::underlying_type_t<H5EbsdReaderParameter::EulerRepresentation>;

constexpr StringLiteral k_InputFilePath = "inputFilePath";
constexpr StringLiteral k_StartSlice = "startSlice";
constexpr StringLiteral k_EndSlice = "endSlice";
constexpr StringLiteral k_UseRecommendedTransform = "useRecommendedTransform";
constexpr StringLiteral k_EulerRepresentation = "eulerRepresentation";
constexpr StringLiteral k_HDF5DataPaths = "hdf5DataPaths";

} // namespace

//-----------------------------------------------------------------------------
H5EbsdReaderParameter::H5EbsdReaderParameter(const std::string& name, const std::string& humanName, const std::string& helpText, const ValueType& defaultValue)
: ValueParameter(name, humanName, helpText)
, m_DefaultValue(defaultValue)
{
}

//-----------------------------------------------------------------------------
Uuid H5EbsdReaderParameter::uuid() const
{
  return ParameterTraits<H5EbsdReaderParameter>::uuid;
}

//-----------------------------------------------------------------------------
IParameter::AcceptedTypes H5EbsdReaderParameter::acceptedTypes() const
{
  return {typeid(ValueType)};
}

//-----------------------------------------------------------------------------
nlohmann::json H5EbsdReaderParameter::toJson(const std::any& value) const
{
  const auto& data = GetAnyRef<ValueType>(value);
  nlohmann::json json;
  json[k_InputFilePath] = data.inputFilePath;
  json[k_StartSlice] = data.startSlice;
  json[k_EndSlice] = data.endSlice;
  json[k_EulerRepresentation] = to_underlying(data.eulerRepresentation);
  json[k_UseRecommendedTransform] = data.useRecommendedTransform;

  // DataPaths
  if(!data.hdf5DataPaths.empty())
  {
    nlohmann::json dataPathsJson = nlohmann::json::array();
    for(const auto& dataPath : data.hdf5DataPaths)
    {
      dataPathsJson.push_back(dataPath);
    }
    json[k_HDF5DataPaths] = std::move(dataPathsJson);
  }
  else
  {
    json[k_HDF5DataPaths] = nullptr;
  }

  return json;
}

//-----------------------------------------------------------------------------
Result<std::any> H5EbsdReaderParameter::fromJson(const nlohmann::json& json) const
{
  static constexpr StringLiteral prefix = "FilterParameter 'H5EbsdReaderParameter' Error: ";

  if(!json.is_object())
  {
    return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Object, fmt::format("{}The JSON data entry for key '{}' is not an object.", prefix.view(), name()));
  }
  // Validate numeric json entries
  std::vector<const char*> keys = {k_StartSlice.c_str(), k_EndSlice.c_str(), k_UseRecommendedTransform.c_str()};
  for(const char* key : keys)
  {
    if(!json.contains(key))
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Missing_Entry, fmt::format("{}The JSON data does not contain an entry with a key of '{}'", prefix.view(), key));
    }
    if(!json[key].is_number_integer())
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Integer, fmt::format("{}JSON value for key '{}' is not an integer", prefix.view(), key));
    }
  }

  keys = {k_InputFilePath.c_str()};
  for(const char* key : keys)
  {
    if(!json.contains(key))
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Missing_Entry, fmt::format("{}The JSON data does not contain an entry with a key of '{}'", prefix.view(), key));
    }
    if(!json[key].is_string())
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_String, fmt::format("{}JSON value for key '{}' is not a string", prefix.view(), key));
    }
  }

  auto ordering_check = json[k_UseRecommendedTransform].get<EulerRepresentationUnderLyingType>();
  if(ordering_check != to_underlying(EulerRepresentation::Radians) && ordering_check != to_underlying(EulerRepresentation::Degrees))
  {
    return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Enumeration,
                                     fmt::format("{}JSON value for key '{}' was not a valid ordering Value. [{}|{}] allowed.", prefix.view(), k_UseRecommendedTransform.view(),
                                                 to_underlying(EulerRepresentation::Radians), to_underlying(EulerRepresentation::Degrees)));
  }

  ValueType value;

  value.inputFilePath = json[k_InputFilePath].get<std::string>();
  value.startSlice = json[k_StartSlice].get<int32>();
  value.endSlice = json[k_EndSlice].get<int32>();
  value.eulerRepresentation = static_cast<EulerRepresentation>(json[k_UseRecommendedTransform].get<EulerRepresentationUnderLyingType>());
  value.useRecommendedTransform = json[k_UseRecommendedTransform].get<bool>();

  const auto& jsonDataPaths = json[k_HDF5DataPaths];
  if(jsonDataPaths.is_null())
  {
    value.hdf5DataPaths = {};
  }
  else
  {
    if(!jsonDataPaths.is_array())
    {
      return MakeErrorResult<std::any>(-6054, fmt::format("{}JSON value for key '{} / {}' is not an array", prefix, name()));
    }
    auto dataPathStrings = jsonDataPaths.get<std::vector<std::string>>();
    std::vector<std::string> dataPaths;
    std::vector<Error> errors;
    for(const auto& dataPath : dataPathStrings)
    {
      if(dataPath.empty())
      {
        errors.push_back(Error{FilterParameter::Constants::k_Json_Value_Not_Value_Type, fmt::format("{}Failed to parse '{}' as HDF5 DataPath", prefix.view(), dataPath)});
        continue;
      }
      dataPaths.push_back(dataPath);
    }

    if(!errors.empty())
    {
      return {{nonstd::make_unexpected(std::move(errors))}};
    }

    value.hdf5DataPaths = std::move(dataPaths);
  }

  return {{std::move(value)}};
}

//-----------------------------------------------------------------------------
IParameter::UniquePointer H5EbsdReaderParameter::clone() const
{
  return std::make_unique<H5EbsdReaderParameter>(name(), humanName(), helpText(), m_DefaultValue);
}

//-----------------------------------------------------------------------------
std::any H5EbsdReaderParameter::defaultValue() const
{
  return m_DefaultValue;
}

//-----------------------------------------------------------------------------
Result<> H5EbsdReaderParameter::validate(const std::any& valueRef) const
{
  const auto& value = GetAnyRef<ValueType>(valueRef);
  if(value.startSlice >= value.endSlice)
  {
    return MakeErrorResult(-1, "startSlice must be less than endSlice");
  }

  // Validate that all HDF5 Paths exist
  std::vector<Error> errors;
  for(const auto& currentFilePath : value.hdf5DataPaths)
  {
    if(!fs::exists(currentFilePath))
    {
      errors.push_back({-2, fmt::format("FILE DOES NOT EXIST: '{}'", currentFilePath)});
    }
  }

  if(!errors.empty())
  {
    return {nonstd::make_unexpected(std::move(errors))};
  }

  return {};
}
} // namespace complex
