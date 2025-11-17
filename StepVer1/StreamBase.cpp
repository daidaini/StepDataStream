#include "StreamBase.h"
#include "StringFunc.h"

using namespace step;

namespace
{
    // 反转义字符对应
    static const std::map<char, char> s_EscapeBackItemMap{
        {'\\', '\\'}, {'a', '='}, {'b', '&'}, {'n', '\n'}};

    // 转义字符对应
    static const std::map<char, char> s_EscapeItemMap{
        {'\\', '\\'}, {'=', 'a'}, {'&', 'b'}, {'\n', 'n'}};
}

const std::string StreamBase::s_EmptyItem = "";

std::string StreamBase::EscapeItem(const std::string &src)
{
    std::string result;
    result.reserve(src.length() + 8);
    for (char elem : src)
    {
        auto it = s_EscapeItemMap.find(elem);
        if (it == s_EscapeItemMap.end())
        {
            result.push_back(elem);
        }
        else
        {
            result.push_back('\\');
            result.push_back(it->second);
        }
    }
    return result;
}

std::string StreamBase::EscapeBackItem(const std::string &src)
{
    if (src.empty() || src.size() < 2)
    {
        return src;
    }

    std::string result;
    result.reserve(src.length());

    for (size_t i = 0; i < src.size(); ++i)
    {
        if (src[i] == '\\' && i != src.size() - 1)
        {
            char elem = src[i + 1];
            auto it = s_EscapeBackItemMap.find(elem);
            if (it != s_EscapeBackItemMap.end())
            { // 需转义
                result.push_back(it->second);
                ++i;
            }
            else
            {
                result.push_back(src[i]);
            }
        }
        else
        {
            result.push_back(src[i]);
        }
    }

    return result;
}

const std::string &StreamBase::GetBaseFieldValue(int stepid)
{
    auto it = m_BaseRecords.find(stepid);
    if (it != m_BaseRecords.end())
        return it->second;

    return s_EmptyItem;
}

void StreamBase::SetBaseFieldValueString(int stepid, const std::string &value)
{
    m_BaseRecords[stepid] = EscapeItem(value);
}

void StreamBase::ParseBaseRecord(const std::string &baseStr)
{
    auto items = str::Split(baseStr, '&');
    for (const auto &item : items)
    {
        size_t index = item.find_first_of('=');
        if (index == item.npos)
        {
            return;
        }

        int stepid = atoi(item.substr(0, index).c_str());

        std::string value;
        if (index != item.size() - 1)
        {
            value = item.substr(index + 1, item.size() - index - 1);
        }

        m_BaseRecords.emplace(stepid, EscapeBackItem(value));
    }
}

std::string StreamBase::BaseRecord() const
{
    std::string result;
    for (const auto &item : m_BaseRecords)
    {
        result.append(std::to_string(item.first));
        result.push_back('=');
        result.append(item.second.empty() ? "" : item.second);
        result.push_back('&');
    }
    result.push_back('\n');
    return result;
}