#include "core/Path.h"

#include <filesystem>

using fs_path = std::filesystem::path;

USING_NAMESPACE_NXS;

std::string Path::GetAssetPath(const std::string& filepath)
{
    return (fs_path(NXS_ASSETS_DIR) / filepath).string();;
}

std::string Path::GetEngineAssetPath(const std::string& filepath)
{
    return (fs_path(NXS_ENGINE_ASSETS_DIR) / filepath).string();;
}
std::string Path::GetGeneratedAssetPath(const std::string& filepath)
{
    return (fs_path(NXS_GENERATED_ASSETS_DIR) / filepath).string();;
}