#include "nexus/graphics/Model.h"
#include "nexus/core/LogDispatcher.h"

#include <ranges>
#include "Model.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Material);

void Model::AddMaterial(Ref<Material> material)
{
    m_materials.push_back(material);
}

void Model::RemoveMaterial(Ref<Material> material)
{
    auto [begin, end] = std::ranges::remove(m_materials, material);
    m_materials.erase(begin, end);
}

Ref<Material> Model::GetMaterial(uint32 index) const
{
    if (index >= m_materials.size())
    {
        LOG_ERROR(LogMaterial, std::format("Invalid material index {}/{}", index, m_materials.size()));
        return nullptr;
    }
    return m_materials[index];
}
Ref<Resource> ModelLoader::Load(const std::string &path, uint32 id)
{
    return Ref<Resource>();
}
