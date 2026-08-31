#include "nexus/graphics/ShaderLoader.h"
#include "nexus/debug/LogDispatcher.h"

#include <format>

USING_NAMESPACE_NXS;

DEFINE_LOG(ShaderLoader);

Ref<Resource> ShaderLoader::Load(const std::string &path, uint32_t id)
{
    const auto shader = std::make_shared<Shader>(path, id);
    std::string veretxShaderSource, fragmentShaderSource, geomtryShaderSource;
    
    if (!m_shaderGenerator.GenerateShaderSource(path,
        veretxShaderSource,
        fragmentShaderSource,
        geomtryShaderSource))
    {
        LOG_FATAL(LogShaderLoader, std::format("Failed to generate shader: {}", path));
        return nullptr;
    }

    shader->CompileFromSource(*m_renderingInterface, veretxShaderSource, fragmentShaderSource, "");
    return PTR_CAST<Resource>(shader);
}
Ref<IResourceLoader::LoadResult> ShaderLoader::LoadAsync(const std::string& path, uint32_t id, TaskScheduler& scheduler, Callback onFinishCallback)
{
    return std::make_shared<IResourceLoader::LoadResult>();
}