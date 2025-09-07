//
// Created by nutta on 9/5/2025.
//

#pragma once

#include "nexus/NxsDefine.h"

DECLARE_LOG_EXTERN(Runnable);

NXS_NAMESPACE
{
    class IRunnable
    {
    public:
        virtual ~IRunnable() = default;
        virtual bool Update() = 0;
    };
}