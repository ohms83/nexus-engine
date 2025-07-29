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
    void OnEvent(const SDL_Event& e) override;
    void Render(nxs::RenderSystem& renderSystem) override;
    void OnKeyDown(SDL_Keycode key) override;
    void OnKeyUp(SDL_Keycode key) override;
    void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize) override;
    void Update() override;

    NODISCARD std::string GetBaseAssetPath() const override;

private:
    //! Editor camera.
    nxs::Ref<nxs::Camera> m_camera;
    nxs::Ref<nxs::Mesh> m_planeMesh;
    nxs::Ref<nxs::Texture> m_texture;
    nxs::Ptr<nxs::Shader> m_shader;
    glm::vec3 m_cameraMoveDir{};
};