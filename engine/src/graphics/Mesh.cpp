#include "graphics/Mesh.h"

#include "graphics/mesh/CubeMesh.h"
#include "graphics/mesh/PlaneMesh.h"
#include "nexus/graphics/MaterialManager.h"
#include "nexus/graphics/TextureManager.h"
#include "nexus/io/Serializer.h"

USING_NAMESPACE_NXS;

Mesh::Mesh()
{
    static uint64 count = 0;
    m_name = std::format("Mesh_{}", count++);
}

Mesh::Mesh(std::string name)
    : m_name(std::move(name))
{
}

VariantData Mesh::Serialize() const
{
    VariantData::Map data;
    data["name"] = m_name;
    data["material"] = m_material ? m_material->GetPath() : m_materialPath;
    data["sphere"] = VariantData::Map{
        {"center", VariantData::Array{DOUBLE_CAST(m_boundingSphere.center.x), DOUBLE_CAST(m_boundingSphere.center.y), DOUBLE_CAST(m_boundingSphere.center.z)}},
        {"radius", DOUBLE_CAST(m_boundingSphere.radius)}
    };
    data["box"] = VariantData::Map{
        {"center", VariantData::Array{DOUBLE_CAST(m_boundingBox.center.x), DOUBLE_CAST(m_boundingBox.center.y), DOUBLE_CAST(m_boundingBox.center.z)}},
        {"extent", VariantData::Array{DOUBLE_CAST(m_boundingBox.extent.x), DOUBLE_CAST(m_boundingBox.extent.y), DOUBLE_CAST(m_boundingBox.extent.z)}}
    };
    return data;
}

void Mesh::Deserialize(const VariantData& data)
{
    m_name = data["name"].GetString();
    // Material path is stored for later resolution by a ResourceManager.
    m_materialPath = data["material"].GetString();
    m_material = nullptr;

    if (data.HasKey("sphere") && data["sphere"].IsMap())
    {
        const auto& sm = data["sphere"].GetMap();
        const auto& arr = sm.at("center").GetArray();
        m_boundingSphere.center.x = FLOAT_CAST(arr.at(0).GetDouble());
        m_boundingSphere.center.y = FLOAT_CAST(arr.at(1).GetDouble());
        m_boundingSphere.center.z = FLOAT_CAST(arr.at(2).GetDouble());
        m_boundingSphere.radius = FLOAT_CAST(sm.at("radius").GetDouble());
    }
    if (data.HasKey("box") && data["box"].IsMap())
    {
        const auto& bm = data["box"].GetMap();
        const auto& carr = bm.at("center").GetArray();
        m_boundingBox.center.x = FLOAT_CAST(carr.at(0).GetDouble());
        m_boundingBox.center.y = FLOAT_CAST(carr.at(1).GetDouble());
        m_boundingBox.center.z = FLOAT_CAST(carr.at(2).GetDouble());
        const auto& earr = bm.at("extent").GetArray();
        m_boundingBox.extent.x = FLOAT_CAST(earr.at(0).GetDouble());
        m_boundingBox.extent.y = FLOAT_CAST(earr.at(1).GetDouble());
        m_boundingBox.extent.z = FLOAT_CAST(earr.at(2).GetDouble());
    }
}

void Mesh::Resolve(MaterialManager& materialManager, TextureManager& textureManager)
{
    if (m_material) return; // already resolved
    if (m_materialPath.empty()) return;
    auto mat = materialManager.Get(m_materialPath);
    if (!mat) return;
    m_material = mat;
    // Resolve the material's textures
    mat->Resolve(textureManager, nullptr);
}