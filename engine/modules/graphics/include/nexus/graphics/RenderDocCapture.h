#pragma once

#include "nexus/NxsDefine.h"
#include <atomic>

NXS_NAMESPACE
{
    namespace RenderDocCapture
    {
        void Initialize();
        void TriggerCaptureNextFrame();
        void BeginCaptureIfRequested();
        void EndCaptureIfActive();
        bool IsInitialized();
    }
}
