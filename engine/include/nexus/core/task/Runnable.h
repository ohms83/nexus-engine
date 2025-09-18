//
// Created by nutta on 9/5/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "sigslot/signal.hpp"

DECLARE_LOG_EXTERN(Runnable);

NXS_NAMESPACE
{
    class IRunnable
    {
    public:
        virtual ~IRunnable() = default;
        MAYBE_UNUSED virtual bool Update() = 0;
    };
    
    using TaskFunc = std::function<bool()>;
    using TaskCallback = sigslot::signal<Ref<IRunnable>>;
}