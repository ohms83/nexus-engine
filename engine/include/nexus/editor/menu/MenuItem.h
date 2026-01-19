//
// Created by nutta on 7/18/2025.
//

#pragma once

#include <nexus/NxsDefine.h>
#include <functional>
#include <string>

NXS_NAMESPACE
{
    class MenuItem
    {
    public:
        MenuItem();
        MenuItem(uint32_t group, const std::string& name, const std::string& desc, const std::string& shortcut);
        virtual ~MenuItem() = default;

        virtual void OnSelected(bool is_selected) = 0;
        virtual bool IsSelected() const = 0;

        uint32_t GetGroup() const { return m_group; }
        const std::string& GetName() const { return m_name; }
        const std::string& GetDescription() const { return m_description; }
        const std::string& GetShortcut() const { return m_shortcut; }

    protected:
        uint32_t m_group;
        std::string m_name;
        std::string m_description;
        std::string m_shortcut;
    };
}
