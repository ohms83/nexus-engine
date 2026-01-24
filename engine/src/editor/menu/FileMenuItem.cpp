#include "editor/menu/FileMenuItem.h"
#include "core/LogDispatcher.h"

USING_NAMESPACE_NXS;

FileMenuItem::FileMenuItem(
    uint32_t group,
    const std::string& name,
    const std::string& desc,
    const std::string& shortcut,
    const FileMenuContext& context)
    : TriggerMenuItem(group, name, desc, shortcut, [this]() {
        ShowFileDialog();
    })
    , m_context(context)
{
}

void FileMenuItem::ShowFileDialog() const
{
    if (m_context.windowContext == nullptr) 
    {
        LOG_WARNING(LogMenu, "Unable to show file dialog. Reason=Window context is null.");
        return;
    }

    FileDialogContext context;
    context.title = GetName();
    context.defaultExtension = m_context.defaultExtension;
    context.filters = m_context.filters;
    context.mode = m_context.dialogMode;

    auto filepath = FileDialog::ShowFileDialog(context);
    OnDialogClosed(filepath);
}