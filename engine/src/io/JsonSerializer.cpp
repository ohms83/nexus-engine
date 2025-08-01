#include "nexus/io/JsonSerializer.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <sstream>

#include "core/Logger.h"

USING_NAMESPACE_NXS;
using namespace std;

DEFINE_LOG(JsonSerializer);

namespace
{
    Serializable FromJson(const rapidjson::Value& value)
    {
        if(value.IsNull()) {
            return Serializable(nullptr);
        }
        if(value.IsBool()) {
            return Serializable(value.GetBool());
        }
        if(value.IsInt() || value.IsInt64()) { // Prefer Int64
            return Serializable(value.GetInt64());
        }
        if(value.IsUint() || value.IsUint64()) { // Handle unsigned if needed, but JSON numbers are usually signed
            // Note: JSON numbers can't distinguish signed/unsigned beyond magnitude.
            // RapidJSON prefers signed for smaller numbers.
            // For large uints, this might get read as double if it exceeds int64_t max
            // or as uint64_t if it fits. We'll stick to int64_t and double for numbers.
            return Serializable(INT64_CAST(value.GetUint64())); // Still provide if GetUint64 is common
        }
        if(value.IsFloat() || value.IsDouble()) {
            return Serializable(value.GetDouble());
        }
        if (value.IsString()) {
            return Serializable(value.GetString());
        }
        if(value.IsArray()) {
            std::vector<Serializable> serializeArray;
            serializeArray.reserve(value.GetArray().Size());
            for( const auto& elem : value.GetArray() ) {
                serializeArray.emplace_back(FromJson(elem));
            }
            return serializeArray; // Use std::vector<Serializable> constructor
        }
        if(value.IsObject()) {
            std::map<std::string, Serializable, std::less<>> serializeMap;
            for(const auto& elem : value.GetObject()) {
                serializeMap.emplace(elem.name.GetString(), FromJson(elem.value));
            }
            return Serializable(serializeMap); // Use std::map<std::string, Serializable> constructor
        }
        LOG_WARNING(LogJsonSerializer, "Unknown JSON value type encountered in FromJson.");
        return Serializable(nullptr); // Fallback to Null
    }

    void WriteJson(const Serializable& source, rapidjson::Writer<rapidjson::StringBuffer>& writer)
    {
        switch (source.GetType()) // Changed from getType() to GetType()
        {
            case DataType::Int64: // Changed from Serializable::Type::INTEGER
                writer.Int64(source.GetInt());
                break;

            case DataType::UInt64: // Changed from Serializable::Type::UNSIGNED
                writer.Uint64(source.GetInt());
                break;

            case DataType::Double: // Changed from Serializable::Type::FLOAT
                writer.Double(source.GetDouble());
                break;

            case DataType::Bool: // Changed from Serializable::Type::BOOLEAN
                writer.Bool(source.GetBool());
                break;

            case DataType::String: // Changed from Serializable::Type::STRING
                writer.String(source.GetString().c_str());
                break;

            case DataType::Array: // Changed from Serializable::Type::ARRAY
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

            case DataType::Map: // Changed from Serializable::Type::MAP
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

            case DataType::None: // Explicitly handle None or other unexpected types
            case DataType::Num:  // Num is usually a count, not a type to serialize
            default:
                LOG_WARNING(LogJsonSerializer, std::format("Unknown or unhandled DataType ({})", NxsGetTypeString(source.GetType())));
                writer.Null();
                break;
        }
    }
}

std::ostream& JsonSerializer::Pack(const Serializable& source, std::ostream& outStream) const // Renamed to Pack
{
    const std::string jsonString = ToString(source);
    outStream << jsonString;
    return outStream;
}

Serializable JsonSerializer::Unpack(std::istream& inStream) const // Renamed to Unpack
{
    std::string contents;
    inStream.seekg(0, std::ios::end);
    const std::streampos length = inStream.tellg();
    inStream.seekg(0, std::ios::beg);
    contents.resize(length);
    inStream.read(&contents[0], length);
    return FromString(contents);
}

std::string JsonSerializer::ToString(const Serializable& source) const // Renamed to ToString
{
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);
    WriteJson(source, writer);
    return s.GetString();
}

Serializable JsonSerializer::FromString(const std::string& inString) const // Renamed to FromString
{
    rapidjson::Document document;
    document.Parse(inString.c_str());

    if(rapidjson::kParseErrorNone == document.GetParseError())
    {
        return FromJson(document);
    }

    LOG_ERROR(LogJsonSerializer, std::format("Invalid JSON -- {}", inString));
    return Serializable();
}
