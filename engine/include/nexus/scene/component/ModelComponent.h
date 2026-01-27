#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/Model.h"
#include "nexus/ecs/Component.h"
#include "nexus/core/resource/ResourceManager.h"


NXS_NAMESPACE
{
    // TODO: Deprecate this in favor of MeshComponent
    struct ModelComponent : public IComponent
    {
        using Super = IComponent;

        IMPLEMENT_COMPONENT(ModelComponent);
        
        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Model");
            if (reflector.VisitString("Model Path", modelPath)) {
                // TODO:
            };
            reflector.VisitBool("Show Bounding Sphere", showBoundingSphere);
            reflector.VisitBool("Show Bounding Box", showBoundingBox);
        };

        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["modelPath"] = modelPath;
            data["showBoundingBox"] = showBoundingBox;
            data["showBoundingSphere"] = showBoundingSphere;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            modelPath = data["modelPath"].GetString();
            showBoundingBox = data["showBoundingBox"].GetBool();
            showBoundingSphere = data["showBoundingSphere"].GetBool();
            return true;
        }

        void Resolve(ResourceManager& resourceManager) override
        {
            resourceManager.Cache(typeid(Model), modelPath);
        }

        Ref<Model> model;
        std::string modelPath;
        bool visible = true;
        bool showBoundingBox = false;
        bool showBoundingSphere = false;
    };

    struct MeshComponent : public IComponent
    {
        IMPLEMENT_COMPONENT(MeshComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Mesh");
            reflector.VisitBool("Show Bounding Sphere", showBoundingSphere);
            reflector.VisitBool("Show Bounding Box", showBoundingBox);
        }

        Ref<Mesh> mesh;
        bool visible = true;
        bool showBoundingBox = false;
        bool showBoundingSphere = false;
    };
}