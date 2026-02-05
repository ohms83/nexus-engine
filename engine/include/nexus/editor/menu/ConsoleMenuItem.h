#pragma once

#include "WidgetMenuItem.h"
#include "../widget/Console.h"

NXS_NAMESPACE
{
    class ConsoleMenuItem : public WidgetMenuItem
    {
    public:
        using Super = WidgetMenuItem;

        ConsoleMenuItem(
            uint32 group,
            const std::string& name,
            const std::string& desc,
            const std::string& shortcut,
            Ref<Console> widget,
            IWidgetOwner& owner,
            class LogDispatcher& logger);
    };
}