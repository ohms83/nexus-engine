#include "Data.hpp"

#include <iostream>
#include <cstring>

USING_NAMESPACE_NXS;

Data::Data(Data&& rhs) noexcept
{
    _buffer = std::move(rhs._buffer);
    _size   = rhs._size;
}

Data& Data::operator = (Data&& rhs) noexcept
{
    _buffer = std::move(rhs._buffer);
    _size   = rhs._size;
    return *this;
}

void Data::copy(char* data, uint64_t size)
{
    char* tmpBuf = new char[size] { 0 };
    memcpy(tmpBuf, data, size);

    _buffer = std::unique_ptr<char>(tmpBuf);
    _size   = size;
}

void Data::take(char* data, uint64_t size)
{
    _buffer = std::unique_ptr<char>(data);
    _size = size;
}

char* Data::give(uint64_t& outSize)
{
    outSize = _size;
}