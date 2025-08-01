//
// Created by nutta on 8/1/2025.
//

#pragma once

#include <string>
#include <cassert>

namespace nxs
{
    typedef int8_t      int8;
    typedef int16_t     int16;
    typedef int32_t     int32;
    typedef int64_t     int64;

    typedef uint8_t     uint8;
    typedef uint16_t    uint16;
    typedef uint32_t    uint32;
    typedef uint64_t    uint64;

    enum class DataType
    {
        None,
        Bool,
        Byte,
        Short,
        Int32,
        Int64,
        UByte,
        UShort,
        UInt32,
        UInt64,
        Float,
        Double,
        String,
        Array,
        Map,
        Num
    };

    inline size_t NxsDataTypeSize(const DataType type)
    {
        switch (type)
        {
        case DataType::UByte:
        case DataType::Byte:
            return sizeof(uint8);
        case DataType::UShort:
        case DataType::Short:
            return sizeof(uint16);
        case DataType::UInt32:
        case DataType::Int32:
            return sizeof(uint32);
        case DataType::UInt64:
        case DataType::Int64:
            return sizeof(uint64);
        case DataType::Float:
            return sizeof(float);
        case DataType::Double:
            return sizeof(double);
        default:
            assert(0);
            return 0;
        }
    }

    // --- Static Helper Function ---
    inline std::string NxsGetTypeString(const DataType type)
    {
        switch (type)
        {
        case DataType::Int64: return "Int64";
        case DataType::Int32: return "Int32";
        case DataType::Short: return "Short";
        case DataType::Byte: return "Byte";
        case DataType::UInt64: return "UInt64";
        case DataType::UInt32: return "UInt32";
        case DataType::UShort: return "UShort";
        case DataType::UByte: return "UByte";
        case DataType::Float: return "Float";
        case DataType::Double: return "Double";
        case DataType::Bool: return "Bool";
        case DataType::String: return "String";
        case DataType::Array: return "Array";
        case DataType::Map: return "Map";
        case DataType::None: return "None";
        case DataType::Num: return "Num (Sentinel)";
        default: break;
        }
        return "Unknown DataType";
    }
}
