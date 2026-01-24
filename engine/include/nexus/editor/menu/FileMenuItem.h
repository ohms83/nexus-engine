#pragma once

#include "TriggerMenuItem.h"
#include "../FileDialog.h"

NXS_NAMESPACE
{
    struct FileMenuContext
    {
		WindowContext windowContext = nullptr;
        std::string defaultExtension;
        std::vector<ExtFilter> filters;
        FileDialogContext::Mode dialogMode;
    };

    class FileMenuItem : public TriggerMenuItem
    {
    public:
        FileMenuItem(
            uint32_t group,
            const std::string& name,
            const std::string& desc,
            const std::string& shortcut,
            const FileMenuContext& context);
        
        void ShowFileDialog() const;

    private:
        virtual void OnDialogClosed(const std::string& selectedPath) const = 0;

        FileMenuContext m_context;
    };
}