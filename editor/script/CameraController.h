#pragma once

#include <nexus/Nexus.h>

NXS_NAMESPACE
{
    namespace editor
    {
        class CameraController : public Script
        {
        public:
            using Super = Script;

            virtual ~CameraController() = default;

            void Begin() override;
            void Update(float dt) override;

            float moveSpeed = 100.f;
            // Turn speed in degree per second.
            float turnSpeed = 30.f;
        };
    }
}