#pragma once

#include "../Mesh.h"

NXS_NAMESPACE
{
    class CubeMesh final : public Mesh
    {
    public:
        explicit CubeMesh(const Ref<RenderingInterface>& renderingInterface);
    };
}