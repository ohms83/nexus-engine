#include "memory/BorrowBuffer.h"

USING_NAMESPACE_NXS;

BorrowBuffer::~BorrowBuffer()
{
    Release();
}

BorrowBuffer::BorrowBuffer(uint8* data, const uint64 size)
    : m_data(data)
    , m_size(size)
{
}


void BorrowBuffer::Borrow(uint8* data, const uint64 size)
{
    m_data = data;
    m_size = size;
}

uint64_t BorrowBuffer::CopyData(uint8* data, const uint64_t bytes, const uint64_t offset)
{
    if (!data || !bytes || !m_data) return 0;

    // The managed buffer is immutable; therefore, the function can only copy as much as
    // the underlying buffer can hold.
    const auto numCopied = std::min(bytes, m_size - offset);
    std::memcpy(m_data + offset, data, numCopied);
    return numCopied;
}
