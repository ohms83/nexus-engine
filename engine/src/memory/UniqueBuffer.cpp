//
// Created by nutta on 6/28/2025.
//
#include "nexus/memory/UniqueBuffer.h"

USING_NAMESPACE_NXS;

UniqueBuffer::UniqueBuffer(UniqueBuffer&& rhs) noexcept
{
    m_buffer = std::move(rhs.m_buffer);
    m_size   = rhs.m_size;
}

UniqueBuffer& UniqueBuffer::operator = (UniqueBuffer&& rhs) noexcept
{
    m_buffer = std::move(rhs.m_buffer);
    m_size   = rhs.m_size;
    return *this;
}

void UniqueBuffer::Copy(const uint8_t* data, const uint64_t size)
{
    if (!size || !data) return;

    m_buffer = std::make_unique<uint8_t[]>(size);
    memcpy(m_buffer.get(), data, size);
    m_size   = size;
}

void UniqueBuffer::Take(uint8_t* data, const uint64_t size)
{
    if (!size || !data) return;

    m_buffer = std::unique_ptr<uint8_t[]>(data);
    m_size = size;
}

uint8_t* UniqueBuffer::Give(uint64_t& outSize)
{
    outSize = m_size;
    uint8_t* data = m_buffer.get();
    
    m_size = 0;
    m_buffer.release();
    return data;
}
