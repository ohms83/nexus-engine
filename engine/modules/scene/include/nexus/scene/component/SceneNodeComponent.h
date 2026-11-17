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
        IMPLEMENT_COMPONENT(SceneNodeComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetReadOnlyFlag(true);
            reflector.VisitUInt("ID", id);
            reflector.SetReadOnlyFlag(false);

            reflector.VisitString("Name", name);
        }

        VariantData Serialize() const override
        {
            VariantData data = Base::Serialize();
            data["id"] = INT64_CAST(id);
            data["name"] = name;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Base::Deserialize(data)) return false;

            id = CAST<Identifier>(data["id"].GetInt());
            name = data["name"].GetString();
            return true;
        }

        Identifier id = NXS_INVALID_ID;
        std::string name;
    };
}
