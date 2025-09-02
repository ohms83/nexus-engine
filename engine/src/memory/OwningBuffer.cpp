//
// Created by nutta on 6/28/2025.
//
#include "nexus/memory/OwningBuffer.h"

#include <format>

USING_NAMESPACE_NXS;

OwningBuffer::OwningBuffer(uint8* data, size_t size)
{
    m_buffer = std::unique_ptr<uint8_t[]>(data);
    m_size = size;
}

OwningBuffer::OwningBuffer(OwningBuffer&& rhs) noexcept
{
    m_buffer = std::move(rhs.m_buffer);
    m_size   = rhs.m_size;
}

OwningBuffer& OwningBuffer::operator = (OwningBuffer&& rhs) noexcept
{
    m_buffer = std::move(rhs.m_buffer);
    m_size   = rhs.m_size;
    return *this;
}

uint64_t OwningBuffer::CopyData(uint8_t* data, const uint64_t bytes, const uint64_t offset)
{
    if (!bytes || !data) return 0;

    if (const auto newSize = offset + bytes; newSize > m_size)
    {
        // Resize the buffer.
        auto newBuffer = std::unique_ptr<uint8_t[]>(new uint8_t[newSize]);
        // Copy the old data (if exists).
        if (m_buffer) std::memcpy(newBuffer.get(), m_buffer.get(), m_size);
        m_buffer = std::move(newBuffer);
        m_size = newSize;
    }
    std::memcpy(m_buffer.get() + offset, data, bytes);
    return bytes;
}

void OwningBuffer::Take(uint8_t*& data, const uint64_t size)
{
    if (!size || !data) return;

    m_buffer = std::unique_ptr<uint8_t[]>(data);
    m_size = size;
    data = nullptr;
}

uint8_t* OwningBuffer::Give(uint64_t& outSize)
{
    outSize = m_size;
    uint8_t* data = m_buffer.get();

    m_size = 0;
    m_buffer.release();
    return data;
}
