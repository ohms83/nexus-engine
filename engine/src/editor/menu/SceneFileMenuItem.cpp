#include "editor/menu/SceneFileMenuItem.h"
#include "scene/SceneManager.h"

USING_NAMESPACE_NXS;

SceneFileMenuItem::SceneFileMenuItem(
    const SceneManager& sceneManager,
    Ref<ISerializer> serializer,
    uint32_t group,
    const std::string& name,
    const std::string& desc,
    const std::string& shortcut,
    const FileMenuContext& context)
    : FileMenuItem(group, name, desc, shortcut, context)
    , m_sceneManager(sceneManager)
    , m_serializer(serializer)
{
}

void SceneFileMenuItem::OnDialogClosed(const std::string& selectedPath) const
{
    if (selectedPath.empty()) return;

    const auto scene = m_sceneManager.GetCurrentScene();
    if (!scene) return;

    if (GetDialogMode() == FileDialogContext::Mode::Open)
    {
        OpenSceneFromFile(scene, selectedPath);
    }
    else
    {
        SaveSceneToFile(scene, selectedPath);
    }
}

void SceneFileMenuItem::SaveSceneToFile(Ref<Scene> scene, const std::string& filepath) const
{
    std::ofstream file(filepath);
    if (!file.good()) return;

    m_serializer->Pack(scene->Serialize(), file);
}

void SceneFileMenuItem::OpenSceneFromFile(Ref<Scene> scene, const std::string& filepath) const
{
    std::ifstream file(filepath);
    if (!file.good()) return;

    scene->Deserialize(m_serializer->Unpack(file));
}
