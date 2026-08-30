#pragma once

#include "EditorWidget.h"
#include "nexus/scene/SceneNode.h"
#include "nexus/reflection/Reflection.h"

NXS_NAMESPACE
{
    class SceneManager;
    class Scene;

    class PropertyWindow : public EditorWidget, public IReflector
    {
    public:
        explicit PropertyWindow(SceneManager& sceneManager);
        virtual ~PropertyWindow() = default;

        void SetSceneNode(Identifier sceneNodeId);

        virtual void Update() override {}
        
        void SetMarker(const std::string& name) override;

        void SetReadOnlyFlag(bool value) override;

        bool VisitBool(const std::string& name, bool& value) override;
        bool VisitInt(const std::string& name, int32_t& value) override;
        bool VisitUInt(const std::string& name, uint32_t& value) override;
        bool VisitInt64(const std::string& name, int64_t& value) override;
        bool VisitUInt64(const std::string& name, uint64_t& value) override;
        bool VisitFloat(const std::string& name, float& value) override;
        bool VisitDouble(const std::string& name, double& value) override;
        bool VisitString(const std::string& name, std::string& value) override;
        bool VisitVec2(const std::string& name, glm::vec2& value) override;
        bool VisitVec3(const std::string& name, glm::vec3& value) override;
        bool VisitVec4(const std::string& name, glm::vec4& value) override;
        bool VisitColor3(const std::string& name, Color3F& value) override;
        bool VisitColor4(const std::string& name, Color4F& value) override;
        bool VisitObject(const std::string& name, IReflector& value) override;

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;
        void ChangeCurrentScene(Ref<Scene> scene);

    private:
        Identifier m_sceneNodeId = InvalidID;
        Ref<Scene> m_currentScene;
        uint32_t m_currentCategory = 0;
    };
}