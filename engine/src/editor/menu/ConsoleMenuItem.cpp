#include "editor/menu/ConsoleMenuItem.h"
#include "editor/widget/Console.h"

#include "core/LogDispatcher.h"

USING_NAMESPACE_NXS;

ConsoleMenuItem::ConsoleMenuItem(
    uint32 group,
    const std::string& name,
    const std::string& desc,
    const std::string& shortcut,
    Ref<Console> widget,
    IWidgetOwner& owner,
    class LogDispatcher& logger)
    : WidgetMenuItem(group, name, desc, shortcut, widget, owner)
{
    logger.AddLogger(PTR_CAST<ILogger>(widget));
}