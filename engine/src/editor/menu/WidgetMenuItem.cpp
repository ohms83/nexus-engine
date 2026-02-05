#include "editor/menu/WidgetMenuItem.h"
#include "editor/widget/EditorWidget.h"

USING_NAMESPACE_NXS;

WidgetMenuItem::WidgetMenuItem(
    uint32_t group,
    const std::string& name,
    const std::string& desc,
    const std::string& shortcut,
    Ref<EditorWidget> widget,
    IWidgetOwner& owner)
    : Super(group, name, desc, shortcut)
    , m_widget(widget)
{
    NXS_ASSERT(widget);
    owner.AddWidget(widget);
}

void WidgetMenuItem::OnSelected(bool is_selected)
{
    if (IsSelected() == is_selected) return;

    if (is_selected) m_widget->Show();
    else m_widget->Hide();

    Super::OnSelected(is_selected);
}

bool WidgetMenuItem::IsSelected() const
{
    return m_widget->GetVisibility() != EditorWidget::Visibility::Hidden;
}