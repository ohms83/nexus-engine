#pragma once

#include "nexus/NxsDefine.h"

NXS_NAMESPACE
{
    class SceneNode;

    class Script
    {
    public:
        Script() = default;
        explicit Script(Ref<SceneNode> owner) : m_owner(owner) {}
        virtual ~Script() = default;

        virtual void Begin() {}

        virtual void Update(float dt)
        {
            if (m_firstFrame)
            {
                Begin();
                m_firstFrame = false;
            }
        };

        void Enable(bool enable);

        void SetOwner(Ref<SceneNode> owner) { m_owner = owner; }
        Ref<SceneNode> GetOwner() const { return m_owner; }

        void SetPriority(uint32_t priority) { m_priority = priority; }
        uint32_t GetPriority() const { return m_priority; }

    private:
        virtual void OnEnable() {}
        virtual void OnDisable() {}

        uint32_t m_priority = 0;
        Ref<SceneNode> m_owner;
        bool m_enable = true;
        bool m_firstFrame = true;
    };
}