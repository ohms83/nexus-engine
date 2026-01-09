#pragma once

#include "SceneRenderer.h"

NAMESPACE_NXS
{
    /**
     * @brief A dummy implementation of SceneRenderer that mainly used for testing new features.
     */
    class DummySceneRenderer final : public SceneRenderer
    {
    public:
        DummySceneRenderer() = default;
        ~DummySceneRenderer() override = default;

        void RenderScene(Ref<Scene> scene, Ref<Camera> camera) override;
    };
}