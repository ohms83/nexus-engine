//
//  Serialize.cpp
//
//  Created by nuttachai on 23/10/19.
//

#include "nexus/io/VariantData.h"
#include "nexus/core/LogDispatcher.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Serialize);

const VariantData VariantData::None;
const std::string VariantData::s_emptyStr;
const std::vector<VariantData> VariantData::s_emptyArray;
const std::map<std::string, VariantData, std::less<>> VariantData::s_emptyMap;

DataType VariantData::GetType() const
{
    const std::vector typeMap = {
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
        // std::vector<VariantData>
        DataType::Array,
        // std::map<std::string, VariantData, std::less<>>
        DataType::Map,
    };

    const auto index = m_value.index();
    // This shouldn't have happened, so it's better to use assertion to catch it during the development.
    NXS_ASSERT_MSG(index < typeMap.size(), std::format("Invalid index {}", index));
    return typeMap[index];
}