#include "editor/menu/TriggerMenuItem.h"

USING_NAMESPACE_NXS;

TriggerMenuItem::TriggerMenuItem()
{

}

TriggerMenuItem::TriggerMenuItem(
    uint32_t group,
    const std::string& name,
    const std::string& desc,
    const std::string& shortcut,
    HandleFunction handler)
    : MenuItem(group, name, desc, shortcut)
    , m_handler(handler)
{
    
}

void TriggerMenuItem::OnSelected(bool is_selected)
{
    m_handler();
}