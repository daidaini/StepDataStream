#include "MemBlock.h"

#include <stdexcept>

MemBlock::MemBlock(size_t capacity)
    : m_Capacity(capacity)
{
    m_Mem = (char *)malloc(capacity);
    if (m_Mem == nullptr)
    {
        throw std::runtime_error("malloc failed.. memory leak");
    }
    ::memset(m_Mem, 0, capacity);

    m_CurrPtr = m_Mem;
}

MemBlock::~MemBlock()
{
    if (m_Mem != nullptr)
    {
        free((void *)m_Mem);
        m_Mem = nullptr;
    }
}

int MemBlock::PushBack(const char *pData, size_t nDataLen)
{
    if (nDataLen + m_UsedSize > m_Capacity)
    {
        return -1;
    }

    // 拷贝
    ::memcpy(m_CurrPtr, pData, nDataLen);
    // 移位
    m_CurrPtr = m_CurrPtr + nDataLen;
    m_Blocks.emplace_back(m_UsedSize);
    m_UsedSize += nDataLen;

    return m_Blocks.size() - 1;
}

std::pair<char *, size_t> MemBlock::Get(int index)
{
    size_t usedLen = m_Blocks.at(index);
    char *ptr = m_Mem + usedLen;

    if (*ptr == '\0')
    {
        return std::make_pair(nullptr, 0);
    }

    size_t len = 0;
    if (m_Blocks.size() > index + 1)
    {
        len = m_Blocks.at(index + 1) - usedLen;
    }
    else
    {
        len = m_Capacity - usedLen;
    }

    return std::make_pair(ptr, len);
}

void MemBlock::Reset()
{
    ::memset(m_Mem, 0, m_Capacity);
    m_CurrPtr = m_Mem;
    m_UsedSize = 0;
    m_Blocks.clear();
}

void MemBlock::Delete(int index)
{
    size_t usedLen = m_Blocks.at(index);
    char *ptr = m_Mem + usedLen;

    size_t len = 0;
    if (m_Blocks.size() > index + 1)
    {
        len = m_Blocks.at(index + 1) - usedLen;
    }
    else
    {
        len = m_Capacity - usedLen;
    }

    ::memset(ptr, 0, len);
}

size_t MemBlock::LeftSize() const
{
    return m_Capacity - m_UsedSize;
}