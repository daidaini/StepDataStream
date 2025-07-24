#include "../StepVer2/StepVer2.h"
#include "stepdef.h"
#include <iostream>
#include <chrono>
#include <cassert>
#include <vector>

using namespace stepver2;

// 测试数据结构
struct TestRecord
{
    std::string scdm;
    std::string hydm;
    int hycs;
    int wtsx;
    int zcsx;
    double zxbdjw;
    double dwbzj;
    int dqr;
    int xqrq;
    std::string bddm;
    std::string bdmc;
    double xqjg;
};

// 生成测试数据
std::vector<TestRecord> GenerateTestData(int count)
{
    std::vector<TestRecord> data;
    data.reserve(count);
    
    for (int i = 0; i < count; ++i)
    {
        TestRecord record;
        record.scdm = (i % 2 == 0) ? "SH" : "SZ";
        record.hydm = "00000" + std::to_string(i % 10000);
        record.hycs = 100 + i;
        record.wtsx = 1000 + i * 10;
        record.zcsx = 2000 + i * 20;
        record.zxbdjw = 0.01 + i * 0.001;
        record.dwbzj = 1000.0 + i * 10.0;
        record.dqr = 20250101 + i;
        record.xqrq = 20251231 + i;
        record.bddm = "BD" + std::to_string(i % 1000);
        record.bdmc = "Stock" + std::to_string(i % 1000);
        record.xqjg = 10.0 + i * 0.1;
        
        data.push_back(record);
    }
    
    return data;
}

// 使用新的StepVer2进行性能测试
double TestNewImplementation(const std::vector<TestRecord> &data)
{
    auto start = std::chrono::high_resolution_clock::now();

    StepVer2 step;
    step.Init();
    
    // 设置基础记录
    step.SetBaseFieldValueInt(STEP_FUNC, 1001);
    step.SetBaseFieldValueInt(STEP_CODE, 0);
    step.SetBaseFieldValueString(STEP_MSG, "Success");
    
    // 添加记录
    for (const auto &record : data)
    {
        step.AppendRecord();
        step.AddFieldValue(STEP_SCDM, record.scdm);
        step.AddFieldValue(STEP_HYDM, record.hydm);
        step.AddFieldValue(STEP_HYCS, record.hycs);
        step.AddFieldValue(STEP_WTSX, record.wtsx);
        step.AddFieldValue(STEP_ZCSX, record.zcsx);
        step.AddFieldValue(STEP_ZXBDJW, record.zxbdjw);
        step.AddFieldValue(STEP_DWBZJ, record.dwbzj);
        step.AddFieldValue(STEP_DQR, record.dqr);
        step.AddFieldValue(STEP_XQRQ, record.xqrq);
        step.AddFieldValue(STEP_BDDM, record.bddm);
        step.AddFieldValue(STEP_BDMC, record.bdmc);
        step.AddFieldValue(STEP_XQJG, record.xqjg);
        step.EndAppendRecord();
    }
    
    // 序列化
    std::string serialized = step.ToString();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "New implementation:" << std::endl;
    std::cout << "  Records: " << step.RecordsCount() << std::endl;
    std::cout << "  Serialized size: " << serialized.size() << " bytes" << std::endl;
    std::cout << "  Time: " << duration.count() << " microseconds" << std::endl;
    
    return duration.count();
}

// 测试序列化和反序列化的完整性
void TestSerializationIntegrity()
{
    std::cout << "\nTesting serialization integrity..." << std::endl;
    
    // 创建测试数据
    std::vector<TestRecord> testData = GenerateTestData(100);
    
    StepVer2 step1;
    step1.Init();
    step1.SetBaseFieldValueInt(STEP_FUNC, 1001);
    step1.SetBaseFieldValueString(STEP_MSG, "Test Message with special chars: =&\n");
    
    for (const auto &record : testData)
    {
        step1.AppendRecord();
        step1.AddFieldValue(STEP_SCDM, record.scdm);
        step1.AddFieldValue(STEP_HYDM, record.hydm);
        step1.AddFieldValue(STEP_HYCS, record.hycs);
        step1.EndAppendRecord();
    }
    
    std::string serialized = step1.ToString();
    
    // 反序列化
    StepVer2 step2;
    bool parseResult = step2.SetPackage(serialized);
    assert(parseResult);
    
    // 验证数据一致性
    assert(step2.GetBaseFieldValue(STEP_FUNC) == "1001");
    assert(step2.GetBaseFieldValue(STEP_MSG) == "Test Message with special chars: =&\n");
    assert(step2.RecordsCount() == static_cast<int>(testData.size()));
    
    // 验证记录内容
    step2.GotoFirst();
    for (size_t i = 0; i < testData.size(); ++i)
    {
        assert(step2.GetStepValueByID(STEP_SCDM) == testData[i].scdm);
        assert(step2.GetStepValueByID(STEP_HYDM) == testData[i].hydm);
        assert(std::stoi(step2.GetStepValueByID(STEP_HYCS)) == testData[i].hycs);
        
        if (i < testData.size() - 1)
        {
            step2.GotoNext();
        }
    }
    
    std::cout << "Serialization integrity test passed!" << std::endl;
}

// 内存使用测试
void TestMemoryUsage()
{
    std::cout << "\nTesting memory usage..." << std::endl;
    
    StepVer2 step;
    step.Init();
    
    const int recordCount = 50000;
    auto testData = GenerateTestData(recordCount);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (const auto &record : testData)
    {
        step.AppendRecord();
        step.AddFieldValue(STEP_SCDM, record.scdm);
        step.AddFieldValue(STEP_HYDM, record.hydm);
        step.AddFieldValue(STEP_HYCS, record.hycs);
        step.AddFieldValue(STEP_WTSX, record.wtsx);
        step.AddFieldValue(STEP_ZCSX, record.zcsx);
        step.EndAppendRecord();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::string serialized = step.ToString();
    
    std::cout << "Memory usage test results:" << std::endl;
    std::cout << "  Records processed: " << recordCount << std::endl;
    std::cout << "  Processing time: " << duration.count() << " ms" << std::endl;
    std::cout << "  Final serialized size: " << serialized.size() << " bytes" << std::endl;
    std::cout << "  Average bytes per record: " << (serialized.size() / recordCount) << std::endl;
    std::cout << "  Records per second: " << (recordCount * 1000.0 / duration.count()) << std::endl;
}

int main()
{
    try
    {
        std::cout << "StepVer2 Performance and Functionality Test" << std::endl;
        std::cout << "=============================================" << std::endl;
        
        // 测试不同数据量的性能
        std::vector<int> testSizes = {100, 1000, 5000, 10000};
        
        for (int size : testSizes)
        {
            std::cout << "\nTesting with " << size << " records:" << std::endl;
            
            auto testData = GenerateTestData(size);
            double newTime = TestNewImplementation(testData);
            
            std::cout << "  Performance: " << (size * 1000000.0 / newTime) << " records/second" << std::endl;
        }
        
        // 完整性测试
        TestSerializationIntegrity();
        
        // 内存使用测试
        TestMemoryUsage();
        
        std::cout << "\nAll tests completed successfully!" << std::endl;
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
