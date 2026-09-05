#pragma once

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
