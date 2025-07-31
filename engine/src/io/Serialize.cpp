//
//  Serialize.cpp
//
//  Created by nuttachai on 23/10/19.
//

#include "nexus/io/Serialize.h"
#include "nexus/core/Logger.h"
#include "nexus/math/Math.h"

#include <sstream>
#include <format>

USING_NAMESPACE_NXS;
using namespace std;

DEFINE_LOG(Serialize);

const Serializable Serializable::None;

// --- Serializable Class Implementations ---

Serializable::Serializable()
    : m_type(DataType::None)
{
    m_pValue.reset(new SerializeValue());
}

Serializable::Serializable(const Serializable& rhs)
    : m_type(rhs.GetType())
{
    CopyValue(rhs.GetType(), *rhs.m_pValue);
}

Serializable::Serializable(Serializable&& rhs) noexcept
{
    m_type = rhs.GetType();
    m_pValue = std::move(rhs.m_pValue);
    // Also set the source object's data to None to prevent errors.
    rhs.m_type = DataType::None;
    rhs.m_pValue.reset(new SerializeValue());
}

// Constructors for various types
Serializable::Serializable(const char* strValue)
    : m_type(DataType::String)
{
    m_pValue.reset(new StringSerializeValue(strValue));
}
Serializable::Serializable(uint8_t uintValue)
    : m_type(DataType::UInt64)
{
    m_pValue.reset(new UintSerializeValue(uintValue));
}
Serializable::Serializable(uint16_t uintValue)
    : m_type(DataType::UInt64)
{
    m_pValue.reset(new UintSerializeValue(uintValue));
}
Serializable::Serializable(uint32_t uintValue)
    : m_type(DataType::UInt64)
{
    m_pValue.reset(new UintSerializeValue(uintValue));
}
Serializable::Serializable(uint64_t uintValue)
    : m_type(DataType::UInt64)
{
    m_pValue.reset(new UintSerializeValue(uintValue));
}
Serializable::Serializable(int8_t intValue)
    : m_type(DataType::Int64)
{
    m_pValue.reset(new IntSerializeValue(intValue));
}
Serializable::Serializable(int16_t intValue)
    : m_type(DataType::Int64)
{
    m_pValue.reset(new IntSerializeValue(intValue));
}
Serializable::Serializable(int32_t intValue)
    : m_type(DataType::Int64)
{
    m_pValue.reset(new IntSerializeValue(intValue));
}

Serializable::Serializable(int64_t intValue)
    : m_type(DataType::Int64)
{
    m_pValue.reset(new IntSerializeValue(intValue));
}

Serializable::Serializable(double floatValue)
    : m_type(DataType::Double)
{
    m_pValue.reset(new FloatSerializeValue(floatValue));
}
Serializable::Serializable(bool boolValue)
    : m_type(DataType::Bool)
{
    m_pValue.reset(new BoolSerializeValue(boolValue));
}
Serializable::Serializable(const std::string& stringValue)
    : m_type(DataType::String)
{
    m_pValue.reset(new StringSerializeValue(stringValue));
}
Serializable::Serializable(const Serializable::Array& arrayValue)
    : m_type(DataType::Array)
{
    m_pValue.reset(new ArraySerializeValue(arrayValue));
}
Serializable::Serializable(const Serializable::Map& mapValue)
    : m_type(DataType::Map)
{
    m_pValue.reset(new MapSerializeValue(mapValue));
}
Serializable::Serializable(std::string&& stringValue)
    : m_type(DataType::String)
{
    m_pValue.reset(new StringSerializeValue(move(stringValue)));
}
Serializable::Serializable(Serializable::Array&& arrayValue)
    : m_type(DataType::Array)
{
    m_pValue.reset(new ArraySerializeValue(move(arrayValue)));
}
Serializable::Serializable(Serializable::Map&& mapValue)
    : m_type(DataType::Map)
{
    m_pValue.reset(new MapSerializeValue(move(mapValue)));
}

// --- Operator Overloads ---
Serializable::operator int64_t() const
{
    return GetInt();
}
Serializable::operator uint64_t() const
{
    return GetUint();
}
Serializable::operator double() const
{
    return GetFloat();
}
Serializable::operator bool() const
{
    return GetBoolean();
}
Serializable::operator std::string() const
{
    return GetString();
}
Serializable::operator Array() const
{
    return GetArray();
}
Serializable::operator Map() const
{
    return GetMap();
}

bool Serializable::operator == (const Serializable& rhs) const
{
    if (GetType() != rhs.GetType()) {
        return false;
    }

    switch (GetType())
    {
    case DataType::None:
        return true;
    case DataType::Bool:
        return GetBoolean() == rhs.GetBoolean();
    case DataType::Byte:
    case DataType::Short:
    case DataType::Int32:
    case DataType::Int64:
        return GetInt() == rhs.GetInt();
    case DataType::UByte:
    case DataType::UShort:
    case DataType::UInt32:
    case DataType::UInt64:
        return GetUint() == rhs.GetUint();
    case DataType::Float:
    case DataType::Double:
        return Math::Approx(GetFloat(), rhs.GetFloat());
    case DataType::String:
        return GetString() == rhs.GetString();
    case DataType::Array:
    {
        const auto& arrayValue = GetArray();
        const auto& rhsArrayValue = rhs.GetArray();
        if (arrayValue.size() != rhsArrayValue.size()) { return false; }
        for (size_t i = 0; i < arrayValue.size(); ++i) {
            if (arrayValue.at(i) != rhsArrayValue.at(i)) { return false; }
        }
        return true;
    }
    case DataType::Map:
    {
        const auto& mapValue = GetMap();
        const auto& rhsMapValue = rhs.GetMap();
        if (mapValue.size() != rhsMapValue.size()) { return false; }
        const auto& rhsMapEnd = rhsMapValue.end();
        for (const auto& elem : mapValue) {
            const auto& rhsElemItr = rhsMapValue.find(elem.first);
            if (rhsElemItr == rhsMapEnd || rhsElemItr->second != elem.second) { return false; }
        }
        return true;
    }
    case DataType::Num:
    default:
        return false;
    }
}

Serializable& Serializable::operator = (Serializable&& rhs) noexcept
{
    m_type = rhs.GetType();
    m_pValue = std::move(rhs.m_pValue);
    // Also set the source object's data to None to prevent errors.
    rhs.m_type = DataType::None;
    rhs.m_pValue.reset(new SerializeValue());
    return *this;
}

bool Serializable::operator != (const Serializable& rhs) const
{
    return !(*this == rhs);
}

const Serializable& Serializable::operator [] (const string& key) const
{
    NXS_ASSERT_MSG(GetType() == DataType::Map, std::format("Object is not a map TYPE=", INT_CAST(GetType())));
    const auto& itr = m_pValue->GetMap().find(key);
    if (itr == m_pValue->GetMap().end())
    {
        LOG_ERROR(LogSerialize, std::format("Invalid key={}", key));
        return None;
    }
    return itr->second;
}

const Serializable& Serializable::At(const string& key) const
{
    NXS_ASSERT_MSG(GetType() == DataType::Map, std::format("Object is not a map TYPE=", INT_CAST(GetType())));
    const auto& itr = m_pValue->GetMap().find(key);
    if (itr == m_pValue->GetMap().end())
    {
        LOG_ERROR(LogSerialize, std::format("Invalid key={}", key));
        return None;
    }
    return itr->second;
}

const Serializable& Serializable::operator [] (size_t index) const
{
    NXS_ASSERT_MSG(GetType() == DataType::Array, std::format("Object is not an array TYPE={}", INT_CAST(GetType())));
    const auto& array = m_pValue->GetArray();
    if (index >= array.size())
    {
        LOG_ERROR(LogSerialize, std::format("Array index out of range: {}/{}", index, array.size()));
        return None;
    }
    return array[index];
}

const Serializable& Serializable::At(size_t index) const
{
    NXS_ASSERT_MSG(GetType() == DataType::Array, std::format("Object is not an array TYPE={}", INT_CAST(GetType())));
    const auto& array = m_pValue->GetArray();
    if (index >= array.size())
    {
        LOG_ERROR(LogSerialize, std::format("Array index out of range: {}/{}", index, array.size()));
        return None;
    }
    return array[index];
}

bool Serializable::HasKey(const std::string& key) const
{
    const Map& mapObj = GetMap();
    return mapObj.find(key) != mapObj.end();
}

bool Serializable::TryGet(const std::string& key, Serializable& outValue) const
{
    const Map& mapObj = GetMap();
    auto itr = mapObj.find(key);

    if (itr != mapObj.end()) {
        outValue.CopyValue(itr->second.m_type, *itr->second.m_pValue);
        return true;
    }
    return false;
}

// --- Getter Implementations ---
int64_t Serializable::GetInt() const
{
    return m_pValue->GetInt();
}
uint64_t Serializable::GetUint() const
{
    return m_pValue->GetUint();
}
double Serializable::GetFloat() const
{
    return m_pValue->GetFloat();
}
bool Serializable::GetBoolean() const
{
    return m_pValue->GetBoolean();
}
const std::string& Serializable::GetString() const
{
    return m_pValue->GetString();
}
const Serializable::Array& Serializable::GetArray() const
{
    return m_pValue->GetArray();
}
const Serializable::Map& Serializable::GetMap() const
{
    return m_pValue->GetMap();
}

// --- Internal Helper Function ---
void Serializable::CopyValue(DataType type, const SerializeValue& value)
{
    switch (type)
    {
    case DataType::Int64:
    case DataType::Int32:
    case DataType::Short:
    case DataType::Byte:
        m_pValue.reset(new IntSerializeValue(value.GetInt()));
        break;
    case DataType::UInt64:
    case DataType::UInt32:
    case DataType::UShort:
    case DataType::UByte:
        m_pValue.reset(new UintSerializeValue(value.GetUint()));
        break;
    case DataType::Double:
    case DataType::Float:
        m_pValue.reset(new FloatSerializeValue(value.GetFloat()));
        break;
    case DataType::Bool:
        m_pValue.reset(new BoolSerializeValue(value.GetBoolean()));
        break;
    case DataType::String:
        m_pValue.reset(new StringSerializeValue(value.GetString()));
        break;
    case DataType::Array:
        m_pValue.reset(new ArraySerializeValue(value.GetArray()));
        break;
    case DataType::Map:
        m_pValue.reset(new MapSerializeValue(value.GetMap()));
        break;
    case DataType::None: // Added handling for None
    case DataType::Num: // Num is not a valid type to copy
    default:
        m_type = DataType::None;
        m_pValue.reset(new SerializeValue());
        break;
    }
    m_type = type;
}

// --- Static Helper Function ---
string Serializable::GetTypeString(DataType type)
{
    switch (type)
    {
    case DataType::Int64: return "DataType::Int64";
    case DataType::Int32: return "DataType::Int32";
    case DataType::Short: return "DataType::Short";
    case DataType::Byte: return "DataType::Byte";
    case DataType::UInt64: return "DataType::UInt64";
    case DataType::UInt32: return "DataType::UInt32";
    case DataType::UShort: return "DataType::UShort";
    case DataType::UByte: return "DataType::UByte";
    case DataType::Float: return "DataType::Float";
    case DataType::Double: return "DataType::Double";
    case DataType::Bool: return "DataType::Bool";
    case DataType::String: return "DataType::String";
    case DataType::Array: return "DataType::Array";
    case DataType::Map: return "DataType::Map";
    case DataType::None: return "DataType::None";
    case DataType::Num: return "DataType::Num (Sentinel)";
    default: break;
    }
    return "Unknown DataType";
}