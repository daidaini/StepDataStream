#pragma once

/**
 * @brief 单记录的数据
 * 一般用于声明请求包，如果确定当前应答是单记录，也可使用
 */

#include "StreamBase.h"

namespace step
{
    class SingleRecordStream : public StreamBase
    {
    public:
        SingleRecordStream() = default;
        ~SingleRecordStream() = default;

        virtual void Init() override;

        virtual std::string ToSerialized() override;

        virtual bool DoDeserialize(const std::string &src) override;

        template <char>
        void AddFiledValue(int stepid, char value)
        {
            m_Items.emplace(stepid, value == '\0' ? "" : std::string(1, value));
        }

        template <typename T>
        void AddFieldValue(int stepid, T &&value)
        {
            if constexpr (std::is_arithmetic<T>::value) // 数字类型
            {
                m_Items.emplace(stepid, std::to_string(value));
            }
            else
            {
                m_Items.emplace(stepid, std::forward<T>(value));
            }
        }

        const std::string &GetFieldValue(int stepid);

    private:
        // 存储的是已经转义回来的数据值
        std::map<int, std::string> m_Items;
    };
}