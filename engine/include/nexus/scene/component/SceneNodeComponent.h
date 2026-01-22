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
            reflector.VisitBool("Active", active);
        }

        Identifier id = 0;
        std::string name;
        bool active = true;
    };
}
