#include "SingleRecordStream.h"

using namespace step;

void SingleRecordStream::Init()
{
    m_Items.clear();
}

std::string SingleRecordStream::ToSerialized()
{
    std::string result = BaseRecord();

    for (auto &item : m_Items)
    {
        result.append(fmt::format("{}={}\n", item.first, item.second));

        result.push_back('\n');
    }

    return result;
}

bool SingleRecordStream::DoDeserialize(const std::string &src)
{
    auto records = str::Split(src, '\n');
    if (records.empty())
    {
        return false;
    }

    Init();
    ParseBaseRecord(records.front());

    if (records.size() == 1)
    {
        return true;
    }

    auto &bodyRec = records.at(1);

    const auto items = str::Split(bodyRec, '&');
    for (const auto &item : items)
    {
        auto idVal = str::Split(item, '=');
        int stepid = atoi(idVal.at(0).c_str());
        if (idVal.size() < 2)
        {
            m_Items.emplace(stepid, "");
        }
        else
        {
            m_Items.emplace(stepid, EscapeBackItem(idVal.at(1)));
        }
    }

    return true;
}

const std::string &SingleRecordStream::GetFieldValue(int stepid)
{
    auto it = m_Items.find(stepid);
    if (it != m_Items.end())
    {
        return it->second;
    }

    return s_EmptyItem;
}