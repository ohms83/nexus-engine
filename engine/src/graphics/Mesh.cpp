#include "graphics/Mesh.h"

#include "graphics/mesh/CubeMesh.h"
#include "graphics/mesh/PlaneMesh.h"

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
