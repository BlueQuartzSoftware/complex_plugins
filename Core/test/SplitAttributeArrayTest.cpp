#include <catch2/catch.hpp>

#include "complex/DataStructure/AttributeMatrix.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"
#include "complex/UnitTest/UnitTestCommon.hpp"

#include "Core/Core_test_dirs.hpp"
#include "Core/Filters/SplitAttributeArrayFilter.hpp"

using namespace complex;

// -----------------------------------------------------------------------------
void fillDataArray(BoolArray* inputArray)
{
  usize numComps = inputArray->getNumberOfComponents();
  bool num = false;
  for(int i = 0; i < inputArray->getNumberOfTuples(); i++)
  {
    (*inputArray)[i * numComps] = num;
    (*inputArray)[i * numComps + 1] = num;
    (*inputArray)[i * numComps + 2] = num;
    num = true;
    (*inputArray)[i * numComps + 3] = num;
    num = true;
    (*inputArray)[i * numComps + 4] = num;
    num = true;
  }
}

// -----------------------------------------------------------------------------
template <typename T>
void fillDataArray(DataArray<T>* inputArray)
{
  usize numComps = inputArray->getNumberOfComponents();
  T num = 0;
  for(int i = 0; i < inputArray->getNumberOfTuples(); i++)
  {
    (*inputArray)[i * numComps] = num;
    num++;
    (*inputArray)[i * numComps + 1] = num;
    num++;
    (*inputArray)[i * numComps + 2] = num;
    num++;
    (*inputArray)[i * numComps + 3] = num;
    num++;
    (*inputArray)[i * numComps + 4] = num;
    num++;
  }
}

// -----------------------------------------------------------------------------
DataStructure createDataStructure()
{
  DataStructure dataStructure;
  AttributeMatrix* am1 = AttributeMatrix::Create(dataStructure, "AttributeMatrix");
  am1->setShape(std::vector<size_t>(1, 10));

  UInt32Array* mcArray1 = UInt32Array::CreateWithStore<DataStore<uint32>>(dataStructure, "MultiComponent Array uint32", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray<uint32>(mcArray1);

  BoolArray* mcArray2 = BoolArray::CreateWithStore<DataStore<bool>>(dataStructure, "MultiComponent Array bool", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray(mcArray2);

  // UCharArray* mcArray3 = UCharArrayType::CreateWithStore<DataStore<>>(dataStructure, "MultiComponent Array unsigned char", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  // fillDataArray<unsigned char>(mcArray3);

  Int8Array* mcArray4 = Int8Array::CreateWithStore<DataStore<int8>>(dataStructure, "MultiComponent Array int8", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray<int8>(mcArray4);

  UInt8Array* mcArray5 = UInt8Array::CreateWithStore<DataStore<uint8>>(dataStructure, "MultiComponent Array uint8", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray<uint8>(mcArray5);

  Int16Array* mcArray6 = Int16Array::CreateWithStore<DataStore<int16>>(dataStructure, "MultiComponent Array int16", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray<int16>(mcArray6);

  UInt16Array* mcArray7 = UInt16Array::CreateWithStore<DataStore<uint16>>(dataStructure, "MultiComponent Array uint16", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray<uint16>(mcArray7);

  Int32Array* mcArray8 = Int32Array::CreateWithStore<DataStore<int32>>(dataStructure, "MultiComponent Array int32", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray<int32>(mcArray8);

  Int64Array* mcArray9 = Int64Array::CreateWithStore<DataStore<int64>>(dataStructure, "MultiComponent Array int64", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray<int64>(mcArray9);

  UInt64Array* mcArray10 = UInt64Array::CreateWithStore<DataStore<uint64>>(dataStructure, "MultiComponent Array uint64", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray<uint64>(mcArray10);

  Float32Array* mcArray11 = Float32Array::CreateWithStore<DataStore<float32>>(dataStructure, "MultiComponent Array float", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray<float32>(mcArray11);

  Float64Array* mcArray12 = Float64Array::CreateWithStore<DataStore<float64>>(dataStructure, "MultiComponent Array double", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray<float64>(mcArray12);

  USizeArray* mcArray13 = USizeArray::CreateWithStore<DataStore<usize>>(dataStructure, "MultiComponent Array size_t", std::vector<size_t>(1, 10), std::vector<size_t>(1, 5), am1->getId());
  fillDataArray<usize>(mcArray13);

  return dataStructure;
}

// -----------------------------------------------------------------------------
template <typename T>
void TestSplitByType(DataStructure& ds, std::string dataType)
{
  SplitAttributeArrayFilter filter;

  DataPath arrayPath({"AttributeMatrix", "MultiComponent Array " + dataType});

  Arguments args;
  // read in the exemplar shift data file
  args.insertOrAssign(SplitAttributeArrayFilter::k_MultiCompArray_Key, std::make_any<DataPath>(arrayPath));
  args.insertOrAssign(SplitAttributeArrayFilter::k_Postfix_Key, std::make_any<std::string>("Component"));

  auto executeResults = filter.execute(ds, args);
  COMPLEX_RESULT_REQUIRE_VALID(executeResults.result);
  using DataArrayPtrType = DataArray<T>*;
  DataArray<T>& mcArray_original = ds.getDataRefAs<DataArray<T>>(DataPath({"AttributeMatrix", "MultiComponent Array " + dataType}));

  std::vector<DataArrayPtrType> mcArraysAfterSplit;
  DataArrayPtrType mcArray0 = ds.getDataAs<DataArray<T>>(DataPath({"AttributeMatrix", "MultiComponent Array " + dataType + "Component0"}));
  mcArraysAfterSplit.push_back(mcArray0);
  DataArrayPtrType mcArray1 = ds.getDataAs<DataArray<T>>(DataPath({"AttributeMatrix", "MultiComponent Array " + dataType + "Component1"}));
  mcArraysAfterSplit.push_back(mcArray1);
  DataArrayPtrType mcArray2 = ds.getDataAs<DataArray<T>>(DataPath({"AttributeMatrix", "MultiComponent Array " + dataType + "Component2"}));
  mcArraysAfterSplit.push_back(mcArray2);
  DataArrayPtrType mcArray3 = ds.getDataAs<DataArray<T>>(DataPath({"AttributeMatrix", "MultiComponent Array " + dataType + "Component3"}));
  mcArraysAfterSplit.push_back(mcArray3);
  DataArrayPtrType mcArray4 = ds.getDataAs<DataArray<T>>(DataPath({"AttributeMatrix", "MultiComponent Array " + dataType + "Component4"}));
  mcArraysAfterSplit.push_back(mcArray4);

  usize numTuples = mcArray_original.getNumberOfTuples();
  usize numComps = mcArray_original.getNumberOfComponents();
  for(int i = 0; i < numTuples; i++)
  {
    for(int j = 0; j < numComps; j++)
    {
      T originalValue = mcArray_original[i * numComps + j];
      T afterSplitValue = (*mcArraysAfterSplit[j])[i];
      REQUIRE(originalValue == afterSplitValue);
    }
  }
}

// -----------------------------------------------------------------------------
TEST_CASE("Core::SplitAttributeArray: Instantiation and Parameter Check", "[Core][SplitAttributeArrayFilter]")
{
  DataStructure ds = createDataStructure();

  TestSplitByType<uint32>(ds, "uint32");
  TestSplitByType<bool>(ds, "bool");
  // TestSplitByType<unsigned char>(ds, "unsigned char");
  TestSplitByType<int8>(ds, "int8");
  TestSplitByType<uint8>(ds, "uint8");
  TestSplitByType<int16>(ds, "int16");
  TestSplitByType<uint16>(ds, "uint16");
  TestSplitByType<int32>(ds, "int32");
  TestSplitByType<int64>(ds, "int64");
  TestSplitByType<uint64>(ds, "uint64");
  TestSplitByType<float>(ds, "float");
  TestSplitByType<double>(ds, "double");
  TestSplitByType<usize>(ds, "size_t");
}
