#include "MemBlockList.h"

MemBlockList::MemBlockList(size_t blockCapacity)
    : m_BlockCapacity(blockCapacity)
{
}

void MemBlockList::PushData(const char *data, size_t datalen)
{
    if (m_BlockWithIndexs.empty())
    {
        if (m_BlockPool.empty())
        {
            AllocateMem();
        }

        auto &memblock = m_BlockPool.front();
        if (datalen <= memblock->LeftSize())
        {
            m_BlockWithIndexs.emplace_back(std::make_pair(0, memblock->PushBack(data, datalen)));
            return;
        }
    }

    auto memIndex = m_BlockWithIndexs.back().first;
    auto &memblock = m_BlockPool.at(memIndex);
    if (memblock->LeftSize() >= datalen)
    {
        m_BlockWithIndexs.emplace_back(std::make_pair(memIndex, memblock->PushBack(data, datalen)));
    }
    else
    {
        // 获取下一段内存
        if (m_BlockPool.size() - 1 < memIndex + 1)
        {
            AllocateMem();
        }
        auto &newMemblock = m_BlockPool.at(memIndex + 1);
        m_BlockWithIndexs.emplace_back(std::make_pair(memIndex + 1, newMemblock->PushBack(data, datalen)));
    }
}

void MemBlockList::AllocateMem()
{
    m_BlockPool.emplace_back(std::make_unique<MemBlock>(m_BlockCapacity));
}

void MemBlockList::Reset()
{
    m_BlockWithIndexs.clear();

    for (auto &memblock : m_BlockPool)
    {
        memblock->Reset();
    }
}

std::vector<std::string> MemBlockList::Traverse()
{
    std::vector<std::string> results;

    for (const auto &item : m_BlockWithIndexs)
    {
        auto dst = m_BlockPool[item.first]->Get(item.second);

        results.emplace_back(std::string((const char *)dst.first, dst.second));
    }

    return results;
}