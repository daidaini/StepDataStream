#pragma once

/**
 * @brief 多记录数据
 * 一般用于应答数据
 */

#include "StreamBase.h"

#include <list>
#include <deque>
#include <vector>

namespace step
{
    using RecordsType = std::vector<std::map<int, std::string>>;
    class MultiRecordStream : public StreamBase
    {
    public:
        MultiRecordStream() = default;
        ~MultiRecordStream() = default;

        virtual void Init() override;

        // 序列化
        virtual std::string ToSerialized() override;

        virtual bool DoDeserialize(const std::string &src)
        {
            return true;
        }

        void AppendRecord();
        // 结束当条记录的添加
        void EndAppendRecord();

        // 添加记录
        template <char>
        void AddFiledValue(int stepid, char value)
        {
            m_CurrRecord.emplace(stepid, value == '\0' ? "" : std::string(1, value));
        }

        template <typename T>
        void AddFieldValue(int stepid, T &&value)
        {
            if constexpr (std::is_arithmetic<T>::value) // 数字类型
            {
                m_CurrRecord.emplace(stepid, std::to_string(value));
            }
            else
            {
                m_CurrRecord.emplace(stepid, std::forward<T>(value));
            }
        }

    private:
        RecordsType m_Records;

        std::map<int, std::string> m_CurrRecord;
    };
}