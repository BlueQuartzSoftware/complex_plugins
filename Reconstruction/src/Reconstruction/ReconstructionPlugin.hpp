#pragma once

#include <vector>

#include "complex/Plugin/AbstractPlugin.hpp"
#include "complex/Utilities/Parsing/HDF5/H5IDataFactory.hpp"

#include "Reconstruction/Reconstruction_export.hpp"

class RECONSTRUCTION_EXPORT ReconstructionPlugin : public complex::AbstractPlugin
{
public:
  ReconstructionPlugin();
  ~ReconstructionPlugin() override;

  ReconstructionPlugin(const ReconstructionPlugin&) = delete;
  ReconstructionPlugin(ReconstructionPlugin&&) = delete;

  ReconstructionPlugin& operator=(const ReconstructionPlugin&) = delete;
  ReconstructionPlugin& operator=(ReconstructionPlugin&&) = delete;

  /**
   * @brief Returns a collection of HDF5 DataStructure factories available
   * through the plugin.
   * @return std::vector<complex::IH5DataFactory*>
   */
  std::vector<complex::H5::IDataFactory*> getDataFactories() const override;
};
