#pragma once

#include "MenuItem.h"
#include "nexus/core/LogDispatcher.h"

NXS_NAMESPACE
{
    class EditorWidget;
    class IWidgetOwner;

    /**
     * @brief A menu item that will show/hide the specified widget upon interaction.
     * 
     */
    class WidgetMenuItem : public MenuItem
    {
    public:
        WidgetMenuItem(
            uint32_t group,
            const std::string& name,
            const std::string& desc,
            const std::string& shortcut,
            Ref<EditorWidget> widget,
            IWidgetOwner& owner);

        void OnSelected(bool is_selected) override;
        bool IsSelected() const override;

        Ref<EditorWidget> GetWidget() const { return m_widget; }
    
    private:
        Ref<EditorWidget> m_widget;
    };
}