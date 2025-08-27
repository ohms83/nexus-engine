//
// Created by nutta on 6/28/2025.
//

#pragma once

#include "nexus/NxsDefine.h"

NXS_NAMESPACE
{
    class IBuffer
    {
    public:
        IBuffer() = default;
        virtual ~IBuffer() = default;

        NODISCARD virtual bool IsValid() const = 0;
        NODISCARD virtual const uint8* Data() const = 0;
        NODISCARD virtual uint64 Size() const = 0;
        virtual void Release() = 0;
    };
}
