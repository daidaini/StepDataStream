#pragma once

/**
 * @brief 管理内存块
 *
 */

#include "MemBlock.h"
#include <memory>
#include <vector>
#include <string>

class MemBlockList
{
public:
    explicit MemBlockList(size_t blockCapacity = 4096); // 默认4k

    ~MemBlockList() = default;

    // 不支持拷贝和赋值
    MemBlockList(MemBlockList &) = delete;
    MemBlockList &operator=(MemBlockList &) = delete;

    void PushData(const char *data, size_t datalen);

    // void Deallocate(char *ptr);

    void Reset();

    std::vector<std::string> Traverse();

private:
    const size_t m_BlockCapacity;

    std::vector<std::unique_ptr<MemBlock>> m_BlockPool; // 不能重置

    /// @brief 每条记录存储m_BlockPool的序号以及对应的MemBlock的序号
    using BlockItemType = std::pair<size_t, size_t>;
    std::vector<BlockItemType> m_BlockWithIndexs;

private:
    void AllocateMem();
};