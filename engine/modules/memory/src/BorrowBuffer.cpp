#include "nexus/memory/BorrowBuffer.h"
#include <cstring> // For memcpy

USING_NAMESPACE_NXS;

BorrowBuffer::~BorrowBuffer()
{
    Release();
}

BorrowBuffer::BorrowBuffer(uint8_t* data, const uint64_t size)
    : m_data(data)
    , m_size(size)
{
}


void BorrowBuffer::Borrow(uint8_t* data, const uint64_t size)
{
    m_data = data;
    m_size = size;
}

uint64_t BorrowBuffer::CopyData(uint8_t* data, const uint64_t bytes, const uint64_t offset)
{
    if (!data || !bytes || !m_data) return 0;

    // The managed buffer is immutable; therefore, the function can only copy as much as
    // the underlying buffer can hold.
    const auto numCopied = std::min(bytes, m_size - offset);
    memcpy(m_data + offset, data, numCopied);
    return numCopied;
}
