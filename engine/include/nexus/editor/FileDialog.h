#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/ApplicationContext.h"

NXS_NAMESPACE
{
	/**
	 * @struct ExtFilter
	 * @brief Represents a file extension filter for use in a file dialog.
	 *
	 * This struct defines a filter with a user-friendly name and a pattern
	 * that specifies which file types are shown in the dialog.
	 */
	struct ExtFilter
	{
		/**
		 * @brief A friendly name for the filter (e.g., "Image Files").
		 * This name is displayed to the user in the file dialog's filter dropdown.
		 */
		std::string name;
		/**
		 * @brief The filter pattern (e.g., "*.jpg;*.jpeg;*.png").
		 * This pattern determines which files are displayed by the dialog.
		 */
		std::string filter;
	};

	/**
	 * @struct FileDialogContext
	 * @brief Holds the configuration for opening a file dialog.
	 *
	 * This struct is used to specify all the necessary parameters for creating
	 * and displaying a file dialog, such as its mode (open or save), title,
	 * initial directory, and file filters.
	 */
	struct FileDialogContext
	{
		/**
		 * @enum Mode
		 * @brief Defines the behavior of the file dialog.
		 */
		enum class Mode
		{
			/** @brief The dialog will be for opening an existing file. */
			Open,
			/** @brief The dialog will be for saving a new file. */
			Save,
		};

		/**
		 * @brief The application's window context. This cannot be null.
		 * The file dialog is modal to this window.
		 */
		WindowContext windowContext = nullptr;
		/**
		 * @brief The directory where the file dialog will initially open.
		 * Defaults to the standard assets directory.
		 */
		std::string initalDirectory = NXS_ASSETS_DIR;
		/**
		 * @brief A list of extension filters to apply.
		 * Defines the file types that will be shown in the dialog.
		 */
		std::vector<ExtFilter> filters;
		/**
		 * @brief The title of the file dialog window.
		 */
		std::string title;
		/**
		 * @brief The default extension to append to the filename if none is provided.
		 * This is primarily used in the "Save" mode.
		 */
		std::string defaultExtension;
		/**
		 * @brief The mode of the dialog, either Open or Save.
		 * Defaults to Open.
		 */
		Mode mode = Mode::Open;
	};

	namespace FileDialog
	{
		/**
		 * @brief Displays a system file dialog based on the provided context.
		 * @param context The configuration for the file dialog.
		 * @return The full path to the selected file, or an empty string if the dialog was canceled.
		 * @note This function is decorated with NODISCARD, indicating the return value should not be ignored.
		 */
		NODISCARD std::string ShowFileDialog(const FileDialogContext& context);
	}
}