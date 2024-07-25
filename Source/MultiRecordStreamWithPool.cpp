#include "MultiRecordStreamWithPool.h"

using namespace step;

MultiRecordStreamWithPool::MultiRecordStreamWithPool()
    : m_BufferPool(s_MaxItemSize)
{
}

MultiRecordStreamWithPool::~MultiRecordStreamWithPool()
{
}

void MultiRecordStreamWithPool::Init()
{
    for (auto &item : m_BodyRecords)
    {
        if (item.first == MemTypeEnum::FromPool)
        {
            m_BufferPool.free(item.second);
        }
        else
        {
            free(item.second);
        }
    }

    m_BodyRecords.clear();
}

bool MultiRecordStreamWithPool::DoDeserialize(const std::string &src)
{
    return true;
}

std::string MultiRecordStreamWithPool::ToSerialized()
{
    std::string result;

    result = BaseRecord();

    for (auto &item : m_BaseRecords)
    {
        result.append(item.second);

        result.push_back('\n');
    }

    return result;
}

void MultiRecordStreamWithPool::AppendRecord()
{
}

void MultiRecordStreamWithPool::EndAppendRecord()
{
    char *cachePtr = nullptr;
    MemTypeEnum memType = MemTypeEnum::FromPool;

    size_t recLen = m_TmpBuffer.size();

    if (recLen > s_MaxItemSize)
    {
        memType = MemTypeEnum::FromSys;
        cachePtr = (char *)malloc(recLen);
    }
    else
    {
        cachePtr = (char *)m_BufferPool.malloc();
    }

    ::memcpy(cachePtr, m_TmpBuffer.c_str(), m_TmpBuffer.size());
    m_TmpBuffer.clear();

    m_BodyRecords.emplace_back(std::make_pair(memType, cachePtr));
}

void MultiRecordStreamWithPool::AddFieldValue(int stepid, const std::string &value, bool isEscape)
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

void MultiRecordStreamWithPool::AddFieldValue(int stepid, int value)
{
    m_TmpBuffer += fmt::format("{}={}&", stepid, value);
}

void MultiRecordStreamWithPool::AddFieldValue(int stepid, double value)
{
    m_TmpBuffer += fmt::format("{}={}&", stepid, value);
}