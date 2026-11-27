//
// Created by nutta on 6/28/2025.
//
#include "nexus/memory/Memory.h"
#include <algorithm>
#include <cstring> // For memcpy

USING_NAMESPACE_NXS;

CopyBuffer::CopyBuffer(const uint8_t* data, size_t size)
{
    if (!data || size == 0) return;

    m_buffer = Ptr<uint8_t[]>(new uint8_t[size]);
    NXS_ASSERT_MSG(m_buffer, std::format("Failed to allocate {} bytes!", size));
    memcpy(m_buffer.get(), data, size);
    m_size = size;
}

CopyBuffer::CopyBuffer(CopyBuffer&& rhs) noexcept
{
    m_buffer = std::move(rhs.m_buffer);
    m_size   = rhs.m_size;
    rhs.m_size = 0;
}

CopyBuffer::~CopyBuffer()
{
    m_buffer.reset();
    m_size = 0;
}

CopyBuffer& CopyBuffer::operator = (CopyBuffer&& rhs) noexcept
{
    if (this == &rhs) return *this;

    m_buffer = std::move(rhs.m_buffer);
    m_size   = rhs.m_size;
    rhs.m_size = 0;
    return *this;
}

uint64_t CopyBuffer::CopyData(uint8_t* data, const uint64_t bytes, const uint64_t offset)
{
    if (!bytes || !data) return 0;

    if (const auto newSize = offset + bytes; newSize > m_size)
    {
        if (Resize(newSize) == 0) return 0;
    }
    memcpy(m_buffer.get() + offset, data, bytes);
    return bytes;
}

uint64_t CopyBuffer::Resize(uint64_t bytes)
{
    if (bytes == m_size) return bytes;

    if (bytes == 0)
    {
        m_buffer.reset();
        m_size = 0;
        return 0;
    }

    auto newBuffer = Ptr<uint8_t[]>(new uint8_t[bytes]);
    if (!newBuffer)
    {
        LOG_FATAL(LogMemory, std::format("Failed to allocate {} bytes!", bytes));
        return 0;
    }

    // Copy the old data (if exists).
    if (m_buffer)
    {
        const uint64_t bytesToCopy = std::min(m_size, bytes);
        memcpy(newBuffer.get(), m_buffer.get(), bytesToCopy);
    }
    m_buffer = std::move(newBuffer);
    m_size = bytes;
    return bytes;
}