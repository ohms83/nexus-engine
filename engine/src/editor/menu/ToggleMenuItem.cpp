#include "editor/menu/ToggleMenuItem.h"

USING_NAMESPACE_NXS;

ToggleMenuItem::ToggleMenuItem()
{
}

ToggleMenuItem::ToggleMenuItem(
    uint32 group,
    const std::string& name,
    const std::string& desc,
    const std::string& shortcut,
    HandleFunction handler)
    : Super(group, name, desc, shortcut)
    , m_hander(handler)
{}

void ToggleMenuItem::OnSelected(const bool is_selected)
{
    m_selected = is_selected;
    m_hander(is_selected);

    Super::OnSelected(is_selected);
}