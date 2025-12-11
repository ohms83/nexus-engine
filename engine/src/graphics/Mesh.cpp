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

void Mesh::ComputeBounds()
{
    if (!m_vertexBuffer) return;

    const auto vertexCount = m_vertexBuffer->VertexCount();
    if (vertexCount == 0) return;

    glm::vec3 minPoint(FLT_MAX);
    glm::vec3 maxPoint(-FLT_MAX);

    auto vertexData = m_vertexBuffer->GetData();

    const auto* positionAttr = m_vertexBuffer->FindAttribute(VertexAttribute::Type::Position);
    if (!positionAttr) return; // No position attribute found

    const auto offset = m_vertexBuffer->GetAttributeOffset(VertexAttribute::Type::Position);
    const size_t vertexSize = m_vertexBuffer->GetStride();
    for (size_t i = 0; i < vertexCount; ++i)
    {
        const uint8* vertexPtr = CAST<const uint8_t*>(vertexData) + i * vertexSize;
        const float* positionPtr = R_CAST<const float*>(vertexPtr + offset);
        glm::vec3 position(positionPtr[0], positionPtr[1], positionPtr[2]);

        minPoint = glm::min(minPoint, position);
        maxPoint = glm::max(maxPoint, position);
    }

    m_boundingBox.center = (minPoint + maxPoint) * 0.5f;
    m_boundingBox.extent = (maxPoint - minPoint) * 0.5f;

    // Compute bounding sphere
    m_boundingSphere.center = m_boundingBox.center;
    // float maxRadiusSq = 0.0f;
    // for (size_t i = 0; i < vertexCount; ++i)
    // {
    //     const uint8* vertexPtr = CAST<const uint8_t*>(vertexData) + i * vertexSize;
    //     const float* positionPtr = R_CAST<const float*>(vertexPtr + offset);
    //     glm::vec3 position(positionPtr[0], positionPtr[1], positionPtr[2]);

    //     float distSq = glm::length2(position - m_boundingSphere.center);
    //     maxRadiusSq = glm::max(maxRadiusSq, distSq);
    // }
    m_boundingSphere.radius = glm::length(m_boundingBox.extent);
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