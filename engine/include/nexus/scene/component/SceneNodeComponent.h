//
// Created by nutta on 7/26/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/ecs/Component.h"

NXS_NAMESPACE
{
    struct SceneNodeComponent : public IComponent
    {
        using Super = IComponent;

        IMPLEMENT_COMPONENT(SceneNodeComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetReadOnlyFlag(true);
            reflector.VisitUInt("ID", id);
            reflector.SetReadOnlyFlag(false);

            reflector.VisitString("Name", name);
            reflector.VisitBool("Active", active);
        }

        VariantData Serialize() const override
        {
            VariantData data = Super::Serialize();
            data["id"] = INT64_CAST(id);
            data["name"] = name;
            data["active"] = active;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;

            id = CAST<Identifier>(data["id"].GetInt());
            name = data["name"].GetString();
            active = data["active"].GetBool();
            return true;
        }

        Identifier id = 0;
        std::string name;
        bool active = true;
    };
}
