//
// Created by nutta on 7/4/2025.
//

#pragma once

#include <cassert>
#include <nexus/NxsDefine.h>

#include "GraphicsConfig.h"

NXS_NAMESPACE
{
    class RenderingInterface
    {
    public:
        RenderingInterface(const RenderingInterface&) = delete;
        RenderingInterface(RenderingInterface&&) = delete;

        static RenderingInterface* Create(WindowContext* window, const GraphicsConfig& config);
        static void Destroy();

        static RenderingInterface& Get()
        {
            assert(m_singleton);
            // ReSharper disable once CppDFANullDereference
            return *m_singleton;
        }

    protected:
        RenderingInterface() = default;
        virtual ~RenderingInterface() = default;

    private:
        static RenderingInterface* m_singleton;
    };
}
