#pragma once

#include "nexus/NxsDefine.h"

DECLARE_LOG_EXTERN(Runnable);

NXS_NAMESPACE
{
    using Action = std::function<void()>;
}