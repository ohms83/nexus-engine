#pragma once

#include "nexus/NxsDefine.h"

NXS_NAMESPACE
{
    class SceneNode;

    class Script
    {
    public:
        Script() = default;
        virtual ~Script() = default;

        virtual void Begin() {}
        virtual void Update(float dt) = 0;

        void Enable(bool enable);

        Ref<SceneNode> GetOwner() const { return m_owner; }

        void SetPriority(uint32_t priority) { m_priority = priority; }
        uint32_t GetPriority() const { return m_priority; }

    private:
        virtual void OnEnable() {}
        virtual void OnDisable() {}

        uint32_t m_priority = 0;
        Ref<SceneNode> m_owner;
        bool m_enable = true;
    };
}