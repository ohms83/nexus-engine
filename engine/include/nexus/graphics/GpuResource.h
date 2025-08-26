//
// Created by nutta on 7/7/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

NXS_NAMESPACE
{
    class IGpuResource
    {
    public:
        IGpuResource() = default;
        virtual ~IGpuResource() = default;

        virtual uint32 GetHandle() const = 0;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual bool IsBinding() const = 0;

        virtual void CopyData(const void* data, size_t bytes, size_t offset = 0) = 0;

    protected:
        /**
         * Allocating the resource object on GPU. The child classes
         * must provide the implementation of this function.
         * @return The generated resource handle; otherwise 0, if failed.
         */
        NODISCARD virtual uint32 Alloc() = 0;

        //! Release the allocated GPU resource.
        virtual void Release() = 0;
    };
}
