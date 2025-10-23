#include "editor/menu/MenuItem.h"

USING_NAMESPACE_NXS;

MenuItem::MenuItem()
{

}

MenuItem::MenuItem(uint32 group, const std::string& name, const std::string& desc, const std::string& shortcut)
    : m_group(group)
    , m_name(name)
    , m_description(desc)
    , m_shortcut(shortcut)
{

}