#pragma once

#include "SceneNode3D.h"
#include "component/ModelComponent.h"

#include "nexus/graphics/Model.h"

NXS_NAMESPACE
{
    class ModelNode : public SceneNode3D
    {
    public:
        ModelNode(Ref<entt::registry> registry, std::string name = "");
        virtual ~ModelNode() override = default;

        IMPLEMENT_NODE(ModelNode);

        bool IsModelReady() const { return GetModel() != nullptr; }
        void SetModel(Ref<Model> model);

        Ref<const Model> GetModel() const
        {
            return m_model;
        }
        
        Ref<Model> GetModel()
        {
            return m_model;
        }

        void Resolve(ResourceManager& resourceManager) override;

    private:
        Ref<Model> m_model;
    };
}