/*
 * @Description: 新的内存池实现，替代ReuseCacheList
 * @Author: yubo
 * @Date: 2025-01-24
 */
#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace stepver2
{
    /**
     * @brief 内存块类，管理单个内存块
     */
    class MemoryBlock
    {
    public:
        explicit MemoryBlock(size_t size);
        ~MemoryBlock() = default;

        // 禁用拷贝构造和赋值
        MemoryBlock(const MemoryBlock &) = delete;
        MemoryBlock &operator=(const MemoryBlock &) = delete;

        // 支持移动构造和赋值
        MemoryBlock(MemoryBlock &&other) noexcept;
        MemoryBlock &operator=(MemoryBlock &&other) noexcept;

        /**
         * @brief 分配指定大小的内存
         * @param size 需要分配的内存大小
         * @return 分配的内存指针，失败返回nullptr
         */
        char *Allocate(size_t size);

        /**
         * @brief 重置内存块，清空所有数据
         */
        void Reset();

        /**
         * @brief 获取已使用的内存大小
         */
        size_t GetUsedSize() const { return used_size_; }

        /**
         * @brief 获取总内存大小
         */
        size_t GetTotalSize() const { return total_size_; }

        /**
         * @brief 获取剩余可用内存大小
         */
        size_t GetAvailableSize() const { return total_size_ - used_size_; }

        /**
         * @brief 检查是否有足够的空间分配指定大小的内存
         */
        bool CanAllocate(size_t size) const { return GetAvailableSize() >= size; }

    private:
        std::unique_ptr<char[]> data_;  // 内存数据
        size_t total_size_;             // 总内存大小
        size_t used_size_;              // 已使用的内存大小
    };

    /**
     * @brief 内存池类，管理多个内存块
     */
    class MemoryPool
    {
    public:
        explicit MemoryPool(size_t block_size = 16 * 1024);
        ~MemoryPool() = default;

        // 禁用拷贝构造和赋值
        MemoryPool(const MemoryPool &) = delete;
        MemoryPool &operator=(const MemoryPool &) = delete;

        // 支持移动构造和赋值
        MemoryPool(MemoryPool &&other) noexcept;
        MemoryPool &operator=(MemoryPool &&other) noexcept;

        /**
         * @brief 分配指定大小的内存
         * @param data 要存储的数据指针
         * @param size 数据大小
         * @return 分配的内存指针，失败抛出异常
         */
        char *Allocate(const char *data, size_t size);

        /**
         * @brief 重置内存池，清空所有数据
         */
        void Reset();

        /**
         * @brief 获取总的已使用内存大小
         */
        size_t GetTotalUsedSize() const;

        /**
         * @brief 获取总的内存大小
         */
        size_t GetTotalSize() const;

        /**
         * @brief 获取内存块数量
         */
        size_t GetBlockCount() const { return blocks_.size(); }

    private:
        /**
         * @brief 分配新的内存块
         */
        void AllocateNewBlock();

        /**
         * @brief 查找可用的内存块
         * @param size 需要的内存大小
         * @return 可用的内存块指针，没有则返回nullptr
         */
        MemoryBlock *FindAvailableBlock(size_t size);

    private:
        size_t block_size_;                             // 每个内存块的大小
        std::vector<std::unique_ptr<MemoryBlock>> blocks_; // 内存块列表
        size_t current_block_index_;                    // 当前使用的内存块索引
    };
}
