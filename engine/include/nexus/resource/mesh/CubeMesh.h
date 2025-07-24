#pragma once

#include "../Mesh.h"

NXS_NAMESPACE
{
    class CubeMesh final : public Mesh
    {
    public:
        explicit CubeMesh(uint32 hash);
        ~CubeMesh() override = default;

        bool Load(const std::string& path) override
        {
            NXS_ASSERT(false);
            return true;
        }

    protected:
        uint8* Load_Impl(const std::string& path, size_t& out_size) override
        {
            NXS_ASSERT(false);
            return nullptr;
        }
    };
}