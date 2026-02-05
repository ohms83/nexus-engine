#pragma once

#include "MenuItem.h"

NXS_NAMESPACE
{
    /**
     * @brief A class refresents a stateless one-time trigger menu item. This menu item won't
     * retain any menu state and will always return IsSelected() == false. The example use cases
     * are open, close, or save files.
     */
    class TriggerMenuItem : public MenuItem
    {
    public:
        using Super = MenuItem;
        using HandleFunction = std::function<void()>;

        TriggerMenuItem();
        TriggerMenuItem(
            uint32_t group,
            const std::string& name,
            const std::string& desc,
            const std::string& shortcut,
            HandleFunction handler);

        void OnSelected(bool is_selected) override;
        bool IsSelected() const override { return false; }

    private:
        HandleFunction m_handler;
    };
}