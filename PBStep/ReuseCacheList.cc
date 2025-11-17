#include "ReuseCacheList.h"

namespace pobo
{
    CacheNode::CacheNode(int nBufSize)
        : m_nBufSize(nBufSize),
          m_nLength(0), m_pNext(nullptr)
    {
        m_pBuffer = new char[m_nBufSize]{};
        m_pData = m_pBuffer;
    }

    CacheNode::~CacheNode()
    {
        delete[] m_pBuffer;
    }

    void *CacheNode::GetData(int len)
    {
        if (m_nLength < len)
            return nullptr;

        return m_pData;
    }

    void CacheNode::PopFront(int len)
    {
        if (len > m_nLength)
            len = m_nLength;

        m_pData += len;
        m_nLength -= len;
    }

    char *CacheNode::PushBack(const char *pData, int nDataLen)
    {
        if (nDataLen > m_nBufSize)
        {
            return nullptr;
        }

        if (m_nBufSize - m_nLength < nDataLen)
        {
            return nullptr;
        }

        char *pTarget = m_pData + m_nLength;
        ::memcpy(pTarget, pData, nDataLen);
        m_nLength += nDataLen;
        return pTarget;
    }

    void CacheNode::ResetData()
    {
        m_nLength = 0;
        m_pData = m_pBuffer;
    }

    ReuseCacheList::ReuseCacheList(int nDataBlockSize)
        : m_nDataBlockSize(nDataBlockSize)
    {
        m_pCacheHead = new CacheNode(nDataBlockSize);
        m_pCacheCurrent = m_pCacheTail = m_pCacheHead;
    }

    ReuseCacheList::~ReuseCacheList()
    {
        CacheNode *pNode = m_pCacheHead;
        CacheNode *pNext = nullptr;
        while (pNode != nullptr)
        {
            pNext = pNode->GetNext();
            delete pNode;
            pNode = pNext;
        }
    }

    char *ReuseCacheList::PushBack(const char *pData, int nDataLen)
    {
        while (m_pCacheCurrent != nullptr)
        {
            char *pTarget = m_pCacheCurrent->PushBack(pData, nDataLen);
            if (pTarget != nullptr)
            {
                return pTarget;
            }
            m_pCacheCurrent = m_pCacheCurrent->GetNext();
        }

        // 最后的结点空间已经用完，分配一个新的结点
        CacheNode *pNode = new CacheNode(m_nDataBlockSize);

        ++m_nCapacity;

        m_pCacheTail->SetNext(pNode);
        m_pCacheCurrent = m_pCacheTail = pNode;

        // 直接将数据复制进最后的结点
        return m_pCacheCurrent->PushBack(pData, nDataLen);
    }

    void ReuseCacheList::Reset()
    {
        m_pCacheCurrent = m_pCacheHead;
        while (m_pCacheCurrent != nullptr)
        {
            m_pCacheCurrent->ResetData();
            m_pCacheCurrent = m_pCacheCurrent->GetNext();
        }
        m_pCacheCurrent = m_pCacheHead;
    }

    void ReuseCacheList::PopFront(int len)
    {
        m_pCacheCurrent->PopFront(len);

        if (m_pCacheCurrent->Length() <= 0)
        {
            // 如果有下个节点则移到下个结点
            if (m_pCacheCurrent->GetNext() != nullptr)
            {
                m_pCacheCurrent = m_pCacheCurrent->GetNext();
            }
        }
    }
}