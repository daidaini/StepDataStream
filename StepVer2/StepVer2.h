/*
 * @Description: 新的PBStep实现，完全替代CachedPBStep的功能
 * @Author: yubo
 * @Date: 2025-01-24
 */
#pragma once

#include "MemoryPool.h"
#include "StringExtensions.h"

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <assert.h>
#include <stdexcept>

namespace stepver2
{
    class StepVer2
    {
    public:
        // 保持与原类相同的常量定义
        static constexpr int CacheBlockSize = 16 * 1024; // 每条记录的最大长度, 暂定16k

        explicit StepVer2(int blockSize = CacheBlockSize);
        virtual ~StepVer2() = default;

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

    protected:
        int blockSize_;
        //<id, val>
        std::map<int, std::string> baseRecord_;

        // 记录信息结构体
        struct RecordInfo
        {
            char *data; // 数据指针
            int length; // 数据长度

            RecordInfo() : data(nullptr), length(0) {}
            RecordInfo(char *d, int l) : data(d), length(l) {}
        };

        // body 存储的内容格式还是 id=value&id=value
        std::vector<RecordInfo> bodyRecords_;

        MemoryPool memoryPool_;

        // 包体的当前记录索引, 没有记录时必须为-1
        int currentRecIndex_ = -1;

        /* 用于数据缓存
         * 该字段的作用：
         * 1) 通过SetPackage反序列化后，tmpBuffer_ 存储的是包体的最后一条记录，对于请求来说，就是包体数据。这种情况，适用于通过该字段来查询对应stepid的数据值
         * 2) 通过AddFieldValue添加记录时，作为临时缓存，每添加完一条记录，就会清空该缓存来准备下一条记录的添加。这种情况，该字段不适用于查询
         */
        std::string tmpBuffer_;
    };
}
