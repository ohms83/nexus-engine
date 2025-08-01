//
//  Serialize.cpp
//
//  Created by nuttachai on 23/10/19.
//

#include "nexus/io/Serialize.h"
#include "nexus/core/Logger.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Serialize);

const Serializable Serializable::None;
const std::string Serializable::s_emptyStr;
const std::vector<Serializable> Serializable::s_emptyArray;
const std::map<std::string, Serializable, std::less<>> Serializable::s_emptyMap;

DataType Serializable::GetType() const
{
    std::vector<DataType> typeMap = {
        // std::monostate
        DataType::None,
        // bool
        DataType::Bool,
        // int64_t
        DataType::Int64,
        // double
        DataType::Double,
        // std::string
        DataType::String,
        // std::vector<Serializable>
        DataType::Array,
        // std::map<std::string, Serializable, std::less<>>
        DataType::Map,
    };

    const auto index = m_value.index();
    // This shouldn't have happened so it's better to use assert to catch it during the development.
    NXS_ASSERT_MSG(index < typeMap.size(), std::format("Invalid index {}", index));
    return typeMap[index];
}