#pragma once

#include "TriggerMenuItem.h"
#include "../FileDialog.h"

NXS_NAMESPACE
{
    struct FileMenuContext
    {
        //! The application's window context. This cannot be null.
        WindowContext windowContext = nullptr;
        //! The file filters to be used in the dialog.
        std::vector<ExtFilter> filters;
        //! The default file extension (without dot).
        std::string defaultExtension = "";
        //! The dialog mode (open or save).
        FileDialogContext::Mode dialogMode = FileDialogContext::Mode::Open;
    };

    class FileMenuItem : public TriggerMenuItem
    {
    public:
        using Super = TriggerMenuItem;

        FileMenuItem(
            uint32_t group,
            const std::string& name,
            const std::string& desc,
            const std::string& shortcut,
            const FileMenuContext& context);
        
        void ShowFileDialog() const;

        FileDialogContext::Mode GetDialogMode() const
        {
            return m_context.dialogMode;
        }

    private:
        virtual void OnDialogClosed(const std::string& selectedPath) const = 0;

        FileMenuContext m_context;
    };
}