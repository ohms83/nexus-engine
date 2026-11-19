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

    private:
        NODISCARD bool VisitBool_Implement(const std::string& name, bool& value) override;
        NODISCARD bool VisitInt_Implement(const std::string& name, int32_t& value) override;
        NODISCARD bool VisitUInt_Implement(const std::string& name, uint32_t& value) override;
        NODISCARD bool VisitInt64_Implement(const std::string& name, int64_t& value) override;
        NODISCARD bool VisitUInt64_Implement(const std::string& name, uint64_t& value) override;
        NODISCARD bool VisitFloat_Implement(const std::string& name, float& value) override;
        NODISCARD bool VisitDouble_Implement(const std::string& name, double& value) override;
        NODISCARD bool VisitString_Implement(const std::string& name, std::string& value) override;
        NODISCARD bool VisitVec2_Implement(const std::string& name, glm::vec2& value) override;
        NODISCARD bool VisitVec3_Implement(const std::string& name, glm::vec3& value) override;
        NODISCARD bool VisitVec4_Implement(const std::string& name, glm::vec4& value) override;
        NODISCARD bool VisitColor3_Implement(const std::string& name, Color3F& value) override;
        NODISCARD bool VisitColor4_Implement(const std::string& name, Color4F& value) override;
        NODISCARD bool VisitObject_Implement(const std::string& name, IReflection& value) override;
        NODISCARD bool VisitEnum_Implement(const std::string& name, int32_t& value, const Array<std::string>& enumStrings) override;
        NODISCARD bool VisitBitfield_Implement(const std::string& name, uint32_t& value, const Array<BitfieldOption>& options) override;

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;
        void ChangeCurrentScene(Ref<Scene> scene);

    private:
        Identifier m_sceneNodeId = NXS_INVALID_ID;
        Ref<Scene> m_currentScene;
        uint32_t m_currentCategory = 0;
    };
}