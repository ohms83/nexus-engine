#ifdef NXS_PLATFORM_WIN64

#include "nexus/editor/FileDialog.h"
#include "nexus/core/LogDispatcher.h"

#include <filesystem>
#include <windows.h>
#include <commdlg.h>

#define FILTER_MAX 512

DEFINE_LOG(FileDialog);

namespace nxs
{
    /**
     * @brief Constructs a filter string for the Windows OPENFILENAME structure.
     *
     * This function takes a vector of ExtFilter objects and creates a single,
     * double-null-terminated string that is required by the `lpstrFilter` member of
     * the OPENFILENAME structure. The format consists of pairs of null-terminated
     * strings, with the first string in the pair being the display name (e.g., "Image Files")
     * and the second being the filter pattern (e.g., "*.jpg;*.png"). The entire
     * list is terminated by an additional null character.
     *
     * @param filters A vector of ExtFilter objects to be converted.
     * @param filter A character buffer to write the formatted filter string into.
     * @param filterSize The total size of the `filter` buffer.
     * @return The number of filters successfully added to the string. This might be
     *         less than the input count if the buffer size is insufficient.
     */
    MAYBE_UNUSED size_t ConstructFilterList(const std::vector<ExtFilter>& filters, char* filter, size_t filterSize)
    {
        size_t index = 0;
        size_t numFilter = 0;
        for (const auto& extFilter : filters)
        {
            const auto nameLen = extFilter.name.length();
            const auto filterLen = extFilter.filter.length();

            if (nameLen + filterLen >= (filterSize - index - 3)) break;

            NXS_STRNCPY(&filter[index], filterSize - index, extFilter.name.c_str(), nameLen);
            index += nameLen;
            filter[index] = '\0';
            index++;
            NXS_STRNCPY(&filter[index], filterSize - index, extFilter.filter.c_str(), filterLen);
            index += filterLen;
            filter[index] = '\0';
            index++;

            numFilter++;
        }
        filter[index] = '\0';
        return numFilter;
    }

    std::string ShowFileDialog(const FileDialogContext& context)
    {
        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(ofn));

        HWND hwnd = (HWND)SDL_GetPointerProperty(
            SDL_GetWindowProperties(context.windowContext),
            SDL_PROP_WINDOW_WIN32_HWND_POINTER,
            NULL);
        // Convert the initial filepath to the OS preferred format (replacing "/" with "\" in the case of Windows).
        std::string defaultDir = std::filesystem::path(context.initalDirectory).make_preferred().string();
        std::string title = context.title;
        std::string ext = context.defaultExtension;

        char fileName[MAX_PATH] = ""; // Output file path.
        char filter[FILTER_MAX] = "";
        ZeroMemory(&fileName, sizeof(fileName));
        ZeroMemory(&filter, sizeof(filter));

        const auto numFilter = ConstructFilterList(context.filters, filter, FILTER_MAX);
        if (numFilter != context.filters.size())
        {
            LOG_WARNING(LogFileDialog, std::format("Filter list is too long. Applied filter {}/{}", numFilter, context.filters.size()));
        }

        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrInitialDir = defaultDir.c_str();
        ofn.lpstrTitle = title.empty() ? nullptr : title.c_str();
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = ext.empty() ? nullptr : ext.c_str();

        bool success = false;
        if (context.mode == FileDialogContext::Mode::Open)
        {
            ofn.Flags |= OFN_FILEMUSTEXIST;
            success = GetOpenFileName(&ofn);
        }
        else if (context.mode == FileDialogContext::Mode::Save)
        {
            ofn.Flags |= OFN_OVERWRITEPROMPT;
            success = GetSaveFileName(&ofn);
        }

        std::string result = success ? ofn.lpstrFile : std::string();
        return result;
    }
}

#endif // NXS_PLATFORM_WIN64