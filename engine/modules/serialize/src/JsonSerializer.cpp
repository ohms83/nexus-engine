#include "nexus/serialize/JsonSerializer.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <sstream>

#include "nexus/debug/LogDispatcher.h"

USING_NAMESPACE_NXS;
using namespace std;

DEFINE_LOG(JsonSerializer);

namespace
{
    VariantData FromJson(const rapidjson::Value& value)
    {
        if(value.IsNull()) {
            return nullptr;
        }
        if(value.IsBool()) {
            return value.GetBool();
        }
        if(value.IsInt() || value.IsInt64()) { // Prefer Int64
            return value.GetInt64();
        }
        if(value.IsUint() || value.IsUint64()) { // Handle unsigned if needed, but JSON numbers are usually signed
            // Note: JSON numbers can't distinguish signed/unsigned beyond magnitude.
            // RapidJSON prefers signed for smaller numbers.
            // For large uints, this might get read as double if it exceeds int64_t max
            // or as uint64_t if it fits. We'll stick to int64_t and double for numbers.
            return INT64_CAST(value.GetUint64()); // Still provide if GetUint64 is common
        }
        if(value.IsFloat() || value.IsDouble()) {
            return value.GetDouble();
        }
        if (value.IsString()) {
            return value.GetString();
        }
        if(value.IsArray()) {
            VariantData::Array serializeArray;
            serializeArray.reserve(value.GetArray().Size());
            for( const auto& elem : value.GetArray() ) {
                serializeArray.emplace_back(FromJson(elem));
            }
            return serializeArray; // Use std::vector<VariantData> constructor
        }
        if(value.IsObject()) {
            VariantData::Map serializeMap;
            for(const auto& elem : value.GetObject()) {
                serializeMap.emplace(elem.name.GetString(), FromJson(elem.value));
            }
            return serializeMap; // Use std::map<std::string, VariantData> constructor
        }
        LOG_WARNING(LogJsonSerializer, "Unknown JSON value type encountered in FromJson.");
        return VariantData::None; // Fallback to None
    }

    void WriteJson(const VariantData& source, rapidjson::Writer<rapidjson::StringBuffer>& writer)
    {
        switch (source.GetType()) // Changed from getType() to GetType()
        {
        case DataType::None: // Explicitly handle None
            writer.Null();
            break;
        case DataType::Int64: // Changed from VariantData::Type::INTEGER
            writer.Int64(source.GetInt());
            break;
        case DataType::UInt64: // Changed from VariantData::Type::UNSIGNED
            writer.Uint64(source.GetInt());
            break;
        case DataType::Double: // Changed from VariantData::Type::FLOAT
            writer.Double(source.GetDouble());
            break;
        case DataType::Bool: // Changed from VariantData::Type::BOOLEAN
            writer.Bool(source.GetBool());
            break;
        case DataType::String: // Changed from VariantData::Type::STRING
            writer.String(source.GetString().c_str());
            break;
        case DataType::Array: // Changed from VariantData::Type::ARRAY
        {
            const auto& array = source.GetArray();
            writer.StartArray();
            for( const auto& elem : array )
            {
                WriteJson(elem, writer); // Recursive call to WriteJson
            }
            writer.EndArray();
        }
            break;
        case DataType::Map: // Changed from VariantData::Type::MAP
        {
            const auto& map = source.GetMap();
            writer.StartObject();
            for( const auto& [fst, snd] : map )
            {
                writer.Key(fst.c_str());
                WriteJson(snd, writer); // Recursive call to WriteJson
            }
            writer.EndObject();
        }
            break;
        case DataType::Byte:
        case DataType::Short:
        case DataType::Int32:
            writer.Int64(source.GetInt()); // Or the appropriate smaller integer writer if available/needed
            break;
        case DataType::UByte:
        case DataType::UShort:
        case DataType::UInt32:
            writer.Uint64(source.GetInt()); // Or appropriate smaller unsigned integer writer
            break;
        case DataType::Float: // This would typically map to Double for JSON output
            writer.Double(source.GetDouble());
            break;
        case DataType::Num:  // Num is usually a count, not a type to serialize
        default:
            LOG_WARNING(LogJsonSerializer, std::format("Unknown or unhandled DataType ({})", NxsGetTypeString(source.GetType())));
            writer.Null();
            break;
        }
    }
}

std::ostream& JsonSerializer::Pack(const VariantData& source, std::ostream& outStream) const
{
    const std::string jsonString = ToString(source);
    outStream << jsonString;
    return outStream;
}

VariantData JsonSerializer::Unpack(std::istream& inStream) const
{
    std::string contents;
    inStream.seekg(0, std::ios::end);
    const std::streampos length = inStream.tellg();
    inStream.seekg(0, std::ios::beg);
    contents.resize(length);
    inStream.read(&contents[0], length);
    return FromString(contents);
}

std::string JsonSerializer::ToString(const VariantData& source) const
{
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);
    WriteJson(source, writer);
    return s.GetString();
}

VariantData JsonSerializer::FromString(const std::string& inString) const
{
    rapidjson::Document document;
    document.Parse(inString.c_str());

    if(rapidjson::kParseErrorNone == document.GetParseError())
    {
        return FromJson(document);
    }

    LOG_ERROR(LogJsonSerializer, std::format("Invalid JSON -- {}", inString));
    return VariantData::None;
}
