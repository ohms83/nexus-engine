//
// Created by nutta on 7/7/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

NXS_NAMESPACE
{
    class GpuResrouce
    {
    public:
        GpuResrouce() = default;
        virtual ~GpuResrouce() = default;

        [[nodiscard]] uint32 GetHandle() const
        {
            return m_handle;
        }

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

    protected:
        /**
         * Allocating the resource object on GPU. The child classes
         * must provide the implementation of this function.
         * @return The generated resource handle; otherwise 0, if failed.
         */
        virtual [[nodiscard]] uint32 Alloc() = 0;

    protected:
        uint32 m_handle = 0;
    };
}
