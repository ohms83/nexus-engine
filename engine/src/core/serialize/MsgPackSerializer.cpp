//
// Created by nutta on 8/1/2025.
//
#include "core/serialize/MsgPackSerializer.h"
#include <msgpack.hpp>

USING_NAMESPACE_NXS;

DEFINE_LOG(MsgPackSerializer);

namespace msgpack { // NOLINT(*-concat-nested-namespaces)
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {// Place class template specialization here
    template<>
    struct convert<nxs::VariantData> {
        msgpack::object const& operator()(msgpack::object const& o, nxs::VariantData& data) const
        {
            switch(o.type)
            {
            case msgpack::type::NIL:
                data = VariantData::None;
                break;
            case msgpack::type::BOOLEAN:
                data = VariantData(o.as<bool>());
                break;
            case msgpack::type::POSITIVE_INTEGER:
            case msgpack::type::NEGATIVE_INTEGER:
                data = VariantData(o.as<int64_t>());
                break;
            case msgpack::type::FLOAT32:
                data = VariantData(o.as<float>());
                break;
            case msgpack::type::FLOAT64:
                data = VariantData(o.as<double>());
                break;
            case msgpack::type::STR:
                data = VariantData(o.as<std::string>());
                break;
            case msgpack::type::ARRAY:
                data = VariantData(o.as<VariantData::Array>());
                break;
            case msgpack::type::MAP:
                data = VariantData(o.as<VariantData::Map>());
                break;
            default:
                NXS_ASSERT_MSG(false, std::format("Unknown type: {}", INT_CAST(o.type)));
                break;
            }
            return o;
        }
    };

    template<>
    struct pack<nxs::VariantData>
    {
        template <typename Stream>
        packer<Stream>& operator()(msgpack::packer<Stream>& o, nxs::VariantData const& data) const
        {
            switch (data.GetType())
            {
            case DataType::None:
                o.pack_nil();
                break;
            case DataType::Bool:
                if (data.GetBool()) o.pack_true();
                else o.pack_false();
                break;
            case DataType::Byte:
                o.pack_signed_char(data.GetInt());
                break;
            case DataType::Short:
                o.pack_int16(data.GetInt());
                break;
            case DataType::Int32:
                o.pack_int32(data.GetInt());
                break;
            case DataType::Int64:
                o.pack_int64(data.GetInt());
                break;
            case DataType::UByte:
                o.pack_unsigned_char(data.GetInt());
                break;
            case DataType::UShort:
                o.pack_uint16(data.GetInt());
                break;
            case DataType::UInt32:
                o.pack_uint32(data.GetInt());
                break;
            case DataType::UInt64:
                o.pack_uint64(data.GetInt());
                break;
            case DataType::Float:
                o.pack_float(data.GetDouble());
                break;
            case DataType::Double:
                o.pack_double(data.GetDouble());
                break;
            case DataType::String:
                o.pack(data.GetString());
                break;
            case DataType::Array:
                o.pack(data.GetArray());
                break;
            case DataType::Map:
                o.pack(data.GetMap());
                break;
            default:
                NXS_ASSERT_MSG(false, std::format("Unknown type: {}", NxsGetTypeString(data.GetType())));
                break;
            }
            return o;
        }
    };
} // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE
} // namespace msgpack

std::ostream& MsgPackSerializer::Pack(const VariantData& source, std::ostream& outStream) const
{
    msgpack::pack(outStream, source);
    return outStream;
}

VariantData MsgPackSerializer::Unpack(std::istream& inStream) const
{
    // copies all data into buffer
    const std::vector buffer(std::istreambuf_iterator<char>(inStream), {});
    const char* data = buffer.data();
    const size_t size = buffer.size();
    return msgpack::unpack(data, size).get().as<VariantData>();
}
