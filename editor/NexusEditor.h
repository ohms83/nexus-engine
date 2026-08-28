//
// Created by nutta on 7/18/2025.
//

#pragma once

#include <nexus/Nexus.h>

class NexusEditor final : public nxs::Application
{
public:
    ~NexusEditor() override;

protected:
    bool Init_Internal() override;
    void InitModel();
    void OnEvent(const SDL_Event& e) override;
    void Render(nxs::RenderSystem& renderSystem) override;
    void OnKeyDown(SDL_Keycode key) override;
    void OnKeyUp(SDL_Keycode key) override;
    void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize) override;
    void Update() override;

private:
    //! Editor camera.
    nxs::Ref<nxs::Camera> m_camera;
    nxs::Ref<nxs::SceneGraphWidget> m_sceneGraphWidget;
    nxs::Ref<nxs::PropertyWindow> m_propertyWindow;
    glm::vec3 m_cameraMoveDir{};
};