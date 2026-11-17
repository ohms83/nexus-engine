#pragma once

#include "SceneNode.h"

#include "nexus/graphics/Model.h"

NXS_NAMESPACE
{
    class ModelNode : public SceneNode
    {
    public:
        using Base = SceneNode;

        ModelNode(Ref<entt::registry> registry, std::string name = "");
        virtual ~ModelNode() override = default;

        IMPLEMENT_NODE(ModelNode);

        NODISCARD Box GetBound() const override;

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

        void Resolve(ResourceManager& resourceManager, const RenderingInterface& renderingInterface) override;

    private:
        Ref<Model> m_model;
    };
}