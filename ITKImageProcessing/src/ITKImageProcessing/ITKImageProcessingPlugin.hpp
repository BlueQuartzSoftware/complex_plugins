#pragma once

#include <vector>

#include "complex/Plugin/AbstractPlugin.hpp"
#include "complex/Utilities/Parsing/HDF5/H5IDataFactory.hpp"

#include "ITKImageProcessing/ITKImageProcessing_export.hpp"

class ITKIMAGEPROCESSING_EXPORT ITKImageProcessingPlugin : public complex::AbstractPlugin
{
public:
  ITKImageProcessingPlugin();
  ~ITKImageProcessingPlugin() noexcept override;

  ITKImageProcessingPlugin(const ITKImageProcessingPlugin&) = delete;
  ITKImageProcessingPlugin(ITKImageProcessingPlugin&&) = delete;

  ITKImageProcessingPlugin& operator=(const ITKImageProcessingPlugin&) = delete;
  ITKImageProcessingPlugin& operator=(ITKImageProcessingPlugin&&) = delete;

  static void RegisterITKImageIO();

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
  std::map<std::string, std::string> getSimplToComplexMap() const override;

  /**
   * @brief Returns a map of UUIDs as strings, where Complex UUIDs are keys to
   * their SIMPL counterpart(s)
   * @return std::map<std::string, std::string>
   */
  std::map<std::string, std::string> getComplexToSimplMap() const override;

  static std::vector<std::string> GetList2DSupportedFileExtensions();
};
