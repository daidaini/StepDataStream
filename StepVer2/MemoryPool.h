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
     * @brief 内存块类，支持4档动态扩充的内存管理
     * 第1档: 1024 bytes, 第2档: 4096 bytes, 第3档: 16384 bytes, 第4档: 65536 bytes
     */
    class MemoryBlock
    {
    public:
        /**
         * @brief 构造函数，指定初始等级(1-4)
         * @param level 初始容量等级，如果超出范围则限制在有效范围内
         */
        explicit MemoryBlock(int level = 1);
        ~MemoryBlock() = default;

        // 禁用拷贝构造和赋值
        MemoryBlock(const MemoryBlock &) = delete;
        MemoryBlock &operator=(const MemoryBlock &) = delete;

        // 支持移动构造和赋值
        MemoryBlock(MemoryBlock &&other) noexcept;
        MemoryBlock &operator=(MemoryBlock &&other) noexcept;

        /**
         * @brief 分配指定大小的内存，支持动态扩充
         * @param size 需要分配的内存大小
         * @return 分配的内存指针，失败返回nullptr
         */
        char *Allocate(size_t size);

        /**
         * @brief 重置内存块，清空所有数据但保持当前容量等级
         */
        void Reset();

        /**
         * @brief 获取已使用的内存大小
         */
        size_t GetUsedSize() const { return used_size_; }

        /**
         * @brief 获取当前总内存大小
         */
        size_t GetTotalSize() const { return data_.size(); }

        /**
         * @brief 获取剩余可用内存大小
         */
        size_t GetAvailableSize() const { return GetTotalSize() - used_size_; }

        /**
         * @brief 检查是否有足够的空间分配指定大小的内存
         */
        bool CanAllocate(size_t size) const { return GetAvailableSize() >= size; }

        /**
         * @brief 获取当前容量等级(1-4)
         */
        int GetLevel() const { return current_level_; }

        /**
         * @brief 获取指定等级的容量大小
         * @param level 等级(1-4)，超出范围返回0
         */
        static size_t GetLevelCapacity(int level);

    private:
        /**
         * @brief 动态扩充到指定等级
         * @param target_level 目标等级
         * @return 扩充成功返回true，失败返回false
         */
        bool ExpandToLevel(int target_level);

        /**
         * @brief 根据所需大小确定目标等级
         * @param required_size 需要的总大小
         * @return 目标等级(1-4)
         */
        int DetermineTargetLevel(size_t required_size) const;

    private:
        std::vector<char> data_;      // 内存数据
        int current_level_;           // 当前容量等级
        size_t used_size_;           // 已使用的内存大小

        // 4档容量大小
        static const size_t LEVEL_CAPACITIES[4];
    };

    /**
     * @brief 内存池类，管理多个支持动态扩充的内存块
     */
    class MemoryPool
    {
    public:
        /**
         * @brief 构造函数，自动选择最优的内存块初始等级
         */
        MemoryPool();
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
         * @param hint_size 提示的内存大小，用于智能选择初始等级
         */
        void AllocateNewBlock(size_t hint_size = 0);

        /**
         * @brief 查找可用的内存块
         * @param size 需要的内存大小
         * @return 可用的内存块指针，没有则返回nullptr
         */
        MemoryBlock *FindAvailableBlock(size_t size);

        /**
         * @brief 根据历史使用情况智能选择内存块的初始等级
         * @param hint_size 提示的内存大小
         * @return 推荐的初始等级(1-4)
         */
        int DetermineOptimalInitialLevel(size_t hint_size) const;

    private:
        std::vector<std::unique_ptr<MemoryBlock>> blocks_; // 内存块列表
        size_t current_block_index_;                    // 当前使用的内存块索引
        size_t total_allocated_count_;                  // 总分配次数
        size_t total_allocated_size_;                   // 总分配大小
    };
}
