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

        MAYBE_UNUSED bool VisitBool(const std::string& name, bool& value) override;
        MAYBE_UNUSED bool VisitInt(const std::string& name, int32_t& value) override;
        MAYBE_UNUSED bool VisitUInt(const std::string& name, uint32_t& value) override;
        MAYBE_UNUSED bool VisitInt64(const std::string& name, int64_t& value) override;
        MAYBE_UNUSED bool VisitUInt64(const std::string& name, uint64_t& value) override;
        MAYBE_UNUSED bool VisitFloat(const std::string& name, float& value) override;
        MAYBE_UNUSED bool VisitDouble(const std::string& name, double& value) override;
        MAYBE_UNUSED bool VisitString(const std::string& name, std::string& value) override;
        MAYBE_UNUSED bool VisitVec2(const std::string& name, glm::vec2& value) override;
        MAYBE_UNUSED bool VisitVec3(const std::string& name, glm::vec3& value) override;
        MAYBE_UNUSED bool VisitVec4(const std::string& name, glm::vec4& value) override;
        MAYBE_UNUSED bool VisitColor3(const std::string& name, Color3F& value) override;
        MAYBE_UNUSED bool VisitColor4(const std::string& name, Color4F& value) override;
        MAYBE_UNUSED bool VisitObject(const std::string& name, IReflection& value) override;
        MAYBE_UNUSED bool VisitEnum(const std::string& name, int32_t& value, const Array<std::string>& enumStrings) override;

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;
        void ChangeCurrentScene(Ref<Scene> scene);

    private:
        Identifier m_sceneNodeId = NXS_INVALID_ID;
        Ref<Scene> m_currentScene;
        uint32_t m_currentCategory = 0;
    };
}