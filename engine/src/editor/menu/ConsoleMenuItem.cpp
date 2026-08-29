#include "editor/menu/ConsoleMenuItem.h"
#include "editor/widget/Console.h"

#include "nexus/debug/LogDispatcher.h"

USING_NAMESPACE_NXS;

ConsoleMenuItem::ConsoleMenuItem(
    uint32_t group,
    const std::string& name,
    const std::string& desc,
    const std::string& shortcut,
    Ref<Console> widget,
    IWidgetOwner& owner,
    class LogDispatcher& logger)
    : Super(group, name, desc, shortcut, widget, owner)
{
    logger.AddLogger(PTR_CAST<ILogger>(widget));
}