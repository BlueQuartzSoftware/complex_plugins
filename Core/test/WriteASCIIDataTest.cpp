#include <catch2/catch.hpp>

#include "Core/Filters/WirteASCIIDataFilter.hpp"
#include "ComplexCore/Filters/ImportTextFilter.hpp"

#include "complex/Common/ScopeGuard.hpp"
#include "complex/DataStructure/DataArray.hpp"
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
namespace Constants
{
    const fs::path k_TestOutput = fs::path(unit_test::k_BinaryDir.view()) / "WriteASCIIDataTest" / ; // ??? 
    constexpr int32 k_NumOfDataArrays = 3; // used for generation
    constexpr int32 k_NumComponents = 16; // used for generation
    constexpr uint64 k_OutputStyleElements = 2; //pull enum # of elements
    constexpr uint64 k_DelimiterElements = 5; //pull enum # of elements
    constexpr uint64 k_MultipleFiles = 0; //enum representation
    constexpr uint64 k_SingleFile = 1; //enum representation
    constexpr uint64 k_TabDelimiter = 1; //enum representation

} // namespace

// -----------------------------------------------------------------------------
template <class T>
class RunTest
{
public:
    //ctor 
    RunTest(DataStructure& dataStructure, FilterList* filterList, T fillValue)
    : m_DataStructure(dataStructure)
    , m_FillValue(fillValue)
    {
    }
    //virtual dtor
    ~RunTest() = default;

    void execute()
    {
        //double for loop for each choice possibility
        for(uint64 fValue = 0; fValue < Constants::k_OutputStyleElements; fValue++)
        {
            for (uint64 del = 0; del < Constants::k_DelimiterElements; del++)
            {
                test(fValue, del);
            }
        }
    }

private:
    DataStructure& m_DataStructure;
    T m_FillValue;
    const Uuid k_ComplexCorePluginId = *Uuid::FromString("05cc618b-781f-4ac0-b9ac-43f26ce1854f");
    const Uuid k_ImportTextFilterId = *Uuid::FromString("25f7df3e-ca3e-4634-adda-732c0e56efd4");
    const FilterHandle k_ImportTextFilterHandle(k_ImportTextFilterId, k_ComplexCorePluginId);

    std::vector<DataPath> CreateDataArrays() //needs a rework
    {
        std::vector<DataPath> arrayPaths(Constants::k_NumOfDataArrays);
        for(int32 index = 0; index < Constants::k_NumOfDataArrays; index ++)
        {
            DataPath array = DataPath({"array_" + index}); //verify proper construction
            auto& dataArray = m_DataStructure.getDataRefAs<DataArray<T>>(array);
            auto& dataStore = dataArray.getDataStoreRef();
            std::fill(dataStore.begin(), dataStore.end(), m_FillValue);
            arrayPaths.push_back(array);
        }
        return arrayPaths;
    }

    void readIn(fs::path filePath, Types::NumericType scalarType, uint64 numTuples, uint64 lineSkip, uint64 delimiter, Datapath path) //review tomorrow
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

        auto filter = filterList->createFilter(k_ImportTextFilterHandle);
        REQUIRE(nullptr != filter);

        Arguments args;
        //args.insertOrAssign(k_InputFileKey, std::make_any<FileSystemPathParameter::ValueType>(fs::path(fmt::format("{}/AlignSectionsFeatureCentroid_1.txt", unit_test::k_BinaryDir))));
        args.insertOrAssign(k_InputFileKey, std::make_any<FileSystemPathParameter::ValueType>(filePath));
        args.insertOrAssign(k_ScalarTypeKey, std::make_any<NumericTypeParameter::ValueType>(scalarType));
        args.insertOrAssign(k_NTuplesKey, std::make_any<uint64>(numTuples));
        args.insertOrAssign(k_NCompKey, std::make_any<uint64>(k_NumComponents));
        args.insertOrAssign(k_NSkipLinesKey, std::make_any<uint64>(lineSkip));
        args.insertOrAssign(k_DelimiterChoiceKey, std::make_any<ChoicesParameter::ValueType>(delimiter));
        args.insertOrAssign(k_DataArrayKey, std::make_any<Datapath>(path));

        // Preflight the filter and check result
        auto preflightResult = filter->preflight(m_DataStructure, args);
        COMPLEX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

        // Execute the filter and check the result
        auto executeResult = filter->execute(m_DataStructure, args);
        COMPLEX_RESULT_REQUIRE_VALID(executeResult.result)
    }

    void compareArrays(DataArray<T>& originalArray, DataArray<T>& rereadArray)
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
    }

    void CompareResults(std::vector<DataPath> daps1, std::vector<DataPath> daps2)
    {
        for(int32 index = 0; index < static_cast<int32>(daps1.size()); index++)
        {
            auto& originalArray = m_DataStructure.getDataRefAs<DataArray<T>(daps1[index]);
            auto& rereadArray = m_DataStructure.getDataRefAs<DataArray<T>(daps2[index]);
            REQUIRE(compareArrays(originalArray, rereadArray));
        }
    }

    void test(uint64 fileType, uint64 delimiter)
    {

        //create data set
        //create DataArrays and store in vector to pass as an args
        std::vector<DataPath> daps1 = CreateDataArrays();

        // Instantiate the filter and an Arguments Object
        WriteASCIIDataFilter filter;
        Arguments args;

        // Create default Parameters for the filter.
        args.insertOrAssign(WriteASCIIDataFilter::k_OutputStyle_Key, std::make_any<ChoicesParameter::ValueType>(fileType)); //uint64 0 and 1
        args.insertOrAssign(WriteASCIIDataFilter::k_OutputPath_Key, std::make_any<FileSystemPathParameter::PathType::OutputDir>(k_TestOutput));
        args.insertOrAssign(WriteASCIIDataFilter::k_FileExtension_Key, std::make_any<StringParameter>(".txt"));
        args.insertOrAssign(WriteASCIIDataFilter::k_MaxValPerLine_Key, std::make_any<Int32Parameter>(1));
        args.insertOrAssign(WriteASCIIDataFilter::k_Delimiter_Key, std::make_any<ChoicesParameter::ValueType>(delimiter)); //uint64 0 - 4 inclusive
        args.insertOrAssign(WriteASCIIDataFilter::k_SelectedDataArrayPaths_Key, std::make_any<MultiArraySelectionParameter::ValueType>(daps1));

        // Preflight the filter and check result
        auto preflightResult = filter.preflight(dataStruct, args);
        REQUIRE(preflightResult.outputActions.valid());

        //print it out to a file
        // Execute the filter and check the result
        auto executeResult = filter.execute(dataStruct, args);
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

        auto scalarType = static_cast<Types::NumericType>(type);

        //read the file(s) back in 
        std::vector<DataPath> daps2;
        //calculate numTuples
        uint64 numTuples = 
        //check multi vs single file storage 
        if(fileType == Constants::k_MultipleFiles) 
        {
            for(int32 i = 0; i < daps1.size(); i++)
            {
                std::string filePath = Constants::k_TestOutput + "array_" + i + ".txt";
                DataPath array = DataPath({"readArray_" + i}); // review???
                readIn(filePath, scalarType, numTuples, 0, delimiter, array);
                daps2.push_back(array);
            }
        }
        else if(filetype == Constants::k_SingleFile)
        {
            std::string filePath = Constants::k_TestOutput + "array_" + i + ".txt";
            uint64 lineSkip = 0;
            for(int32 i = 0; i < daps1.size(); i++)
            {
                DataPath array = DataPath({"readArray_" + i}); // review???
                readIn(filePath, scalarType, numTuples, , delimiter, array);
                daps2.push_back(array);
            }
        }

        //compare the read in to the data set
        CompareResults(daps1, daps2);
    }
}

struct make_shared_enabler : public complex::Application
{
};

TEST_CASE("Core::WriteASCIIDataFilter: Auto Generated", "[WriteASCIIDataFilter]")
{
    std::shared_ptr<make_shared_enabler> app = std::make_shared<make_shared_enabler>();
    app->loadPlugins(unit_test::k_BuildDir.view(), true);
    auto* filterList = Application::Instance()->getFilterList();

    DataStructure& dsRef = DataStructure dataStructure;

    RunTest<int8>(dsRef, filterList, -2).execute();
    RunTest<int16>(dsRef, filterList, -29).execute();
    RunTest<int32>(dsRef, filterList, -2999).execute();
    RunTest<int64>(dsRef, filterList, -2999999).execute();
    RunTest<uint8>(dsRef, filterList, 2).execute();
    RunTest<uint16>(dsRef, filterList, 29).execute();
    RunTest<uint32>(dsRef, filterList, 2999).execute();
    RunTest<uint64>(dsRef, filterList, 2999999).execute();
    RunTest<float32>(dsRef, filterList, 2.9999).execute();
    RunTest<float64>(dsRef, filterList, 2.999999999).execute(); 
} // end of test case