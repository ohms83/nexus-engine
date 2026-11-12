#include <array>
#include <chrono>
#include <cmath>
#include <format>
#include <iostream>
#include <thread>

#include "nexus/Nexus.h"

namespace
{
    constexpr int kDepth = 6;
    constexpr int kNodesPerLevel = 3;
    constexpr int kIterations = 200000;
}

class ProfilerStressApp final : public nxs::Application
{
public:
    bool Init_Internal() override
    {
        auto root = std::make_shared<nxs::SceneNode3D>(std::make_shared<entt::registry>(), "ProfilerRoot");
        m_root = std::move(root);

        BuildHierarchy(m_root, 0);
        LOG_DEBUG(LogTemp, std::format("Num nodes={}", m_numNodes));
        m_scene = std::make_shared<nxs::Scene>("ProfilerScene");
        m_scene->AddChild(m_root);
        m_scene->SetRenderer(std::make_unique<nxs::ForwardSceneRenderer>(GetRenderSystem()));
        return true;
    }

    void Update() override
    {
        Application::Update();

        rmt_BeginCPUSample(MakeDirty, 0);
        
        m_root->SetPosition(glm::vec3(1));
        m_root->SetOrientation(glm::quat {1, 0, 0, 0});
        m_root->SetScale(glm::vec3(1));
        m_root->VisitDescendants([](nxs::SceneNode& node) {
            node.SetPosition(glm::vec3(1));
            node.SetOrientation(glm::quat {1, 0, 0, 0});
            node.SetScale(glm::vec3(1));
        });
        rmt_EndCPUSample();

        auto begin = std::chrono::high_resolution_clock::now();
        rmt_BeginCPUSample(StressTest, 0);
        for (auto node : m_leaveNodes)
        {
            (void)node->GetGlobalPosition();
            (void)node->GetGlobalOrientation();
            (void)node->GetGlobalScale();
        }
        rmt_EndCPUSample();

        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - begin).count();

        if (m_lastLogMs == 0 || elapsed > 0)
        {
            LOG_INFO(LogTemp, std::format("ProfilerStress: {} ms for {} nodes", elapsed, m_numNodes));
            m_lastLogMs = static_cast<uint64_t>(elapsed);
        }
    }

    void Render(nxs::RenderSystem& renderSystem) override
    {
        renderSystem.BeginDraw();
        renderSystem.EndDraw();
    }

private:
    void BuildHierarchy(const nxs::Ref<nxs::SceneNode>& root, int level)
    {
        ++m_numNodes;

        if (level >= kDepth)
        {
            m_leaveNodes.push_back(root);
            return;
        }

        for (int i = 0; i < kNodesPerLevel; ++i)
        {
            const auto child = root->EmplaceChild<nxs::SceneNode3D>(std::format("Node_{}_{}", level, i));
            child->SetPosition({float(i), float(level), float(i % 3)});
            child->SetScale({1.0f + (float(i) * 0.1f), 1.0f, 1.0f});
            child->SetOrientation(glm::quat(glm::radians(glm::vec3(0.0f, float(i * 7), 0.0f))));
            BuildHierarchy(child, level + 1);
        }
    }

    nxs::Array<nxs::Ref<nxs::SceneNode>> m_leaveNodes;
    nxs::Ref<nxs::SceneNode> m_root;
    nxs::Ref<nxs::Scene> m_scene;
    uint64_t m_lastLogMs = 0;
    uint32_t m_numNodes = 0;
};

int main()
{
    constexpr auto vsync = true;
    constexpr auto fullscreen = false;

    nxs::GraphicsConfig graphicsConfig {
        nxs::GraphicsAPI::OpenGL,
        1280, 960,
        vsync,
    };

    nxs::ApplicationConfig config {
        "Profiler Stress",
        graphicsConfig,
        fullscreen,
        true,
        true,
        true,
        0,
    };
    config.allowProfile = true;
    return nxs::RunApplication<ProfilerStressApp>(config);
}
