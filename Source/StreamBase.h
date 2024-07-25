#pragma once

/**
 * @brief 数据管理的基类
 * 需要进行包头数据的管理
 */

#include <map>
#include <fmt/format.h>
#include "StringFunc.h"

namespace step
{
    class StreamBase
    {
    public:
        StreamBase() = default;
        ~StreamBase() = default;

        virtual void Init() = 0;
        // 反序列化
        virtual bool DoDeserialize(const std::string &src) = 0;
        // 序列化
        virtual std::string ToSerialized() = 0;

        const std::string &GetBaseFieldValue(int stepid);
        void SetBaseFieldValueString(int stepid, const std::string &value);

    protected:
        // 转义字段
        static std::string EscapeItem(const std::string &src);

        // 字段反义(反序列化时使用)
        static std::string EscapeBackItem(const std::string &src);

        void ParseBaseRecord(const std::string &baseStr);

        std::string BaseRecord() const;

    protected:
        std::map<int, std::string> m_BaseRecords;

        static const std::string s_EmptyItem;
    };
}