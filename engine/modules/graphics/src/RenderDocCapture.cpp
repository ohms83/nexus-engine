// RenderDoc capture helper: dynamically load RenderDoc and allow "capture next frame" functionality
#include "nexus/graphics/RenderDocCapture.h"
#include "nexus/debug/LogDispatcher.h"

#include <Windows.h>

USING_NAMESPACE_NXS;

DEFINE_LOG(RenderDocCapture);

namespace
{
    typedef int(*pRENDERDOC_GetAPI_t)(int, void*);
    void* g_rdoc_api = nullptr;
    HMODULE g_rdoc_lib = nullptr;
}

namespace NXS_NAMESPACE::RenderDocCapture
{
    static std::atomic<bool> s_initialized{false};
    static std::atomic<bool> s_captureNextFrame{false};
    static std::atomic<bool> s_captureActive{false};

    void Initialize()
    {
        if (s_initialized.load()) return;

        g_rdoc_lib = LoadLibraryA("renderdoc.dll");
        if (!g_rdoc_lib)
        {
            LOG_WARNING(LogRenderDocCapture, "renderdoc.dll not found; RenderDoc capture disabled.");
            s_initialized.store(false);
            return;
        }

        auto getApi = reinterpret_cast<pRENDERDOC_GetAPI_t>(GetProcAddress(g_rdoc_lib, "RENDERDOC_GetAPI"));
        if (!getApi)
        {
            LOG_WARNING(LogRenderDocCapture, "RENDERDOC_GetAPI not found in renderdoc.dll");
            FreeLibrary(g_rdoc_lib);
            g_rdoc_lib = nullptr;
            s_initialized.store(false);
            return;
        }

        // Request the RENDERDOC API version 10400 (RENDERDOC_API_Version_1_4_0) as integer
        const int RENDERDOC_API_1_1 = 10400; // safe default; RenderDoc exposes compatibility
        if (getApi(RENDERDOC_API_1_1, &g_rdoc_api) == 0)
        {
            LOG_WARNING(LogRenderDocCapture, "Failed to acquire RenderDoc API");
            FreeLibrary(g_rdoc_lib);
            g_rdoc_lib = nullptr;
            g_rdoc_api = nullptr;
            s_initialized.store(false);
            return;
        }

        s_initialized.store(true);
        LOG_INFO(LogRenderDocCapture, "RenderDoc API initialized.");
    }

    void TriggerCaptureNextFrame()
    {
        if (!s_initialized.load()) Initialize();
        if (!s_initialized.load()) return;
        s_captureNextFrame.store(true);
        LOG_INFO(LogRenderDocCapture, "RenderDoc: capture requested for next frame.");
    }

    void BeginCaptureIfRequested()
    {
        if (!s_initialized.load()) return;
        if (!s_captureNextFrame.exchange(false)) return;

        // Call StartFrameCapture via API - function pointer in g_rdoc_api
        using StartFn = int(*)(void*, void*);
        auto start = reinterpret_cast<StartFn>(reinterpret_cast<char*>(g_rdoc_api) + 0); // we will lookup via exported function pointer via dlsym equivalent not available; use documented API struct

        // Instead of offset tricks, use known symbol names via GetProcAddress on DLL
        auto startFn = reinterpret_cast<StartFn>(GetProcAddress(g_rdoc_lib, "RENDERDOC_StartFrameCapture"));
        auto endFn = reinterpret_cast<int(*)(void*, void*)>(GetProcAddress(g_rdoc_lib, "RENDERDOC_EndFrameCapture"));
        if (!startFn || !endFn)
        {
            LOG_WARNING(LogRenderDocCapture, "RenderDoc start/end functions not found in DLL; trying API struct");
            // Try API struct function pointers (common name in API struct)
            // We attempt to cast g_rdoc_api to a known struct with function pointers.
            struct RENDERDOC_API_1_1
            {
                void* SetLogFilePathTemplate;
                int(*StartFrameCapture)(void*, void*);
                int(*EndFrameCapture)(void*, void*);
                // We don't need other functions here
            };

            auto api = reinterpret_cast<RENDERDOC_API_1_1*>(g_rdoc_api);
            if (api && api->StartFrameCapture && api->EndFrameCapture)
            {
                api->StartFrameCapture(NULL, NULL);
                s_captureActive.store(true);
                LOG_INFO(LogRenderDocCapture, "RenderDoc: started frame capture (API struct).");
                return;
            }

            LOG_WARNING(LogRenderDocCapture, "Unable to start frame capture - RenderDoc API unavailable.");
            return;
        }

        startFn(NULL, NULL);
        s_captureActive.store(true);
        LOG_INFO(LogRenderDocCapture, "RenderDoc: started frame capture (DLL functions).");
    }

    void EndCaptureIfActive()
    {
        if (!s_initialized.load()) return;
        if (!s_captureActive.exchange(false)) return;

        auto endFn = reinterpret_cast<int(*)(void*, void*)>(GetProcAddress(g_rdoc_lib, "RENDERDOC_EndFrameCapture"));
        if (endFn)
        {
            endFn(NULL, NULL);
            LOG_INFO(LogRenderDocCapture, "RenderDoc: ended frame capture (DLL function).");
            return;
        }

        // Fallback to API struct
        struct RENDERDOC_API_1_1
        {
            void* SetLogFilePathTemplate;
            int(*StartFrameCapture)(void*, void*);
            int(*EndFrameCapture)(void*, void*);
        };

        auto api = reinterpret_cast<RENDERDOC_API_1_1*>(g_rdoc_api);
        if (api && api->EndFrameCapture)
        {
            api->EndFrameCapture(NULL, NULL);
            LOG_INFO(LogRenderDocCapture, "RenderDoc: ended frame capture (API struct).");
            return;
        }

        LOG_WARNING(LogRenderDocCapture, "Unable to end frame capture - RenderDoc API unavailable.");
    }

    bool IsInitialized()
    {
        return s_initialized.load();
    }
}
