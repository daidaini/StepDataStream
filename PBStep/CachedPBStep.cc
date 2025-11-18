#include "CachedPBStep.h"

#include "StringFunc.h"
#include "stepdef.h"

// 反转义字符对应
static const std::map<char, char> s_EscapeBackItemMap{
    {'\\', '\\'}, {'a', '='}, {'b', '&'}, {'n', '\n'}};

// 转义字符对应
static const std::map<char, char> s_EscapeItemMap{
    {'\\', '\\'}, {'=', 'a'}, {'&', 'b'}, {'\n', 'n'}};

CachedPBStep::CachedPBStep(int blockSize)
    : blockSize_(blockSize), cachePoolPtr_(new pobo::ReuseCacheList(blockSize))
{
    tmpBuffer_.reserve(1024);
    bodyRecords_.reserve(128);
}

CachedPBStep::~CachedPBStep()
{
    delete cachePoolPtr_;
}

void CachedPBStep::Init()
{
    baseRecord_.clear();
    bodyRecords_.clear();
    cachePoolPtr_->Reset();
    tmpBuffer_.clear();

    currentRecIndex_ = -1;
}

bool CachedPBStep::SetPackage(const std::string &src)
{
    auto records = str::Split(src, '\n');
    if (records.empty())
    {
        return false;
    }

    Init();
    ParseBaseRecord(records.front());

    for (size_t i = 1; i < records.size(); ++i)
    {
        GotoNext();
        if(records[i].empty())
        {
            continue;
        }

        auto &currBuf = records[i];
        if (currBuf.back() != '&')
        {
            currBuf.push_back('&'); // 补位
        }

        char *cachePtr = cachePoolPtr_->PushBack(currBuf.data(), currBuf.size());

        bodyRecords_.emplace_back(std::make_pair(cachePtr, currBuf.size()));
    }
    GotoFirst();
    return true;
}

void CachedPBStep::ParseBaseRecord(const std::string &baseStr)
{
    auto items = str::Split(baseStr, '&');
    for (const auto &item : items)
    {
        size_t index = item.find_first_of('=');
        if (index == item.npos)
        {
            return;
        }

        int stepid = atoi(item.substr(0, index).c_str());

        std::string value;
        if (index != item.size() - 1)
        {
            value = item.substr(index + 1, item.size() - index - 1);
        }

        baseRecord_.emplace(stepid, EscapeBackItem(value));
    }
}

std::string CachedPBStep::ToString()
{
    std::string result;
    if (!bodyRecords_.empty())
    {
        result.reserve(bodyRecords_.size() * bodyRecords_.front().second + 1024);
    }

    result = BaseRecord();

    for (auto &item : bodyRecords_)
    {
        result.append((const char *)item.first, item.second);

        result.push_back('\n');
    }

    return result;
}

std::string CachedPBStep::BaseRecord() const
{
    std::string result;
    for (const auto &item : baseRecord_)
    {
        result.append(std::to_string(item.first));
        result.push_back('=');
        result.append(item.second.empty() ? "" : item.second);
        result.push_back('&');
    }
    result.push_back('\n');
    return result;
}

std::string CachedPBStep::FormatedRecords(int start, int end)
{
    if (start < 0 || end <= start)
    {
        return "";
    }

    std::string result;
    if (start < end)
    {
        result.reserve((end - start) * bodyRecords_[start].second + 1024);
    }

    for (int i = start; i < end && i < (int)bodyRecords_.size(); ++i)
    {
        result.append((const char *)bodyRecords_[i].first, bodyRecords_[i].second);
        result.push_back('\n');
    }

    return result;
}

void CachedPBStep::AppendRecord()
{
    if (!tmpBuffer_.empty())
    {
        EndAppendRecord();
        // should not here
        tmpBuffer_.clear();
    }

    bodyRecords_.emplace_back(std::make_pair(nullptr, 0));
    // 序号更新到下一条
    GotoNext();
}

void CachedPBStep::EndAppendRecord()
{
    if (tmpBuffer_.empty())
    {
        return;
    }

    // 首先尝试将数据存储到当前缓存池
    char *ptr = cachePoolPtr_->PushBack(tmpBuffer_.data(), tmpBuffer_.size());
    if (ptr == nullptr)
    {
        // 当前缓存池空间不足，尝试扩容
        CheckAndExpandCache(tmpBuffer_.size());

        // 扩容后再次尝试存储
        ptr = cachePoolPtr_->PushBack(tmpBuffer_.data(), tmpBuffer_.size());
        if (ptr == nullptr)
        {
            // 扩容后仍然无法存储，抛出异常
            throw std::runtime_error("[CachedPBStep]CachePool size is not enough to store data after expansion.");
        }
    }

    bodyRecords_.back().first = ptr;
    bodyRecords_.back().second = tmpBuffer_.size();

    tmpBuffer_.clear();
}

void CachedPBStep::GotoFirst()
{
    currentRecIndex_ = 0;
}

void CachedPBStep::GotoNext()
{
    ++currentRecIndex_;
}

std::pair<const char *, int> CachedPBStep::FindItem(int stepid)
{
    if (currentRecIndex_ < 0 || bodyRecords_.empty())
    { // 无包体记录
        return {nullptr, 0};
    }

    const char *begin = (const char *)bodyRecords_[currentRecIndex_].first;
    int limitedLen = bodyRecords_[currentRecIndex_].second;

    if (begin == nullptr)
    { // 记录尚未添加记录
        return {nullptr, 0};
    }

    char key[16];
    int keylen = sprintf(key, "%d=", stepid);

    if (::strncmp(begin, key, strlen(key)) == 0)
    {
        // 匹配到头
        const char *tValPtr = begin + strlen(key);
        const char *tValueEnd = ::strstr(tValPtr, "&");
        int tLen = tValueEnd - tValPtr;
        return std::make_pair(tValPtr, tLen);
    }
    else
    {
        // 非头，则修改key继续查找
        keylen = sprintf(key, "&%d=", stepid);
    }

    const char *fPtr = ::strstr(begin, key);
    // 确认非key的情况
    // 1. 返回为空
    // 2. 返回指针与头之间的距离超出限制
    if (fPtr == nullptr)
    {
        return {nullptr, 0};
    }
    const char *valPtr = fPtr + keylen;
    if (int(fPtr - begin) + keylen >= limitedLen)
    {
        return {nullptr, 0};
    }

    const char *valueEnd = ::strstr(valPtr, "&");
    int len = 0;
    if (valueEnd == nullptr) // 最后一个字段
    {
        len = limitedLen - (valPtr - begin);
    }
    else
    {
        len = valueEnd - valPtr;
    }

    return std::make_pair(valPtr, len);
}

std::pair<const char *, int> CachedPBStep::FindItemByBuffer(int stepid)
{
    size_t pos = 0;
    std::string key = std::to_string(stepid);
    key.push_back('=');

    if (tmpBuffer_.compare(0, key.size(), key) != 0)
    {
        key = "&" + key;
        pos = tmpBuffer_.find(key);
        if (pos == tmpBuffer_.npos)
        {
            return {nullptr, 0};
        }
    }

    pos += key.size();

    size_t valPos = tmpBuffer_.find('&', pos);

    int len = 0;
    if (valPos == tmpBuffer_.npos) // 表示最后一个字段没有&结尾
    {
        len = tmpBuffer_.size() - pos;
    }
    else
    {
        len = valPos - pos;
    }

    if (len <= 0)
    {
        return {nullptr, 0};
    }

    return std::make_pair(tmpBuffer_.data() + pos, len);
}

std::string CachedPBStep::GetItem(int stepid)
{
    std::pair<const char *, int> result = FindItem(stepid);
    if (result.first != nullptr)
    {
        return std::string(result.first, result.second);
    }
    return "";
}

std::string CachedPBStep::GetStepValueByID(int stepid)
{
    std::pair<const char *, int> result = FindItem(stepid);

    if (result.first != nullptr)
    {
        return EscapeBackItem(std::string(result.first, result.second));
    }

    return "";
}

std::string CachedPBStep::GetBaseFieldValue(int stepid)
{
    auto it = baseRecord_.find(stepid);
    if (it != baseRecord_.end())
        return it->second;

    return "";
}

bool CachedPBStep::SetFieldValue(int stepid, const char *value)
{
    if (currentRecIndex_ < 0 || bodyRecords_.empty())
    {
        return false; // 包体为空时，设置值失败
    }

    int recSize = bodyRecords_[currentRecIndex_].second;
    if (recSize == 0)
    {
        return false;
    }

    std::string currRec{"&"};
    currRec.append(bodyRecords_[currentRecIndex_].first, recSize);
    char key[16];
    sprintf(key, "&%d=", stepid);
    size_t pos = currRec.find(key);
    if (pos == currRec.npos)
    {
        currRec += key;
        currRec += value;
        currRec += '&';
    }
    else
    {
        std::string newStr = currRec.substr(0, pos + strlen(key));
        newStr += value;

        size_t nextPos = currRec.find('&', pos + 1);
        if (nextPos != currRec.npos)
        {
            newStr.append(currRec.substr(nextPos));
        }

        currRec = std::move(newStr);
    }

    bodyRecords_[currentRecIndex_].second = std::min(int(currRec.size() - 1), blockSize_ - 1);
    strncpy(bodyRecords_[currentRecIndex_].first, currRec.data() + 1, bodyRecords_[currentRecIndex_].second);
    return true;
}

bool CachedPBStep::SetFieldValueInt(int stepid, int value)
{
    char cval[128]{};
    snprintf(cval, sizeof(cval), "%d", value);
    return SetFieldValue(stepid, cval);
}

void CachedPBStep::SetBaseFieldValueInt(int stepid, int value)
{
    baseRecord_[stepid] = std::to_string(value);
}

void CachedPBStep::SetBaseFieldValueString(int stepid, const std::string &value)
{
    baseRecord_[stepid] = EscapeItem(value);
}

std::string CachedPBStep::EscapeItem(const std::string &src)
{
    std::string result;
    result.reserve(src.length() + 8);
    for (char elem : src)
    {
        auto it = s_EscapeItemMap.find(elem);
        if (it == s_EscapeItemMap.end())
        {
            result.push_back(elem);
        }
        else
        {
            result.push_back('\\');
            result.push_back(it->second);
        }
    }
    return result;
}

std::string CachedPBStep::EscapeBackItem(const std::string &src)
{
    if (src.empty() || src.size() < 2)
    {
        return src;
    }

    std::string result;
    result.reserve(src.length());

    for (size_t i = 0; i < src.size(); ++i)
    {
        if (src[i] == '\\' && i != src.size() - 1)
        {
            char elem = src[i + 1];
            auto it = s_EscapeBackItemMap.find(elem);
            if (it != s_EscapeBackItemMap.end())
            { // 需转义
                result.push_back(it->second);
                ++i;
            }
            else
            {
                result.push_back(src[i]);
            }
        }
        else
        {
            result.push_back(src[i]);
        }
    }

    return result;
}

int CachedPBStep::GetNextTierBlockSize(int currentSize) const
{
    if (currentSize <= CacheBlockSizeTier1)
    {
        return CacheBlockSizeTier2; // 从第一档扩容到第二档
    }
    else if (currentSize <= CacheBlockSizeTier2)
    {
        return CacheBlockSizeTier3; // 从第二档扩容到第三档
    }
    else
    {
        return -1; // 第三档也无法满足，返回-1表示无法扩容
        
    }
}

void CachedPBStep::CheckAndExpandCache(size_t requiredSize)
{
    if (requiredSize <= static_cast<size_t>(blockSize_))
    {
        return; // 当前block size足够，无需扩容
    }

    // 尝试扩容到下一档
    int nextTierSize = GetNextTierBlockSize(blockSize_);
    if (nextTierSize == -1)
    {
        // 已经是第三档或无法扩容，抛出异常
        throw std::runtime_error("[CachedPBStep]Required size " + std::to_string(requiredSize) +
                               " exceeds maximum cache block size " + std::to_string(CacheBlockSizeTier3));
    }

    // 扩容到下一档：保存现有数据，重建缓存池
    int oldBlockSize = blockSize_;

    // 临时保存所有现有记录的数据
    std::vector<std::string> savedRecords;
    for (const auto& record : bodyRecords_)
    {
        if (record.first != nullptr && record.second > 0)
        {
            savedRecords.emplace_back(std::string(record.first, record.second));
        }
    }

    // 更新block size
    blockSize_ = nextTierSize;

    // 重建缓存池：删除旧的，创建新的
    delete cachePoolPtr_;
    cachePoolPtr_ = new pobo::ReuseCacheList(blockSize_);

    // 重新添加所有保存的记录
    bodyRecords_.clear();
    for (const auto& recordData : savedRecords)
    {
        char* ptr = cachePoolPtr_->PushBack(recordData.data(), recordData.size());
        if (ptr == nullptr)
        {
            // 重建失败，回滚并抛出异常
            blockSize_ = oldBlockSize;
            delete cachePoolPtr_;
            cachePoolPtr_ = new pobo::ReuseCacheList(blockSize_);
            bodyRecords_.clear();
            throw std::runtime_error("[CachedPBStep]Failed to rebuild cache pool during expansion");
        }
        bodyRecords_.emplace_back(std::make_pair(ptr, recordData.size()));
    }
}