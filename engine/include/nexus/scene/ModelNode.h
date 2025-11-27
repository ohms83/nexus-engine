#pragma once

#include "SceneNode3D.h"
#include "component/ModelComponent.h"

#include "nexus/graphics/Model.h"

NXS_NAMESPACE
{
    class ModelNode : public SceneNode3D
    {
    public:
        ModelNode(Ref<entt::registry> registry, Ref<Model> model);
        virtual ~ModelNode() = default;

        IMPLEMENT_REFLECTION(ModelNode);

        bool IsModelReady() const { return GetModel() != nullptr; }
        void SetModel(Ref<Model> model);

        Ref<const Model> GetModel() const
        {
            return GetComponent<ModelComponent>().model;
        }
        
        Ref<Model> GetModel()
        {
            return GetComponent<ModelComponent>().model;
        }
    };
}