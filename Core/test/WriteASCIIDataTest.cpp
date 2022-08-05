#include <catch2/catch.hpp>

#include "Core/Filters/WriteASCIIDataFilter.hpp"
#include "complex/Core/Application.hpp"

#include "complex/Common/ScopeGuard.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/IDataArray.hpp"
#include "complex/DataStructure/DataStore.hpp"
#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/FileSystemPathParameter.hpp"
#include "complex/Parameters/MultiArraySelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"
#include "complex/Common/TypeTraits.hpp"
#include "complex/Common/TypesUtility.hpp"
#include "complex/Common/Types.hpp"
#include "complex/Filter/Arguments.hpp"
#include "complex/Filter/FilterHandle.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "complex/UnitTest/UnitTestCommon.hpp"

#include "complex/unit_test/complex_test_dirs.hpp"

namespace fs = std::filesystem;
using namespace complex;

/**
 * @brief The WriteASCIIDataTest class
 */
namespace
{
    const fs::path k_TestOutput = fs::path(fmt::format("{}/Testing/Temporary/WriteASCIIDataTest", unit_test::k_BuildDir));
    constexpr int32 k_NumOfDataArrays = 3; // used for generation
    constexpr int32 k_NumOfTuples = 10; // used for generation
    constexpr int32 k_NumComponents = 16; // used for generation
    constexpr uint64 k_OutputStyleElements = 2; //pull enum # of elements
    constexpr uint64 k_DelimiterElements = 5; //pull enum # of elements
    constexpr uint64 k_MultipleFiles = 0; //enum representation
    constexpr uint64 k_SingleFile = 1; //enum representation
    constexpr uint64 k_TabDelimiter = 4; //enum representation

} // namespace

// -----------------------------------------------------------------------------
template <class T>
class RunTest
{
public:
    //ctor 
    RunTest(DataStructure& dataStructure, FilterList* filterList)
    : m_DataStructure(dataStructure)
    , m_FilterList(filterList)
    {
    }
    //virtual dtor
    ~RunTest() = default;

    void execute()
    {
        //double for loop for each choice possibility
        for(uint64 fValue = 0; fValue < k_OutputStyleElements; fValue++)
        {
            for (uint64 del = 0; del < k_DelimiterElements; del++)
            {
                test(fValue, del);
            }
        }
    }

private:
    DataStructure& m_DataStructure;
    FilterList* m_FilterList;
    const Uuid k_ComplexCorePluginId = *Uuid::FromString("05cc618b-781f-4ac0-b9ac-43f26ce1854f");
    const Uuid k_ImportTextFilterId = *Uuid::FromString("25f7df3e-ca3e-4634-adda-732c0e56efd4");
    const FilterHandle k_ImportTextFilterHandle = FilterHandle(k_ImportTextFilterId, k_ComplexCorePluginId);

    std::vector<DataPath> createDataArrays() //needs a rework
    {
        std::vector<DataPath> arrayPaths(k_NumOfDataArrays);
        for(int32 index = 0; index < k_NumOfDataArrays; index ++)
        {
            auto* dataArray = UnitTest::CreateTestDataArray(m_DataStructure, "array_" + index, static_cast<DataStore<T>::ShapeType>(std::vector<T>(k_NumOfTuples)), static_cast<DataStore<T>::ShapeType>(std::vector<T>(k_NumComponents)));
            arrayPaths.push_back(DataPath(dataArray->getName()));
        }
        return arrayPaths;
    }

    void readIn(fs::path filePath, NumericType scalarType, uint64 numTuples, uint64 lineSkip, uint64 delimiter, DataPath path)
    {
        //read in using the Import Text filter
        //swap delimiter index 0 and 2 to match new filter definition
        if(delimiter == 0)
        {
            delimiter = 2;
        }
        else if(delimiter == 2)
        {
            delimiter = 0;
        }

        static constexpr StringLiteral k_InputFileKey = "input_file";
        static constexpr StringLiteral k_ScalarTypeKey = "scalar_type";
        static constexpr StringLiteral k_NTuplesKey = "n_tuples";
        static constexpr StringLiteral k_NCompKey = "n_comp";
        static constexpr StringLiteral k_NSkipLinesKey = "n_skip_lines";
        static constexpr StringLiteral k_DelimiterChoiceKey = "delimiter_choice";
        static constexpr StringLiteral k_DataArrayKey = "output_data_array";

        IFilter::UniquePointer filter = m_FilterList->createFilter(k_ImportTextFilterHandle);
        REQUIRE(filter != nullptr);

        Arguments args;
        //args.insertOrAssign(k_InputFileKey, std::make_any<FileSystemPathParameter::ValueType>(fs::path(fmt::format("{}/AlignSectionsFeatureCentroid_1.txt", unit_test::k_BinaryDir))));
        args.insertOrAssign(k_InputFileKey, std::make_any<FileSystemPathParameter::ValueType>(filePath));
        args.insertOrAssign(k_ScalarTypeKey, std::make_any<NumericType>(scalarType));
        args.insertOrAssign(k_NTuplesKey, std::make_any<uint64>(numTuples));
        args.insertOrAssign(k_NCompKey, std::make_any<uint64>(k_NumComponents));
        args.insertOrAssign(k_NSkipLinesKey, std::make_any<uint64>(lineSkip));
        args.insertOrAssign(k_DelimiterChoiceKey, std::make_any<ChoicesParameter::ValueType>(delimiter));
        args.insertOrAssign(k_DataArrayKey, std::make_any<DataPath>(path));

        // Preflight the filter and check result
        auto preflightResult = filter->preflight(m_DataStructure, args);
        COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

        // Execute the filter and check the result
        auto executeResult = filter->execute(m_DataStructure, args);
        COMPLEX_RESULT_REQUIRE_VALID(executeResult.result)
    }

    bool compareArrays(IDataArray& originalArray, IDataArray& rereadArray)
    {
        size_t numTuples = originalArray.getNumberOfTuples();
        size_t numTuples2 = rereadArray.getNumberOfTuples();
        size_t numComp = originalArray.getNumberOfComponents();
        size_t numComp2 = rereadArray.getNumberOfComponents();
        REQUIRE((numTuples == numTuples2) && (numComp == numComp2)) 
        for(size_t tup = 0; tup < numTuples; tup++)
        {
            for(size_t comp = 0; comp < numComp; comp++)
            {
                auto originalElement = originalArray[tup * numComp + comp];
                auto rereadElement = originalArray[tup * numComp + comp];
                REQUIRE(originalElement == rereadElement)
            }
        }
        return true;
    }

    void compareResults(std::vector<DataPath> daps1, std::vector<DataPath> daps2)
    {
        for(int32 index = 0; index < static_cast<int32>(daps1.size()); index++)
        {
            auto& originalArray = m_DataStructure.getDataRefAs<IDataArray>(daps1[index]);
            auto& rereadArray = m_DataStructure.getDataRefAs<IDataArray>(daps2[index]);
            REQUIRE(compareArrays(originalArray, rereadArray));
        }
    }

    void test(uint64 fileType, uint64 delimiter)
    {
        //create data set
        //create DataArrays and store in vector to pass as an args
        std::vector<DataPath> daps1 = createDataArrays();

        // Instantiate the filter and an Arguments Object
        WriteASCIIDataFilter filter;
        Arguments args;

        // Create default Parameters for the filter.
        args.insertOrAssign(WriteASCIIDataFilter::k_OutputStyle_Key, std::make_any<ChoicesParameter::ValueType>(fileType)); //uint64 0 and 1
        args.insertOrAssign(WriteASCIIDataFilter::k_OutputPath_Key, std::make_any<fs::path>(k_TestOutput));
        args.insertOrAssign(WriteASCIIDataFilter::k_FileExtension_Key, std::make_any<StringParameter>(".txt"));
        args.insertOrAssign(WriteASCIIDataFilter::k_MaxValPerLine_Key, std::make_any<Int32Parameter>(1));
        args.insertOrAssign(WriteASCIIDataFilter::k_Delimiter_Key, std::make_any<ChoicesParameter::ValueType>(delimiter)); //uint64 0 - 4 inclusive
        args.insertOrAssign(WriteASCIIDataFilter::k_SelectedDataArrayPaths_Key, std::make_any<MultiArraySelectionParameter::ValueType>(daps1));

        // Preflight the filter and check result
        auto preflightResult = filter.preflight(m_DataStructure, args);
        REQUIRE(preflightResult.outputActions.valid());

        //print it out to a file
        // Execute the filter and check the result
        auto executeResult = filter.execute(m_DataStructure, args);
        REQUIRE(executeResult.result.valid());

        uint8 type = 0;
        //"cast" T to NumericType::int8
        if(std::is_same<T, int8>::value)
        {
            type = 0;
        }
        else if(std::is_same<T, uint8>::value)
        {
            type = 1;
        }
        else if(std::is_same<T, int16>::value)
        {
            type = 2;
        }
        else if(std::is_same<T, uint16>::value)
        {
            type = 3;
        }
        else if(std::is_same<T, int32>::value)
        {
            type = 4;
        }
        else if(std::is_same<T, uint32>::value)
        {
            type = 5;
        }
        else if(std::is_same<T, int64>::value)
        {
            type = 6;
        }
        else if(std::is_same<T, uint64>::value)
        {
            type = 7;
        }
        else if(std::is_same<T, float32>::value)
        {
            type = 8;
        }
        else if(std::is_same<T, float64>::value)
        {
            type = 9;
        }
        else 
        {
            return; //invalid type
        }

        auto scalarType = static_cast<NumericType>(type);

        //read the file(s) back in 
        std::vector<DataPath> daps2;
        //calculate numTuples
        uint64 numTuples = static_cast<uint64>(k_NumOfTuples);
        //check multi vs single file storage 
        if(fileType == k_MultipleFiles) 
        {
            for(int32 i = 0; i < daps1.size(); i++)
            {
                auto& selectedArrayPtr = m_DataStructure.getDataRefAs<IDataArray>(daps1[i]);
                fs::path filePath = fs::path(k_TestOutput.string() + "/" + selectedArrayPtr.getName() + ".txt");
                DataPath array = DataPath({"readArray_" + i});
                readIn(filePath, scalarType, numTuples, 0, delimiter, array); // maybe type cast filePath to actual filePathparam?
                daps2.push_back(array);
            }
        }
        else if(fileType == k_SingleFile)
        {
            auto& selectedArrayPtr = m_DataStructure.getDataRefAs<IDataArray>(daps1[0]);
            fs::path filePath = fs::path(k_TestOutput.string() + "/" + selectedArrayPtr.getName() + ".txt");
            uint64 lineSkip = 0;
            if(delimiter == k_TabDelimiter)
            {
                lineSkip = lineSkip + k_NumOfDataArrays + 1; // headers + newline character
            }
            else
            {
                lineSkip = 2; // header + newline;
            }
            for(int32 i = 0; i < daps1.size(); i++)
            {
                DataPath array = DataPath({"readArray_" + i});
                readIn(filePath, scalarType, numTuples, lineSkip, delimiter, array);
                daps2.push_back(array);
                lineSkip = lineSkip + numTuples + 1; //add previous lines plus length of previous array and newline character
            }
        }

        //compare the read in to the data set
        compareResults(daps1, daps2);
    }
};

struct make_shared_enabler : public complex::Application
{
};

TEST_CASE("Core::WriteASCIIDataFilter: Auto Generated", "[WriteASCIIDataFilter]")
{
    std::shared_ptr<make_shared_enabler> app = std::make_shared<make_shared_enabler>();
    app->loadPlugins(unit_test::k_BuildDir.view(), true);
    auto* filterList = Application::Instance()->getFilterList();

    DataStructure dataStructure;
    DataStructure& dsRef = dataStructure;

    RunTest<int8>(dsRef, filterList).execute();
    RunTest<int16>(dsRef, filterList).execute();
    RunTest<int32>(dsRef, filterList).execute();
    RunTest<int64>(dsRef, filterList).execute();
    RunTest<uint8>(dsRef, filterList).execute();
    RunTest<uint16>(dsRef, filterList).execute();
    RunTest<uint32>(dsRef, filterList).execute();
    RunTest<uint64>(dsRef, filterList).execute();
    RunTest<float32>(dsRef, filterList).execute();
    RunTest<float64>(dsRef, filterList).execute(); 
} // end of test case