//
// Created by nutta on 6/23/2025.
//
#include "nexus/Nexus.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/container/Array.h"
#include "imgui.h"
#include <limits>

static const nxs::Array<std::string> modelPaths = {
    "meshes/apple/3DApple001_SQ-1K-PNG.obj",
    "meshes/armadillo/armadillo.obj",
    "meshes/bunny/stanford-bunny.obj",
    "meshes/cube/cube_textured.obj",
    "meshes/barrel/wine_barrel_01_4k.gltf",
};
static const nxs::Array<std::string> modelLabels = {
    "Apple",
    "Armadillo",
    "Bunny",
    "Crate",
    "Wine Barrel",
};
static const nxs::Array<glm::vec3> modelScales = {
    glm::vec3(3),
    glm::vec3(0.01),
    glm::vec3(3),
    glm::vec3(1),
    glm::vec3(1),
};

class Example_06 final : public nxs::Application
{
public:
    ~Example_06() override = default;

    void Update() override
    {
        const auto selectedNode = m_sceneGraphWidget->GetSelectedNode();
        m_propertyWindow->SetSceneNode(selectedNode);
        UpdateModel();
    }

    void Render(nxs::RenderSystem& renderSystem) override
    {
        if (!m_scene) return;
        m_scene->Render(renderSystem);
    }

    void DrawUI() override
    {
        ImGui::Begin("Shadow Settings");
        {
            ImGui::SeparatorText("Model");
            if (ImGui::BeginCombo("Select Model", modelLabels[m_selectedModel].c_str()))
            {
                for (size_t n = 0; n < modelLabels.size(); n++)
                {
                    const bool isSelected = (m_selectedModel == n);
                    if (ImGui::Selectable(modelLabels[n].c_str(), isSelected) && !isSelected)
                    {
                        m_selectedModel = n;
                        m_displayedModel = std::numeric_limits<size_t>::max();
                        m_modelNode->RemoveAllChildren(true);
                        m_modelError.clear();
                    }

                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            const auto& load = m_loadedModels[m_selectedModel];
            if (!load || load->status.load() == nxs::IResourceLoader::LoadResult::Status::Loading)
                ImGui::TextUnformatted("Loading model...");
            else if (load->status.load() == nxs::IResourceLoader::LoadResult::Status::Failed ||
                     load->status.load() == nxs::IResourceLoader::LoadResult::Status::Invalid)
                ImGui::TextWrapped("Model load failed: %s", load->error.c_str());
            else if (!m_modelError.empty())
                ImGui::TextWrapped("%s", m_modelError.c_str());

            ImGui::SliderFloat("Scale", &m_scale, 0.25f, 3.0f);
            ImGui::SliderFloat("Height above ground", &m_height, 0.0f, 3.0f);
            ImGui::Checkbox("Draw Sphere", &m_drawSphere);
            ImGui::Checkbox("Draw Box", &m_drawBox);
            ImGui::TextUnformatted("Rotate model with arrow keys.");
            ImGui::SeparatorText("Directional shadows");
            ImGui::Checkbox("Light casts shadows", &m_light->Properties().canCastShadows);
            ImGui::Checkbox("Model casts shadows", &m_castShadow);
            ImGui::Checkbox("Model receives shadows", &m_receiveShadows);
            ImGui::Checkbox("Ground receives shadows", &m_groundMaterial->receiveShadows);
            glm::vec3 direction = m_light->Direction();
            if (ImGui::SliderFloat3("Light direction", &direction.x, -1.0f, 1.0f) &&
                glm::dot(direction, direction) > 0.0001f)
                m_light->Direction() = direction;
            ImGui::TextWrapped("Transparent materials cast solid silhouettes.");
        }
        ImGui::End();

        auto renderSystem = nxs::Engine::Instance().GetRenderSystem();
        m_propertyWindow->Draw(*renderSystem);
        m_sceneGraphWidget->Draw(*renderSystem);
    }

protected:
    bool Init_Internal() override
    {
        const auto engine = nxs::Engine::Instance();
        m_modelLoader = std::make_unique<nxs::ModelLoader>(
            engine.GetResourceManager()
        );

        auto sceneManager = engine.GetSceneManager();
        m_scene = sceneManager->EmplaceAndChange<nxs::Scene>("Main Scene");
        sceneManager->sceneChangedCallback.connect([this](nxs::Ref<nxs::Scene> prev, nxs::Ref<nxs::Scene> next) {
            m_scene = next;
        });

        InitScene();
        InitLights();

        m_sceneGraphWidget = std::make_shared<nxs::SceneGraphWidget>(*sceneManager);
        m_propertyWindow = std::make_shared<nxs::PropertyWindow>(*sceneManager);

        const nxs::KeyInputMap modelRotationInput = {
            {
                {SDLK_LEFT, nxs::KeyInputMap::AxisPlusY},
                {SDLK_RIGHT, nxs::KeyInputMap::AxisMinusY},
                {SDLK_UP, nxs::KeyInputMap::AxisPlusX},
                {SDLK_DOWN, nxs::KeyInputMap::AxisMinusX},
            }
        };

        auto& inputManager = nxs::InputManager::Instance();
        inputManager.RegisterAxisInputMap("model_rotate", modelRotationInput);
        return true;
    }

    void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize) override
    {
        Application::OnResize(screenSize, actualSize);
        m_camera.width = FLOAT_CAST(screenSize.x);
        m_camera.height = FLOAT_CAST(screenSize.y);

        auto camera = m_scene ? PTR_CAST<nxs::Camera>(m_scene->FindNodeWithName("Camera")) : nullptr;
        if (camera && screenSize.x > 0 && screenSize.y > 0) {
            camera->SetProjection(m_camera.fov, m_camera.width, m_camera.height, m_camera.nearZ, m_camera.farZ);
        }
    }

private:
    void InitScene()
    {
        auto camera = m_scene->EmplaceChild<nxs::Camera>("Camera");
        camera->SetPosition({5, 5, 5});
        camera->LookAt(glm::vec3(0), glm::vec3(0, 1, 0));
        camera->SetProjection(m_camera.fov, m_camera.width, m_camera.height, m_camera.nearZ, m_camera.farZ);
        camera->Properties().clearColor = nxs::Color3F::Grey;

        m_loadedModels.resize(modelPaths.size());
        m_loadedModels[0] = LoadModel(0);

        m_modelNode = m_scene->EmplaceChild<nxs::SceneNode>("Model");

        m_groundMaterial = std::make_shared<nxs::Material>("Ground Material", 0);
        m_groundMaterial->castShadow = false;
        m_groundMaterial->CreateDefaultShader(*nxs::Engine::Instance().GetResourceManager());

        auto meshComp = m_scene->EmplaceChild<nxs::SceneNode>("Ground")->AddComponent<nxs::MeshComponent>();
        meshComp->SetMesh(nxs::PrimitiveMesh::CreatePlane(
                "Ground Plane",
                10, 10,
                *nxs::GetGpuResourceFactory(),
                m_groundMaterial
            )
        );

        m_scene->SetRenderer(std::make_unique<nxs::ForwardSceneRenderer>(GetRenderSystem()));
    }

    nxs::Ref<nxs::IResourceLoader::LoadResult> LoadModel(const size_t index)
    {
        const auto taskScheduler = nxs::Engine::Instance().GetTaskScheduler();
        const auto assetPath = GetAssetPath(modelPaths[index]);
        return m_modelLoader->LoadAsync(assetPath, UINT_CAST(index), *taskScheduler, [](nxs::Ref<nxs::Resource>) {});
    }

    void InitLights()
    {
        m_scene->Ambient() = {0.2f, 0.2f, 0.2f};
        m_light = m_scene->EmplaceChild<nxs::DirectionalLight>("Direct Light");
        m_light->Properties().color = {1, 1, 1};
        m_light->Direction() = {0.7f, -1.0f, 0.3f};
    }

    void UpdateModel()
    {
        const auto& inputManager = nxs::InputManager::Instance();
        const glm::vec2 euler = inputManager.GetAxisValue("model_rotate") * 90.f * GetDeltaTime();
        m_euler.x += euler.x;
        m_euler.y += euler.y;

        auto& load = m_loadedModels[m_selectedModel];
        if (!load)
        {
            for (const auto& pending : m_loadedModels)
            {
                if (pending && pending->status.load() == nxs::IResourceLoader::LoadResult::Status::Loading)
                    return;
            }
            load = LoadModel(m_selectedModel);
        }
        if (m_displayedModel != m_selectedModel &&
            load->status.load() == nxs::IResourceLoader::LoadResult::Status::Ready)
        {
            const auto model = PTR_CAST<nxs::Model>(load->resource);
            if (!model || model->GetMeshes().empty())
            {
                m_modelError = std::format("Model {} has no meshes.", load->path);
                LOG_ERROR(LogTemp, m_modelError);
                m_displayedModel = m_selectedModel;
                return;
            }
            const auto& bounds = model->GetBoundingBox();
            m_modelOffset = {-bounds.center.x, -bounds.center.y + bounds.extent.y, -bounds.center.z};
            for (const auto& mesh : model->GetMeshes())
            {
                auto child = m_modelNode->EmplaceChild<nxs::SceneNode>(mesh->GetName());
                child->AddComponent<nxs::MeshComponent>()->SetMesh(mesh);
            }
            m_displayedModel = m_selectedModel;
        }

        m_modelNode->SetPosition({0.0f, m_height, 0.0f});
        m_modelNode->SetOrientation(glm::quat(glm::radians(m_euler)));
        m_modelNode->SetScale(modelScales[m_selectedModel] * m_scale);
        nxs::SceneNode::ChildList children;
        m_modelNode->GetAllChildren(children);
        for (const auto& child : children)
        {
            child->SetPosition(m_modelOffset);
            auto* mesh = child->GetComponent<nxs::MeshComponent>();
            mesh->showBoundingBox = m_drawBox;
            mesh->showBoundingSphere = m_drawSphere;
            if (const auto material = mesh->GetMesh()->GetMaterial())
            {
                material->castShadow = m_castShadow;
                material->receiveShadows = m_receiveShadows;
            }
        }
    }

protected:
    nxs::Ref<nxs::Scene> m_scene;
    nxs::CameraComponent m_camera;
    glm::vec3 m_euler {};
    glm::vec3 m_modelOffset {};
    std::string m_modelError;
    nxs::Ref<nxs::SceneNode> m_modelNode;
    nxs::Ref<nxs::DirectionalLight> m_light;
    nxs::Ref<nxs::Material> m_groundMaterial;
    nxs::Ptr<nxs::ModelLoader> m_modelLoader;
    nxs::Array<nxs::Ref<nxs::IResourceLoader::LoadResult>> m_loadedModels;
    nxs::Ref<nxs::SceneGraphWidget> m_sceneGraphWidget;
    nxs::Ref<nxs::PropertyWindow> m_propertyWindow;
    size_t m_selectedModel = 0;
    size_t m_displayedModel = std::numeric_limits<size_t>::max();
    float m_scale = 1.0f;
    float m_height = 0.5f;
    bool m_drawBox = false;
    bool m_drawSphere = false;
    bool m_castShadow = true;
    bool m_receiveShadows = true;
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
    return nxs::RunApplication<Example_06>({
        "Example 06 - Shadow Mapping",
        graphicsConfig,
        fullscreen,
        true,
        // editMode
        true,
        // maximize
        true,
        // allowProfile
        false,
    });
}
