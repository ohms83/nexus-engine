#pragma once

#include "../Mesh.h"

NXS_NAMESPACE
{
    class Cube : public Mesh
    {
    public:
        Cube(uint32 hash);
        virtual ~Cube() = default;

        bool Load(const std::string& path) override
        {
            assert(false);
            return true;
        }

    protected:
        uint8* Load_Impl(const std::string& path, size_t& out_size) override
        {
            assert(false);
            return nullptr;
        }
    };
}