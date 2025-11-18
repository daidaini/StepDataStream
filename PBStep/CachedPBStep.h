/*
 * @Descripttion: 设计一个类，有原先PBStep的功能，
 * 但是内存管理是使用内存池处理（即 内存可以复用，而不是不用了直接释放）
 * 注：该类的设计，是线程不安全的
 * @Author: yubo
 * @Date: 2022-09-01 17:26:07
 * @LastEditTime: 2023-01-04 16:42:16
 */
#pragma once

#include "ReuseCacheList.h"

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <assert.h>
#include <stdexcept>

namespace std
{
    inline std::string to_string(std::string &&src)
    {
        return std::forward<std::string>(src);
    }

    inline std::string to_string(const std::string &src)
    {
        return src;
    }

    inline std::string to_string(const char *src)
    {
        if (src == nullptr)
            return "";

        return src;
    }

    inline std::string to_string(char src)
    {
        if (src == '\0')
            return "";

        return std::string(1, src);
    }
}

class CachedPBStep
{
public:
    // 三档内存块大小定义
    static constexpr int CacheBlockSizeTier1 = 4 * 1024;   // 第一档：4K
    static constexpr int CacheBlockSizeTier2 = 16 * 1024;  // 第二档：16K
    static constexpr int CacheBlockSizeTier3 = 64 * 1024;  // 第三档：64K

    explicit CachedPBStep(int blockSize = CacheBlockSizeTier1);
    virtual ~CachedPBStep();

    void Init();

    /*对于字符的转义，需要注意：
     * 1. 存入内存中的数据必须是转义过的数据，否则内存中的数据无法被正确查找
     * 2. 从内存中查找Get到的数据，需要做反转义的操作，才是准确的数据
     * 3. 将数据Add到内存的时候，要转义
     */
    // 序列化
    bool SetPackage(const std::string &src);
    // 反序列化
    std::string ToString();

    // 获取从第start条到第end条[不包含end]记录
    std::string FormatedRecords(int start, int end);

    std::string BaseRecord() const;

    void AppendRecord();
    // 结束当条记录的添加
    void EndAppendRecord();

    // 添加记录
    template <class DataType>
    void AddFieldValue(int stepid, DataType &&value, bool isEscape = false)
    {
        tmpBuffer_.append(std::to_string(stepid));
        tmpBuffer_.push_back('=');

        if (isEscape)
        {
            tmpBuffer_.append(
                EscapeItem(std::to_string(std::forward<DataType>(value))));
        }
        else
        {
            tmpBuffer_.append(
                std::to_string(std::forward<DataType>(value)));
        }

        tmpBuffer_.push_back('&');
    }

    bool SetFieldValue(int stepid, const char *value) __attribute__((__warn_unused_result__));
    bool SetFieldValueInt(int stepid, int value) __attribute__((__warn_unused_result__));
    void SetBaseFieldValueInt(int stepid, int value);
    void SetBaseFieldValueString(int stepid, const std::string &value);

    // 获取记录
    std::string GetStepValueByID(int stepid);
    std::string GetBaseFieldValue(int stepid);

    void GotoFirst();
    void GotoNext();

    int RecordsCount() const
    {
        return static_cast<int>(bodyRecords_.size());
    }

protected:
    void ParseBaseRecord(const std::string &baseStr);

    std::pair<const char *, int> FindItem(int stepid);
    std::pair<const char *, int> FindItemByBuffer(int stepid);

    // 字段反义
    static std::string EscapeBackItem(const std::string &src);
    // 转义字段
    static std::string EscapeItem(const std::string &src);

    // 不带转义
    std::string GetItem(int stepid);

    // 检查并扩容缓存
    void CheckAndExpandCache(size_t requiredSize);

    // 获取下一档的block size
    int GetNextTierBlockSize(int currentSize) const;

    // 获取缓存池的辅助函数
    pobo::ReuseCacheList& getCachePool() { return *cachePoolPtr_; }

protected:
    int blockSize_;
    //<id, val>
    std::map<int, std::string> baseRecord_;

    // pair<ptr, len>
    // body 存储的内容格式还是 id=value&id=value
    std::vector<std::pair<char *, int>> bodyRecords_;

    pobo::ReuseCacheList* cachePoolPtr_;

    // 包体的当前记录索引, 没有记录时必须为-1
    int currentRecIndex_ = -1;

    /* 用于数据缓存
     * 该字段的作用：
     * 1) 通过SetPackage反序列化后，tmpBuffer_ 存储的是包体的最后一条记录，对于请求来说，就是包体数据。这种情况，适用于通过该字段来查询对应stepid的数据值
     * 2) 通过AddFieldValue添加记录时，作为临时缓存，每添加完一条记录，就会清空该缓存来准备下一条记录的添加。这种情况，该字段不适用于查询
     */
    std::string tmpBuffer_;
};