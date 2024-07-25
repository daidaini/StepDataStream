#pragma once

/**
 * @brief 内存块管理，内存长度固定
 * 只支持添加数据，查询数据以及删除和重置数据，不支持修改
 */

#include <cstring>
#include <cstdlib>

#include <vector>
#include <map>

class MemBlock
{
public:
    explicit MemBlock(size_t capacity);
    ~MemBlock();

    /**向缓冲区尾部加入一块数据
     * @param pData 加入数据的指针
     * @param nDataLen 加入数据的长度
     * @return 对应内存块存储的索引位置
     */
    int PushBack(const char *pData, size_t nDataLen);

    std::pair<char *, size_t> Get(int index);

    void Delete(int index);

    void Reset();

    size_t LeftSize() const;

private:
    char *m_Mem = nullptr;
    char *m_CurrPtr = nullptr;

    const size_t m_Capacity;
    size_t m_UsedSize{0};

    // 标识不同内存块的长度
    std::vector<size_t> m_Blocks;
};