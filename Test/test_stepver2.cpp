#include "../StepVer2/StepVer2.h"
#include "stepdef.h"
#include <iostream>
#include <cassert>
#include <chrono>

using namespace stepver2;

// 基本功能测试
void TestBasicFunctionality()
{
    std::cout << "Testing basic functionality..." << std::endl;
    
    CachedGatePBStep step;
    
    // 测试初始化
    step.Init();
    assert(step.RecordsCount() == 0);
    
    // 测试基础记录设置
    step.SetBaseFieldValueInt(STEP_FUNC, 1001);
    step.SetBaseFieldValueString(STEP_MSG, "Test Message");
    
    std::string baseRecord = step.BaseRecord();
    assert(!baseRecord.empty());
    
    // 测试获取基础字段
    assert(step.GetBaseFieldValue(STEP_FUNC) == "1001");
    assert(step.GetBaseFieldValue(STEP_MSG) == "Test Message");
    
    std::cout << "Basic functionality test passed!" << std::endl;
}

// 记录操作测试
void TestRecordOperations()
{
    std::cout << "Testing record operations..." << std::endl;
    
    CachedGatePBStep step;
    step.Init();
    
    // 添加第一条记录
    step.AppendRecord();
    step.AddFieldValue(STEP_SCDM, "SH");
    step.AddFieldValue(STEP_HYDM, "000001");
    step.AddFieldValue(STEP_HYCS, 100);
    step.EndAppendRecord();
    
    assert(step.RecordsCount() == 1);
    
    // 添加第二条记录
    step.AppendRecord();
    step.AddFieldValue(STEP_SCDM, "SZ");
    step.AddFieldValue(STEP_HYDM, "000002");
    step.AddFieldValue(STEP_HYCS, 200);
    step.EndAppendRecord();
    
    assert(step.RecordsCount() == 2);
    
    // 测试记录导航
    step.GotoFirst();
    assert(step.GetStepValueByID(STEP_SCDM) == "SH");
    assert(step.GetStepValueByID(STEP_HYDM) == "000001");
    
    step.GotoNext();
    assert(step.GetStepValueByID(STEP_SCDM) == "SZ");
    assert(step.GetStepValueByID(STEP_HYDM) == "000002");
    
    std::cout << "Record operations test passed!" << std::endl;
}

// 序列化和反序列化测试
void TestSerialization()
{
    std::cout << "Testing serialization..." << std::endl;
    
    CachedGatePBStep step1;
    step1.Init();
    
    // 设置基础记录
    step1.SetBaseFieldValueInt(STEP_FUNC, 1001);
    step1.SetBaseFieldValueInt(STEP_CODE, 0);
    step1.SetBaseFieldValueString(STEP_MSG, "Success");
    
    // 添加记录
    step1.AppendRecord();
    step1.AddFieldValue(STEP_SCDM, "SH");
    step1.AddFieldValue(STEP_HYDM, "000001");
    step1.AddFieldValue(STEP_HYCS, 100);
    step1.EndAppendRecord();
    
    step1.AppendRecord();
    step1.AddFieldValue(STEP_SCDM, "SZ");
    step1.AddFieldValue(STEP_HYDM, "000002");
    step1.AddFieldValue(STEP_HYCS, 200);
    step1.EndAppendRecord();
    
    // 序列化
    std::string serialized = step1.ToString();
    assert(!serialized.empty());
    
    // 反序列化
    CachedGatePBStep step2;
    bool result = step2.SetPackage(serialized);
    assert(result);
    
    // 验证反序列化结果
    assert(step2.GetBaseFieldValue(STEP_FUNC) == "1001");
    assert(step2.GetBaseFieldValue(STEP_CODE) == "0");
    assert(step2.GetBaseFieldValue(STEP_MSG) == "Success");
    assert(step2.RecordsCount() == 2);
    
    step2.GotoFirst();
    assert(step2.GetStepValueByID(STEP_SCDM) == "SH");
    assert(step2.GetStepValueByID(STEP_HYDM) == "000001");
    
    step2.GotoNext();
    assert(step2.GetStepValueByID(STEP_SCDM) == "SZ");
    assert(step2.GetStepValueByID(STEP_HYDM) == "000002");
    
    std::cout << "Serialization test passed!" << std::endl;
}

// 字符转义测试
void TestEscaping()
{
    std::cout << "Testing character escaping..." << std::endl;
    
    CachedGatePBStep step;
    step.Init();
    
    // 测试包含特殊字符的字符串
    std::string testStr = "Test=Value&Another\"Line";
    step.SetBaseFieldValueString(STEP_MSG, testStr);
    
    // 验证转义和反转义
    std::string retrieved = step.GetBaseFieldValue(STEP_MSG);
    assert(retrieved == testStr);
    
    // 测试记录中的转义
    step.AppendRecord();
    step.AddFieldValue(STEP_XXNR, testStr, true); // 启用转义
    step.EndAppendRecord();
    
    step.GotoFirst();
    std::string recordValue = step.GetStepValueByID(STEP_XXNR);
    std::cout << "TestEscaping = \n" << recordValue << std::endl
              << testStr << std::endl;
    assert(recordValue == testStr);

    std::cout << "Character escaping test passed!" << std::endl;
}

// 性能测试
void TestPerformance()
{
    std::cout << "Testing performance..." << std::endl;
    
    const int recordCount = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    CachedGatePBStep step;
    step.Init();
    
    // 添加大量记录
    for (int i = 0; i < recordCount; ++i)
    {
        step.AppendRecord();
        step.AddFieldValue(STEP_SCDM, "SH");
        step.AddFieldValue(STEP_HYDM, std::to_string(i));
        step.AddFieldValue(STEP_HYCS, i * 100);
        step.AddFieldValue(STEP_WTSX, i * 1000);
        step.AddFieldValue(STEP_ZCSX, i * 2000);
        step.EndAppendRecord();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Added " << recordCount << " records in " << duration.count() << " ms" << std::endl;
    std::cout << "Records per second: " << (recordCount * 1000.0 / duration.count()) << std::endl;
    
    assert(step.RecordsCount() == recordCount);
    
    // 测试序列化性能
    start = std::chrono::high_resolution_clock::now();
    std::string serialized = step.ToString();
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Serialization took " << duration.count() << " ms" << std::endl;
    std::cout << "Serialized size: " << serialized.size() << " bytes" << std::endl;
    
    std::cout << "Performance test completed!" << std::endl;
}

int main()
{
    try
    {
        TestBasicFunctionality();
        TestRecordOperations();
        TestSerialization();
        TestEscaping();
        TestPerformance();
        
        std::cout << "\nAll tests passed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}
