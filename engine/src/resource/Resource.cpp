//
// Created by nutta on 7/9/2025.
//

#include <nexus/resource/Resource.h>

USING_NAMESPACE_NXS;

DEFINE_LOG(Resource);

bool Resource::Load(const std::string& path)
{
    m_filepath = path;
    size_t size = 0;
    const auto data = Load_Impl(path, size);
    m_data.Take(data, size);
    return data != nullptr;
}