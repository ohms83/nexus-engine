#include "memory/BorrowBuffer.h"

USING_NAMESPACE_NXS;

BorrowBuffer::~BorrowBuffer()
{
    Release();
}

BorrowBuffer::BorrowBuffer(const uint8* data, uint64 size)
    : m_data(data)
    , m_size(size)
{
}


void BorrowBuffer::Borrow(const uint8* data, uint64 size)
{
    m_data = data;
    m_size = size;
}