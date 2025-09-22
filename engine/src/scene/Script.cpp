#include "scene/Script.h"

USING_NAMESPACE_NXS;

void Script::Enable(bool enable)
{
    if (m_enable != enable)
    {
        m_enable = enable;
        if (enable) OnEnable();
        else OnDisable();
    }
}