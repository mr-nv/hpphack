#pragma once

#include <assert.h>
#include "platform.hpp"

void UTLMEMORY_TRACK_FREE();
void MEM_ALLOC_CREDIT_CLASS();
void UTLMEMORY_TRACK_ALLOC();

template< class T, class I = int >
class CUtlMemory
{
public:
    // constructor, destructor
    CUtlMemory(int nGrowSize = 0, int nInitSize = 0);
    CUtlMemory(T* pMemory, int numElements);
    CUtlMemory(const T* pMemory, int numElements);
    ~CUtlMemory();

    // Set the size by which the memory grows
    void Init(int nGrowSize = 0, int nInitSize = 0);

    class Iterator_t
    {
    public:
        Iterator_t(I i) : index(i) {}
        I index;

        bool operator==(const Iterator_t it) const { return index == it.index; }
        bool operator!=(const Iterator_t it) const { return index != it.index; }
    };
    Iterator_t First() const { return Iterator_t(IsIdxValid(0) ? 0 : InvalidIndex()); }
    Iterator_t Next(const Iterator_t &it) const { return Iterator_t(IsIdxValid(it.index + 1) ? it.index + 1 : InvalidIndex()); }
    I GetIndex(const Iterator_t &it) const { return it.index; }
    bool IsIdxAfter(I i, const Iterator_t &it) const { return i > it.index; }
    bool IsValidIterator(const Iterator_t &it) const { return IsIdxValid(it.index); }
    Iterator_t InvalidIterator() const { return Iterator_t(InvalidIndex()); }

    // element access
	T& operator[](I i) { return m_pMemory[i]; }
    const T& operator[](I i) const;
    T& Element(I i);
    const T& Element(I i) const;

    bool IsIdxValid(I i) const;

    static const I INVALID_INDEX = (I)-1; // For use with COMPILE_TIME_ASSERT
    static I InvalidIndex() { return INVALID_INDEX; }

	T* Base() { return m_pMemory; }

    const T* Base() const;

    void SetExternalBuffer(T* pMemory, int numElements);
    void SetExternalBuffer(const T* pMemory, int numElements);
    void AssumeMemory(T *pMemory, int nSize);
    T* Detach();
    void *DetachMemory();

    void Swap(CUtlMemory< T, I > &mem);
    void ConvertToGrowableMemory(int nGrowSize);
    int NumAllocated() const;
    int Count() const;
    void Grow(int num = 1);
    void EnsureCapacity(int num);
    void Purge();
    void Purge(int numElements);
    bool IsExternallyAllocated() const;
    bool IsReadOnly() const;
    void SetGrowSize(int size);

protected:
    void ValidateGrowSize()
    {

    }

    enum
    {
        EXTERNAL_BUFFER_MARKER = -1,
        EXTERNAL_CONST_BUFFER_MARKER = -2,
    };

    T* m_pMemory;
    int m_nAllocationCount;
    int m_nGrowSize;
};

//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------

template< class T, class I >
CUtlMemory<T, I>::CUtlMemory(int nGrowSize, int nInitAllocationCount) : m_pMemory(0),
m_nAllocationCount(nInitAllocationCount), m_nGrowSize(nGrowSize)
{
    ValidateGrowSize();
    assert(nGrowSize >= 0);
    if(m_nAllocationCount) {
        m_pMemory = (T*)new unsigned char[m_nAllocationCount * sizeof(T)];
        //m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
    }
}

template< class T, class I >
CUtlMemory<T, I>::CUtlMemory(T* pMemory, int numElements) : m_pMemory(pMemory),
m_nAllocationCount(numElements)
{
    // Special marker indicating externally supplied modifyable memory
    m_nGrowSize = EXTERNAL_BUFFER_MARKER;
}

template< class T, class I >
CUtlMemory<T, I>::CUtlMemory(const T* pMemory, int numElements) : m_pMemory((T*)pMemory),
m_nAllocationCount(numElements)
{
    // Special marker indicating externally supplied modifyable memory
    m_nGrowSize = EXTERNAL_CONST_BUFFER_MARKER;
}

template< class T, class I >
CUtlMemory<T, I>::~CUtlMemory()
{
    //Purge();
}

template< class T, class I >
void CUtlMemory<T, I>::Init(int nGrowSize /*= 0*/, int nInitSize /*= 0*/)
{
    //Purge();

    m_nGrowSize = nGrowSize;
    m_nAllocationCount = nInitSize;
    ValidateGrowSize();
    assert(nGrowSize >= 0);
    if(m_nAllocationCount) {
        UTLMEMORY_TRACK_ALLOC();
        MEM_ALLOC_CREDIT_CLASS();
        m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
    }
}

//-----------------------------------------------------------------------------
// Fast swap
//-----------------------------------------------------------------------------
template< class T, class I >
void CUtlMemory<T, I>::Swap(CUtlMemory<T, I> &mem)
{
    V_swap(m_nGrowSize, mem.m_nGrowSize);
    V_swap(m_pMemory, mem.m_pMemory);
    V_swap(m_nAllocationCount, mem.m_nAllocationCount);
}


//-----------------------------------------------------------------------------
// Switches the buffer from an external memory buffer to a reallocatable buffer
//-----------------------------------------------------------------------------
template< class T, class I >
void CUtlMemory<T, I>::ConvertToGrowableMemory(int nGrowSize)
{
    if(!IsExternallyAllocated())
        return;

    m_nGrowSize = nGrowSize;
    if(m_nAllocationCount) {
        int nNumBytes = m_nAllocationCount * sizeof(T);
        T *pMemory = (T*)malloc(nNumBytes);
        memcpy(pMemory, m_pMemory, nNumBytes);
        m_pMemory = pMemory;
    } else {
        m_pMemory = NULL;
    }
}

