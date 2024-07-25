#include "MultiRecordStreamWithMem.h"

using namespace step;

void MultiRecordStreamWithMem::Init()
{
    m_MemBlockList.Reset();
}

bool MultiRecordStreamWithMem::DoDeserialize(const std::string &src)
{
    return true;
}

std::string MultiRecordStreamWithMem::ToSerialized()
{
    std::string result;

    result = BaseRecord();

    auto datas = m_MemBlockList.Traverse();
    for (auto &item : datas)
    {
        result.append(std::move(item));

        result.push_back('\n');
    }

    return result;
}

void MultiRecordStreamWithMem::AppendRecord()
{
}

void MultiRecordStreamWithMem::EndAppendRecord()
{
    m_MemBlockList.PushData(m_TmpBuffer.c_str(), m_TmpBuffer.size());

    m_TmpBuffer.clear();
}

void MultiRecordStreamWithMem::AddFieldValue(int stepid, const std::string &value, bool isEscape)
{
    if (isEscape)
    {
        m_TmpBuffer.append(fmt::format("{}={}&", stepid, EscapeItem(value)));
    }
    else
    {
        m_TmpBuffer.append(fmt::format("{}={}&", stepid, value));
    }
}

void MultiRecordStreamWithMem::AddFieldValue(int stepid, int value)
{
    m_TmpBuffer += fmt::format("{}={}&", stepid, value);
}

void MultiRecordStreamWithMem::AddFieldValue(int stepid, double value)
{
    m_TmpBuffer += fmt::format("{}={}&", stepid, value);
}