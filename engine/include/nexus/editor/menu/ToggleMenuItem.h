#pragma once

#include "MenuItem.h"

NXS_NAMESPACE
{
    class ToggleMenuItem : public MenuItem
    {
    public:
        using HandleFunction = std::function<void(bool)>;

        ToggleMenuItem();
        ToggleMenuItem(
            uint32_t group,
            const std::string& name,
            const std::string& desc,
            const std::string& shortcut,
            HandleFunction handler);

        void OnSelected(bool is_selected) override;
        bool IsSelected() const override { return m_selected; }

    private:
        bool m_selected = false;
        HandleFunction m_hander;

        virtual bool* SelectedFlag() { return &m_selected; }
    };
}