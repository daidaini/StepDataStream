#pragma once

#include "CachedPBStep.h"
#include "stepdef.h"


class GatePBStep : public CachedPBStep
{
public:
    GatePBStep(int blockSize = CacheBlockSize);
    virtual ~GatePBStep() = default;

    // 只序列化头（0记录）
    bool SetPackageHead(const std::string &src);

    // 设置Base记录（不带session和requestno）
    void SetBaseRecord(int funcno, int errcode, const char *errmsg); // 错误单记录
    void SetBaseRecord(int funcno, int recordCnt = 1);               // 成功的记录
    // 获取记录
    int GetBaseFieldValueInt(int stepid);
    // 需要转义回来
    void GetFieldValueString(int stepid, char *out, int outlen);
    int GetFieldValueInt(int stepid);
    double GetFieldValueDouble(int stepid);
    char GetFieldValueChar(int stepid);

    int FunctionId();
    int Requestno();

    void AddFieldValueDouble(int stepid, double value, int dot);

    // 金额默认保留2位小数
    void AddMoneyValue(int stepid, double value);
    void AddMoneyValue(int stepid, const std::string &value);
    // 价格默认保留4位小数
    void AddPriceValue(int stepid, double value);
};
