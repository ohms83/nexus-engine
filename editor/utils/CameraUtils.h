#pragma once

#include "nexus/Nexus.h"

NXS_NAMESPACE
{
    namespace editor
    {
        class CameraUtils
        {
        public:
            static Ref<nxs::Camera> InitCamera(Scene& scene, const std::string& name = "Editor Camera");
        };
    }
}