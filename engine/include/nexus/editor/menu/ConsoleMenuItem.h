#pragma once

#include "WidgetMenuItem.h"
#include "../Console.h"

NXS_NAMESPACE
{
    class ConsoleMenuItem : public WidgetMenuItem
    {
    public:
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