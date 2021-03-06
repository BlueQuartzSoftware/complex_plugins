#pragma once

#include <vector>

#include "complex/Plugin/AbstractPlugin.hpp"
#include "complex/Utilities/Parsing/HDF5/H5IDataFactory.hpp"

#include "UCSBUtilities/UCSBUtilities_export.hpp"

class UCSBUTILITIES_EXPORT UCSBUtilitiesPlugin : public complex::AbstractPlugin
{
public:
  UCSBUtilitiesPlugin();
  ~UCSBUtilitiesPlugin() override;

  UCSBUtilitiesPlugin(const UCSBUtilitiesPlugin&) = delete;
  UCSBUtilitiesPlugin(UCSBUtilitiesPlugin&&) = delete;

  UCSBUtilitiesPlugin& operator=(const UCSBUtilitiesPlugin&) = delete;
  UCSBUtilitiesPlugin& operator=(UCSBUtilitiesPlugin&&) = delete;

  /**
   * @brief Returns a collection of HDF5 DataStructure factories available
   * through the plugin.
   * @return std::vector<complex::IH5DataFactory*>
   */
  std::vector<complex::H5::IDataFactory*> getDataFactories() const override;
};
