#include "MemoryPool.h"
#include <algorithm>

namespace stepver2
{
          // 初始化静态成员变量
    const size_t MemoryBlock::LEVEL_CAPACITIES[4] = {1024, 4096, 16384, 65536};

    // MemoryBlock 实现
    MemoryBlock::MemoryBlock(int level)
        : used_size_(0)
    {
        // 确保level在有效范围内
        current_level_ = std::max(1, std::min(4, level));

        // 分配初始内存
        size_t capacity = LEVEL_CAPACITIES[current_level_ - 1];
        data_.resize(capacity);
    }

    MemoryBlock::MemoryBlock(MemoryBlock &&other) noexcept
        : data_(std::move(other.data_)),
          current_level_(other.current_level_),
          used_size_(other.used_size_)
    {
        other.current_level_ = 1;
        other.used_size_ = 0;
    }

    MemoryBlock &MemoryBlock::operator=(MemoryBlock &&other) noexcept
    {
        if (this != &other)
        {
            data_ = std::move(other.data_);
            current_level_ = other.current_level_;
            used_size_ = other.used_size_;

            other.current_level_ = 1;
            other.used_size_ = 0;
        }
        return *this;
    }

    size_t MemoryBlock::GetLevelCapacity(int level)
    {
        if (level < 1 || level > 4)
        {
            return 0;
        }
        return LEVEL_CAPACITIES[level - 1];
    }

    int MemoryBlock::DetermineTargetLevel(size_t required_size) const
    {
        for (int i = 0; i < 4; ++i)
        {
            if (required_size <= LEVEL_CAPACITIES[i])
            {
                return i + 1;
            }
        }
        return 4; // 超出最大等级，返回最大等级
    }

    bool MemoryBlock::ExpandToLevel(int target_level)
    {
        if (target_level <= current_level_ || target_level > 4)
        {
            return false; // 无需扩展或目标等级无效
        }

        size_t new_capacity = LEVEL_CAPACITIES[target_level - 1];

        try
        {
            data_.resize(new_capacity);
            current_level_ = target_level;
            return true;
        }
        catch (const std::bad_alloc&)
        {
            return false; // 内存分配失败
        }
    }

    char *MemoryBlock::Allocate(size_t size)
    {
        if (size == 0)
        {
            return nullptr;
        }

        size_t required_size = used_size_ + size;

        // 检查是否需要扩展
        if (required_size > data_.size())
        {
            int target_level = DetermineTargetLevel(required_size);
            if (!ExpandToLevel(target_level))
            {
                return nullptr; // 扩展失败
            }
            // 扩展后重新检查空间
            required_size = used_size_ + size;
        }

        // 确保有足够空间
        if (required_size > data_.size())
        {
            return nullptr; // 即使扩展后仍然不够
        }

        char *result = data_.data() + used_size_;
        used_size_ += size;
        return result;
    }

    void MemoryBlock::Reset()
    {
        used_size_ = 0;
        // 不清零vector数据以提高性能，只重置使用量
    }

    // MemoryPool 实现
    MemoryPool::MemoryPool(int initial_level)
        : initial_level_(initial_level), current_block_index_(0)
    {
        if (initial_level < 1 || initial_level > 4)
        {
            throw std::invalid_argument("MemoryPool initial level must be between 1 and 4");
        }

        // 预分配一个内存块
        AllocateNewBlock();
    }

    MemoryPool::MemoryPool(MemoryPool &&other) noexcept
        : initial_level_(other.initial_level_),
          blocks_(std::move(other.blocks_)),
          current_block_index_(other.current_block_index_)
    {
        other.initial_level_ = 1;
        other.current_block_index_ = 0;
    }

    MemoryPool &MemoryPool::operator=(MemoryPool &&other) noexcept
    {
        if (this != &other)
        {
            initial_level_ = other.initial_level_;
            blocks_ = std::move(other.blocks_);
            current_block_index_ = other.current_block_index_;

            other.initial_level_ = 1;
            other.current_block_index_ = 0;
        }
        return *this;
    }

    char *MemoryPool::Allocate(const char *data, size_t size)
    {
        if (!data || size == 0)
        {
            throw std::invalid_argument("Invalid data or size");
        }

        // 检查大小是否超过最大等级容量
        if (size > MemoryBlock::GetLevelCapacity(4))
        {
            throw std::invalid_argument("Data size exceeds maximum capacity");
        }

        // 查找可用的内存块
        MemoryBlock *available_block = FindAvailableBlock(size);

        if (!available_block)
        {
            // 没有可用的内存块，分配新的
            AllocateNewBlock();
            available_block = blocks_.back().get();
        }

        char *result = available_block->Allocate(size);
        if (!result)
        {
            throw std::runtime_error("Failed to allocate memory from block");
        }

        // 复制数据
        std::memcpy(result, data, size);
        return result;
    }

    void MemoryPool::Reset()
    {
        for (auto &block : blocks_)
        {
            block->Reset();
        }
        current_block_index_ = 0;
    }

    size_t MemoryPool::GetTotalUsedSize() const
    {
        size_t total = 0;
        for (const auto &block : blocks_)
        {
            total += block->GetUsedSize();
        }
        return total;
    }

    size_t MemoryPool::GetTotalSize() const
    {
        size_t total = 0;
        for (const auto &block : blocks_)
        {
            total += block->GetTotalSize();
        }
        return total;
    }

    void MemoryPool::AllocateNewBlock()
    {
        try
        {
            auto new_block = std::make_unique<MemoryBlock>(initial_level_);
            blocks_.push_back(std::move(new_block));
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Failed to allocate new memory block: " + std::string(e.what()));
        }
    }

    MemoryBlock *MemoryPool::FindAvailableBlock(size_t size)
    {
        // 从当前块开始查找
        for (size_t i = current_block_index_; i < blocks_.size(); ++i)
        {
            if (blocks_[i]->CanAllocate(size))
            {
                current_block_index_ = i;
                return blocks_[i].get();
            }
        }

        // 如果当前块之后没有找到，从头开始查找
        for (size_t i = 0; i < current_block_index_; ++i)
        {
            if (blocks_[i]->CanAllocate(size))
            {
                current_block_index_ = i;
                return blocks_[i].get();
            }
        }

        return nullptr;
    }
}
