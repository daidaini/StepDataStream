#pragma once

/**
 * @brief 用于多记录数据，且使用内存池
 * 一般使用在需要复用内存的情况
 */

#include "StreamBase.h"
#include <boost/pool/pool.hpp>
#include <map>

namespace step
{
    constexpr int s_MaxItemSize = 8 * 1024;
    class MultiRecordStreamWithPool : public StreamBase
    {
    public:
        MultiRecordStreamWithPool();
        ~MultiRecordStreamWithPool();

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
        boost::pool<> m_BufferPool;

        std::string m_TmpBuffer;

        enum class MemTypeEnum
        {
            FromPool,
            FromSys,
        };
        std::vector<std::pair<MemTypeEnum, char *>> m_BodyRecords;
    };
}
