//
// Created by nutta on 6/28/2025.
//
#include <nexus/memory/Buffer.h>

USING_NAMESPACE_NXS;

Buffer::Buffer(Buffer&& rhs) noexcept
{
    m_buffer = std::move(rhs.m_buffer);
    m_size   = rhs.m_size;
}

Buffer& Buffer::operator = (Buffer&& rhs) noexcept
{
    m_buffer = std::move(rhs.m_buffer);
    m_size   = rhs.m_size;
    return *this;
}

void Buffer::Copy(const uint8_t* data, const uint64_t size)
{
    m_buffer = std::make_unique<uint8_t[]>(size);
    memcpy(m_buffer.get(), data, size);
    m_size   = size;
}

void Buffer::Take(uint8_t* data, const uint64_t size)
{
    m_buffer = std::unique_ptr<uint8_t[]>(data);
    m_size = size;
}

uint8_t* Buffer::Give(uint64_t& outSize)
{
    outSize = m_size;
    uint8_t* data = m_buffer.get();
    
    m_size = 0;
    m_buffer.release();
    return data;
}
