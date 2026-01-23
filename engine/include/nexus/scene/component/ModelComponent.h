#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/Model.h"
#include "nexus/ecs/Component.h"

NXS_NAMESPACE
{
    // TODO: Deprecate this in favor of MeshComponent
    struct ModelComponent : public IComponent
    {
        using Super = IComponent;

        IMPLEMENT_COMPONENT(ModelComponent);
        
        void AcceptReflector(IReflector& reflector) override
        {
            if (!model) return;

            auto path = model->GetPath();

            reflector.SetMarker("Model");
            if (reflector.VisitString("Model Path", path)) {
                // TODO:
            };
            reflector.VisitBool("Show Bounding Sphere", showBoundingSphere);
            reflector.VisitBool("Show Bounding Box", showBoundingBox);
        };

        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["modelPath"] = model->GetPath();
            data["showBoundingBox"] = showBoundingBox;
            data["showBoundingSphere"] = showBoundingSphere;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            // TODO: Load model
            showBoundingBox = data["showBoundingBox"].GetBool();
            showBoundingSphere = data["showBoundingSphere"].GetBool();
            return true;
        }

        Ref<Model> model;
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