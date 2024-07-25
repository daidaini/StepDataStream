#pragma once

/**
 * @brief 用于多记录数据，且使用自定义内memblocklist
 * 一般使用在需要复用内存的情况
 */

#include "StreamBase.h"
#include "MemBlockList.h"
#include <map>

namespace step
{
    class MultiRecordStreamWithMem : public StreamBase
    {
    public:
        MultiRecordStreamWithMem() = default;
        ~MultiRecordStreamWithMem() = default;

        virtual void Init() override;
        // 反序列化
        virtual bool DoDeserialize(const std::string &src) override;
        // 序列化
        virtual std::string ToSerialized() override;

        void AppendRecord();
        // 结束当条记录的添加
        void EndAppendRecord();

        // 添加记录
        void AddFieldValue(int stepid, const std::string &value, bool isEscape = false);

        void AddFieldValue(int stepid, int value);
        void AddFieldValue(int stepid, double value);

    private:
        std::string m_TmpBuffer;

        MemBlockList m_MemBlockList;
    };
}
