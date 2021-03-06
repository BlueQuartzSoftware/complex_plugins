#pragma once

#include <vector>

#include "complex/Plugin/AbstractPlugin.hpp"
#include "complex/Utilities/Parsing/HDF5/H5IDataFactory.hpp"

#include "ImportExport/ImportExport_export.hpp"

class IMPORTEXPORT_EXPORT ImportExportPlugin : public complex::AbstractPlugin
{
public:
  ImportExportPlugin();
  ~ImportExportPlugin() override;

  ImportExportPlugin(const ImportExportPlugin&) = delete;
  ImportExportPlugin(ImportExportPlugin&&) = delete;

  ImportExportPlugin& operator=(const ImportExportPlugin&) = delete;
  ImportExportPlugin& operator=(ImportExportPlugin&&) = delete;

  /**
   * @brief Returns a collection of HDF5 DataStructure factories available
   * through the plugin.
   * @return std::vector<complex::IH5DataFactory*>
   */
  std::vector<complex::H5::IDataFactory*> getDataFactories() const override;
};
