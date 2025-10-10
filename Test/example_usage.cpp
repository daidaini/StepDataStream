/*
 * @Description: StepVer2使用示例
 * @Author: yubo
 * @Date: 2025-01-24
 */

#include "../StepVer2/StepVer2.h"
#include "stepdef.h"
#include <iostream>
#include <string>
#include <chrono>

using namespace stepver2;

void BasicUsageExample()
{
    std::cout << "=== 基本使用示例 ===" << std::endl;

    // 创建StepVer2实例
    GatePBStep step;

    // 初始化
    step.Init();

    // 设置基础字段
    step.SetBaseFieldValueInt(STEP_FUNC, 1001);        // 功能号
    step.SetBaseFieldValueInt(STEP_CODE, 0);           // 错误代码
    step.SetBaseFieldValueString(STEP_MSG, "Success"); // 错误信息
    step.SetBaseFieldValueInt(STEP_RETURNNUM, 2);      // 返回记录数
    step.SetBaseFieldValueInt(STEP_TOTALNUM, 2);       // 总记录数

    // 添加第一条记录
    step.AppendRecord();
    step.AddFieldValue(STEP_SCDM, "SH");         // 市场代码
    step.AddFieldValue(STEP_HYDM, "000001");     // 合约代码
    step.AddFieldValue(STEP_HYDMMC, "平安银行"); // 合约名称
    step.AddFieldValue(STEP_HYCS, 100);          // 合约乘数
    step.AddFieldValue(STEP_WTSX, 10000);        // 委托上限
    step.AddFieldValue(STEP_ZCSX, 20000);        // 最大上限
    step.AddFieldValue(STEP_ZXBDJW, 0.01);       // 最小变动价位
    step.AddFieldValue(STEP_DWBZJ, 1000.50);     // 单位保证金
    step.EndAppendRecord();

    // 添加第二条记录
    step.AppendRecord();
    step.AddFieldValue(STEP_SCDM, "SZ");
    step.AddFieldValue(STEP_HYDM, "000002");
    step.AddFieldValue(STEP_HYDMMC, "万科A");
    step.AddFieldValue(STEP_HYCS, 100);
    step.AddFieldValue(STEP_WTSX, 15000);
    step.AddFieldValue(STEP_ZCSX, 25000);
    step.AddFieldValue(STEP_ZXBDJW, 0.01);
    step.AddFieldValue(STEP_DWBZJ, 1500.75);
    step.EndAppendRecord();

    std::cout << "添加了 " << step.RecordsCount() << " 条记录" << std::endl;

    // 序列化
    std::string serialized = step.ToString();
    std::cout << "序列化后大小: " << serialized.size() << " 字节" << std::endl;

    // 显示序列化内容（前200个字符）
    std::cout << "序列化内容预览: " << std::endl;
    std::cout << serialized.substr(0, 200) << "..." << std::endl;
}

void SerializationExample()
{
    std::cout << "\n=== 序列化和反序列化示例 ===" << std::endl;

    // 创建原始数据
    GatePBStep originalStep;
    originalStep.Init();

    originalStep.SetBaseFieldValueInt(STEP_FUNC, 2001);
    originalStep.SetBaseFieldValueString(STEP_MSG, "查询成功");

    originalStep.AppendRecord();
    originalStep.AddFieldValue(STEP_SCDM, "SH");
    originalStep.AddFieldValue(STEP_HYDM, "600000");
    originalStep.AddFieldValue(STEP_HYDMMC, "浦发银行");
    originalStep.EndAppendRecord();

    // 序列化
    std::string packageData = originalStep.ToString();
    std::cout << "原始数据序列化完成" << std::endl;

    // 反序列化到新对象
    GatePBStep newStep;
    bool success = newStep.SetPackage(packageData);

    if (success)
    {
        std::cout << "反序列化成功!" << std::endl;
        std::cout << "功能号: " << newStep.GetBaseFieldValue(STEP_FUNC) << std::endl;
        std::cout << "消息: " << newStep.GetBaseFieldValue(STEP_MSG) << std::endl;
        std::cout << "记录数: " << newStep.RecordsCount() << std::endl;

        // 读取记录内容
        newStep.GotoFirst();
        std::cout << "第一条记录:" << std::endl;
        std::cout << "  市场代码: " << newStep.GetStepValueByID(STEP_SCDM) << std::endl;
        std::cout << "  合约代码: " << newStep.GetStepValueByID(STEP_HYDM) << std::endl;
        std::cout << "  合约名称: " << newStep.GetStepValueByID(STEP_HYDMMC) << std::endl;
    }
    else
    {
        std::cout << "反序列化失败!" << std::endl;
    }
}

void NavigationExample()
{
    std::cout << "\n=== 记录导航示例 ===" << std::endl;

    GatePBStep step;
    step.Init();

    // 添加多条记录
    for (int i = 1; i <= 5; ++i)
    {
        step.AppendRecord();
        step.AddFieldValue(STEP_SCDM, (i % 2 == 0) ? "SH" : "SZ");
        step.AddFieldValue(STEP_HYDM, "00000" + std::to_string(i));
        step.AddFieldValue(STEP_HYCS, i * 100);
        step.EndAppendRecord();
    }

    std::cout << "总记录数: " << step.RecordsCount() << std::endl;

    // 遍历所有记录
    step.GotoFirst();
    for (int i = 0; i < step.RecordsCount(); ++i)
    {
        std::cout << "记录 " << (i + 1) << ":" << std::endl;
        std::cout << "  市场: " << step.GetStepValueByID(STEP_SCDM) << std::endl;
        std::cout << "  代码: " << step.GetStepValueByID(STEP_HYDM) << std::endl;
        std::cout << "  乘数: " << step.GetStepValueByID(STEP_HYCS) << std::endl;

        if (i < step.RecordsCount() - 1)
        {
            step.GotoNext();
        }
    }
}

void SpecialCharacterExample()
{
    std::cout << "\n=== 特殊字符处理示例 ===" << std::endl;

    GatePBStep step;
    step.Init();

    // 包含特殊字符的字符串
    std::string specialMsg = "包含特殊字符: = & \n 的消息";
    step.SetBaseFieldValueString(STEP_MSG, specialMsg);

    step.AppendRecord();
    step.AddFieldValue(STEP_HYDMMC, "股票名称=测试&换行\n字符", true); // 启用转义
    step.EndAppendRecord();

    // 序列化和反序列化
    std::string serialized = step.ToString();

    GatePBStep newStep;
    newStep.SetPackage(serialized);

    // 验证特殊字符是否正确处理
    std::string retrievedMsg = newStep.GetBaseFieldValue(STEP_MSG);
    std::cout << "原始消息: " << specialMsg << std::endl;
    std::cout << "恢复消息: " << retrievedMsg << std::endl;
    std::cout << "消息一致: " << (specialMsg == retrievedMsg ? "是" : "否") << std::endl;

    newStep.GotoFirst();
    std::string retrievedName = newStep.GetStepValueByID(STEP_HYDMMC);
    std::cout << "合约名称: " << retrievedName << std::endl;
}

void PerformanceExample()
{
    std::cout << "\n=== 性能测试示例 ===" << std::endl;

    const int recordCount = 10000;

    auto start = std::chrono::high_resolution_clock::now();

    GatePBStep step;
    step.Init();

    step.SetBaseFieldValueInt(STEP_FUNC, 3001);
    step.SetBaseFieldValueInt(STEP_RETURNNUM, recordCount);

    // 批量添加记录
    for (int i = 0; i < recordCount; ++i)
    {
        step.AppendRecord();
        step.AddFieldValue(STEP_SCDM, (i % 2 == 0) ? "SH" : "SZ");
        step.AddFieldValue(STEP_HYDM, std::to_string(i));
        step.AddFieldValue(STEP_HYCS, i * 100);
        step.AddFieldValue(STEP_WTSX, i * 1000);
        step.AddFieldValue(STEP_ZXBDJW, i * 0.01);
        step.EndAppendRecord();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "添加 " << recordCount << " 条记录耗时: " << duration.count() << " 毫秒" << std::endl;
    std::cout << "平均每秒处理: " << (recordCount * 1000.0 / duration.count()) << " 条记录" << std::endl;

    // 序列化性能
    start = std::chrono::high_resolution_clock::now();
    std::string serialized = step.ToString();
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "序列化耗时: " << duration.count() << " 毫秒" << std::endl;
    std::cout << "序列化大小: " << serialized.size() << " 字节" << std::endl;
}

int main()
{
    std::cout << "GatePBStep 使用示例" << std::endl;
    std::cout << "==================" << std::endl;

    try
    {
        BasicUsageExample();
        SerializationExample();
        NavigationExample();
        SpecialCharacterExample();
        PerformanceExample();

        std::cout << "\n所有示例运行完成!" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "示例运行出错: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
