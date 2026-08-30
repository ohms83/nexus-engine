#pragma once

#include "FileMenuItem.h"
#include "nexus/scene/Scene.h"
#include "nexus/serialize/Serializer.h"

NXS_NAMESPACE
{
    class SceneManager;

    class SceneFileMenuItem : public FileMenuItem
    {
    public:
        using Super = FileMenuItem;

        SceneFileMenuItem(
            const SceneManager& sceneManager,
            Ref<ISerializer> serializer,
            uint32_t group,
            const std::string& name,
            const std::string& desc,
            const std::string& shortcut,
            const FileMenuContext& context);

    protected:
        void OnDialogClosed(const std::string& selectedPath) const override;

        void SaveSceneToFile(Ref<Scene> scene, const std::string& filepath) const;
        void OpenSceneFromFile(Ref<Scene> scene, const std::string& filepath) const;

    private:
        const SceneManager& m_sceneManager;
        Ref<ISerializer> m_serializer;
    };

    class SceneImportMenuItem : public FileMenuItem
    {
    public:
        SceneImportMenuItem(uint32_t group, WindowContext windowContext);

    protected:
        void OnDialogClosed(const std::string& selectedPath) const override;
    };
}