#pragma once

#include <vector>

#include "CoreLegacyUUIDMapping.hpp"
#include "complex/Plugin/AbstractPlugin.hpp"
#include "complex/Utilities/Parsing/HDF5/H5IDataFactory.hpp"

#include "Core/Core_export.hpp"

class CORE_EXPORT CorePlugin : public complex::AbstractPlugin
{
public:
  CorePlugin();
  ~CorePlugin() override;

  CorePlugin(const CorePlugin&) = delete;
  CorePlugin(CorePlugin&&) = delete;

  CorePlugin& operator=(const CorePlugin&) = delete;
  CorePlugin& operator=(CorePlugin&&) = delete;

  /**
   * @brief Returns a collection of HDF5 DataStructure factories available
   * through the plugin.
   * @return std::vector<complex::IH5DataFactory*>
   */
  std::vector<complex::H5::IDataFactory*> getDataFactories() const override;

  /**
   * @brief Returns a map of UUIDs as strings, where SIMPL UUIDs are keys to
   * their complex counterpart
   * @return std::map<std::string, std::string>
   */
  std::map<std::string, std::string> getSimplToComplexMap() const override
  {
    return complex::k_SIMPL_to_Core;
  }

  /**
   * @brief Returns a map of UUIDs as strings, where Complex UUIDs are keys to
   * their SIMPL counterpart(s)
   * @return std::map<std::string, std::string>
   */
  std::map<std::string, std::string> getComplexToSimplMap() const override
  {
    return complex::k_Core_to_SIMPL;
  }
};
