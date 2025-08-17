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

    void InitCube(nxs::Scene& scene, nxs::int32 row, nxs::int32 col);

private:
    //! Editor camera.
    nxs::Ref<nxs::Camera> m_camera;
    nxs::Ref<nxs::Mesh> m_planeMesh;
    nxs::Ref<nxs::Texture> m_texture;
    nxs::Ref<nxs::Shader> m_shader;
    nxs::Ref<nxs::Model> m_model;
    glm::vec3 m_cameraMoveDir{};
};