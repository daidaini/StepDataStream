#include "MemoryPool.h"
#include <algorithm>

namespace stepver2
{
    // MemoryBlock 实现
    MemoryBlock::MemoryBlock(size_t size)
        : data_(std::make_unique<char[]>(size)), total_size_(size), used_size_(0)
    {
        if (size == 0)
        {
            throw std::invalid_argument("MemoryBlock size cannot be zero");
        }
    }

    MemoryBlock::MemoryBlock(MemoryBlock &&other) noexcept
        : data_(std::move(other.data_)), total_size_(other.total_size_), used_size_(other.used_size_)
    {
        other.total_size_ = 0;
        other.used_size_ = 0;
    }

    MemoryBlock &MemoryBlock::operator=(MemoryBlock &&other) noexcept
    {
        if (this != &other)
        {
            data_ = std::move(other.data_);
            total_size_ = other.total_size_;
            used_size_ = other.used_size_;
            
            other.total_size_ = 0;
            other.used_size_ = 0;
        }
        return *this;
    }

    char *MemoryBlock::Allocate(size_t size)
    {
        if (size == 0)
        {
            return nullptr;
        }

        if (!CanAllocate(size))
        {
            return nullptr;
        }

        char *result = data_.get() + used_size_;
        used_size_ += size;
        return result;
    }

    void MemoryBlock::Reset()
    {
        used_size_ = 0;
    }

    // MemoryPool 实现
    MemoryPool::MemoryPool(size_t block_size)
        : block_size_(block_size), current_block_index_(0)
    {
        if (block_size == 0)
        {
            throw std::invalid_argument("MemoryPool block size cannot be zero");
        }
        
        // 预分配一个内存块
        AllocateNewBlock();
    }

    MemoryPool::MemoryPool(MemoryPool &&other) noexcept
        : block_size_(other.block_size_),
          blocks_(std::move(other.blocks_)),
          current_block_index_(other.current_block_index_)
    {
        other.block_size_ = 0;
        other.current_block_index_ = 0;
    }

    MemoryPool &MemoryPool::operator=(MemoryPool &&other) noexcept
    {
        if (this != &other)
        {
            block_size_ = other.block_size_;
            blocks_ = std::move(other.blocks_);
            current_block_index_ = other.current_block_index_;
            
            other.block_size_ = 0;
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

        if (size > block_size_)
        {
            throw std::invalid_argument("Data size exceeds block size");
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
        return blocks_.size() * block_size_;
    }

    void MemoryPool::AllocateNewBlock()
    {
        try
        {
            auto new_block = std::make_unique<MemoryBlock>(block_size_);
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
