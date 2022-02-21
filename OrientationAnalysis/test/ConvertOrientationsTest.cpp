/**
 * This file is auto generated from the original OrientationAnalysis/ConvertOrientations
 * runtime information. These are the steps that need to be taken to utilize this
 * unit test in the proper way.
 *
 * 1: Validate each of the default parameters that gets created.
 * 2: Inspect the actual filter to determine if the filter in its default state
 * would pass or fail BOTH the preflight() and execute() methods
 * 3: UPDATE the ```REQUIRE(result.result.valid());``` code to have the proper
 *
 * 4: Add additional unit tests to actually test each code path within the filter
 *
 * There are some example Catch2 ```TEST_CASE``` sections for your inspiration.
 *
 * NOTE the format of the ```TEST_CASE``` macro. Please stick to this format to
 * allow easier parsing of the unit tests.
 *
 * When you start working on this unit test remove "[ConvertOrientations][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Common/Numbers.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"

#include "OrientationAnalysis/Filters/ConvertOrientations.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"

using namespace complex;

TEST_CASE("OrientationAnalysis::ConvertOrientations: Instantiation and Parameter Check", "[OrientationAnalysis][ConvertOrientations]")
{
#if 0
  std::vector<std::string> inRep = {"eu", "om",  "qu", "ax", "ro", "ho", "cu"};
  std::vector<std::string> outRep = {"eu", "om",  "qu", "ax", "ro", "ho", "cu"};
  std::vector<std::string> names = {  "Euler" ,
     "OrientationMatrix",
      "Quaternion",
      "AxisAngle",
      "Rodrigues",
      "Homochoric",
      "Cubochoric"};

  std::vector<int> strides = {3, 9, 4, 4, 4, 3, 3};

  for(size_t i = 0; i < 7; i++)
  {
    for(size_t o = 0; o < 7; o++)
    {
      if(inRep[i] == outRep[o]) { continue;}

      std::cout << "else if( inputType == OrientationRepresentation::Type::" << names[i]
      << " && outputType == OrientationRepresentation::Type::" << names[o] << ")\n"
      << "{\n";

      if(inRep[i] == "qu" )
      {
        std::cout << "  std::function<OutputType(QuaterionType, Quaternion<float>::Order)> " << inRep[i] << "2" << outRep[o] << " = OrientationTransformation::" << inRep[i] << "2" << outRep[o] << "<QuaterionType, OutputType>;\n";
        std::cout << "  ::FromQuaterion<float, " << strides[i] << ", " << strides[o] << ">(inputDataArray, outputDataArray, " << inRep[i] << "2" << outRep[o]
                  << ", QuaterionType::Order::VectorScalar);\n";
      }
      else if (outRep[o] == "qu")
      {
        std::cout << "  std::function<QuaterionType(InputType, Quaternion<float>::Order)> " << inRep[i] << "2" << outRep[o] << " = OrientationTransformation::" << inRep[i] << "2" << outRep[o] << "<InputType, QuaterionType>;\n";
        std::cout << "  ::ToQuaternion<float, " << strides[i] << ", " << strides[o] << ">(inputDataArray, outputDataArray, " << inRep[i] << "2" << outRep[o]
                  << ", QuaterionType::Order::VectorScalar);\n";
      }
      else
      {
        std::cout << "  std::function<OutputType(InputType)> " << inRep[i] << "2" << outRep[o] << " = OrientationTransformation::" << inRep[i] << "2" << outRep[o] << "<InputType, OutputType>;\n";
        std::cout << "  ::ConvertOrientation<float, " << strides[i] << ", " << strides[o] << ">(inputDataArray, outputDataArray, " << inRep[i] << "2" << outRep[o] << ");\n";
      }
      std::cout << "}\n";
    };
  }
#endif

  // Instantiate the filter, a DataStructure object and an Arguments Object
  ConvertOrientations filter;
  DataStructure dataGraph;
  Arguments args;

  DataGroup* topLevelGroup = DataGroup::Create(dataGraph, Constants::k_SmallIN100);
  DataGroup* scanData = DataGroup::Create(dataGraph, Constants::k_EbsdScanData, topLevelGroup->getId());

  std::vector<size_t> tupleShape = {10};
  std::vector<size_t> componentShape = {3};

  Float32Array* angles = UnitTest::CreateTestDataArray<float>(dataGraph, Constants::k_EulerAngles, tupleShape, componentShape, scanData->getId());
  for(size_t t = 0; t < tupleShape[0]; t++)
  {
    for(size_t c = 0; c < componentShape[0]; c++)
    {
      (*angles)[t * componentShape[0] + c] = static_cast<float>(t * c);
    }
  }

  // Create default Parameters for the filter.
  args.insertOrAssign(ConvertOrientations::k_InputType_Key, std::make_any<ChoicesParameter::ValueType>(0));
  args.insertOrAssign(ConvertOrientations::k_OutputType_Key, std::make_any<ChoicesParameter::ValueType>(1));
  args.insertOrAssign(ConvertOrientations::k_InputOrientationArrayPath_Key, std::make_any<DataPath>(DataPath({Constants::k_SmallIN100, Constants::k_EbsdScanData, Constants::k_EulerAngles})));
  args.insertOrAssign(ConvertOrientations::k_OutputOrientationArrayName_Key, std::make_any<DataPath>(DataPath({Constants::k_SmallIN100, Constants::k_EbsdScanData, Constants::k_AxisAngles})));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(dataGraph, args);
  //  REQUIRE(preflightResult.outputActions.valid());

  //  // Execute the filter and check the result
  auto executeResult = filter.execute(dataGraph, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("OrientationAnalysis::ConvertOrientations: Valid filter execution")
//{
//
//}

// TEST_CASE("OrientationAnalysis::ConvertOrientations: InValid filter execution")
//{
//
//}
