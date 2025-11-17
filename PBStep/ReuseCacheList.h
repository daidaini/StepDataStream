/*
 * @Descripttion: 该类主要特单是复用已申请内存，已申请的内存不释放，在重置后可继续使用。
 * 该类功能及使用场景是：不停地需要临时缓存某些数据，而且这些数据长度不限需要动态管理。
 * 这种场景下，当需要缓存数据时，使用该类进行缓存；当数据可以失效后进行清理重置；然后下次新的数据需要缓存时进行复用
 * @Author: yubo
 * @Date: 2022-09-06 09:59:29
 * @LastEditTime: 2022-09-08 09:31:57
 */
#pragma once

#include <string.h>

namespace pobo
{
    class CacheNode
    {
    public:
        CacheNode(int nBufSize);

        ~CacheNode();

        /**取出缓冲区头上的一块数据
         * @param len 输入：取出的数据长度；
         * @return 取到的数据。若未取到数据则为空
         */
        void *GetData(int len);

        /**将缓冲区最前面的数据移丢弃
         * @param len 丢弃数据的长度
         * @return 实际丢弃数据的长度
         * 注：此"丢弃"非真的删除数据，只是将内部指针后移，对len长度的数据不再可取
         */
        void PopFront(int len);

        /**向缓冲区尾部加入一块数据
         * @param pData 加入数据的指针
         * @param nDataLen 加入数据的长度
         * @return 加入缓冲区的地址
         */
        char *PushBack(const char *pData, int nDataLen);

        void ResetData();

        void SetNext(CacheNode *pNext)
        {
            m_pNext = pNext;
        }

        CacheNode *GetNext()
        {
            return m_pNext;
        }

        int Length() const
        {
            return m_nLength;
        }

    private:
        int m_nBufSize;     /**< 缓冲区长度 */
        char *m_pBuffer;    /**< 缓冲区指针 */
        int m_nLength;      /**< 本结点已占用的数据长度 */
        char *m_pData;      /**< 针向实际的数据块 */
        CacheNode *m_pNext; /**< 下一个结点的指针 */
    };

    class ReuseCacheList final
    {
    public:
        /**构造函数
         * @param nDataBlockSize 每个结点可存贮数据的长度，缺省值为1M
         */
        explicit ReuseCacheList(int nDataBlockSize);

        /**析构造函数
         */
        virtual ~ReuseCacheList();

        ReuseCacheList(ReuseCacheList &) = delete;
        ReuseCacheList &operator=(ReuseCacheList &) = delete;

        /**取出队列头上的一块数据
         * @param len 输入：希望取出的数据长度；输出：实际取到的长度，不超过输入参数
         * @return 取到的数据。若未取到数据则为空
         */
        // void *GetData(int len);

        /**将队列最前面的数据移丢弃
         * @param len 丢弃数据的长度
         * 只丢弃当前结点的数据
         */
        void PopFront(int len);

        /**向队列尾部加入一块数据
         * @param pData 加入数据的指针
         * @param nDataLen 加入数据的长度
         * @return 加入缓冲区的地址
         */
        char *PushBack(const char *pData, int nDataLen);

        /**检查队列是否为空
         * @return true 队列中无数据 false 队列中有数据
         */
        // bool IsEmpty();

        /**缓存重置
         */
        void Reset();

    private:
        int m_nCapacity = 1;      // 总节点个数
        int m_nDataBlockSize = 0; /**< 每个结点可存贮数据的长度*/

        CacheNode *m_pCacheHead;    /**< 头结点 */
        CacheNode *m_pCacheCurrent; /**< 当前结点 */
        CacheNode *m_pCacheTail;    /**< 尾结点 */
    };
}