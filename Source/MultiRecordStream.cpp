#include "MultiRecordStream.h"
#include "StringFunc.h"

using namespace step;

void MultiRecordStream::Init()
{
    m_Records.clear();
}

std::string MultiRecordStream::ToSerialized()
{
    std::string result = BaseRecord();

    for (auto &record : m_Records)
    {
        for (auto &item : record)
        {
            result.append(fmt::format("{}={}\n", item.first, item.second));
        }

        result.push_back('\n');
    }

    return result;
}

void MultiRecordStream::AppendRecord()
{
    if (!m_CurrRecord.empty())
    {
        m_Records.emplace_back(std::move(m_CurrRecord));
        m_CurrRecord.clear();
    }

    return;
}

void MultiRecordStream::EndAppendRecord()
{
    m_Records.emplace_back(std::move(m_CurrRecord));
}

/*
void MultiRecordStream::AddFieldValue(int stepid, std::string &&value, bool isEscape)
{
    if (isEscape)
    {
        m_CurrRecord.emplace(stepid, EscapeItem(std::forward<std::string>(value)));
    }
    else
    {
        m_CurrRecord.emplace(stepid, std::forward<std::string>(value));
    }
}
*/