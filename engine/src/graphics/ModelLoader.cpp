//
// Created by nutta on 8/18/2025.
//
#include "graphics/ModelLoader.h"
#include "core/LogDispatcher.h"
#include "memory/OwningBuffer.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/mesh.h"

#include <filesystem>
#include <future>
#include <queue>

#include "core/task/FutureWaitingTask.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(ModelLoader);

namespace
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        // TODO: Multiple texture coordinates
        glm::vec2 texCoords;
    };

    const std::map<aiTextureType, TextureType> s_textureTypeMap = {
        {aiTextureType_DIFFUSE, TextureType::Diffuse},
        {aiTextureType_NORMALS, TextureType::Normal},
        {aiTextureType_SPECULAR, TextureType::Specular},
        {aiTextureType_EMISSIVE, TextureType::Emissive},
        {aiTextureType_HEIGHT, TextureType::Depth},
        {aiTextureType_AMBIENT, TextureType::Ambient},
        {aiTextureType_OPACITY, TextureType::Opacity},
        {aiTextureType_AMBIENT_OCCLUSION, TextureType::Ambient},
        {aiTextureType_METALNESS, TextureType::Metalness},
        {aiTextureType_DIFFUSE_ROUGHNESS, TextureType::Roughness},
        {aiTextureType_LIGHTMAP, TextureType::LightMap},
    };
}

/**
 * Asynchronously preload all the textures.
 * @param scene
 * @param manager
 * @param taskScheduler
 * @return A list of asynchronous resource loading results.
 */
static std::queue<Ref<IResourceLoader::LoadResult>> PreloadTextures(const aiScene& scene, const std::filesystem::path directory, TextureManager& manager, TaskScheduler& taskScheduler)
{
    std::queue<Ref<IResourceLoader::LoadResult>> result;
    for (int i = 0; i < scene.mNumTextures; i++)
    {
        const aiTexture* texture = scene.mTextures[i];
        auto texturePath = directory / texture->mFilename.C_Str();
        result.push(manager.RequestResourceAsync(texturePath.string(), taskScheduler));
    }

    std::vector<aiTextureType> aiTextures = {};
    for (const auto& aitTexture : s_textureTypeMap | std::views::keys)
    {
        aiTextures.push_back(aitTexture);
    }

    for (unsigned int i = 0; i < scene.mNumMaterials; i++)
    {
        const aiMaterial* material = scene.mMaterials[i];
        for (const auto aiTexture : aiTextures)
        {
            for (unsigned int j = 0; j < material->GetTextureCount(aiTexture); j++)
            {
                aiString path;
                material->GetTexture(aiTexture, i, &path);
                auto texturePath = directory / path.C_Str();
                result.push(manager.RequestResourceAsync(texturePath.string(), taskScheduler));
            }
        }
    }
    return result;
}

ModelLoader::ModelLoader(
    const Ref<RenderingInterface>& renderingInterface,
    const Ref<TextureManager>& textureManager,
    const Ref<MaterialManager>& materialManager)
{
    NXS_ASSERT(renderingInterface && textureManager && materialManager);
    m_renderingInterface = renderingInterface;
    m_textureManager = textureManager;
    m_materialManager = materialManager;
}

Ref<Resource> ModelLoader::Load(const std::string &path, uint32 id)
{
    Assimp::Importer importer;
    // The ReadFile method returns an aiScene object.
    // It's crucial to specify post-processing flags for desired data.
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate             | // Convert all faces to triangles
        aiProcess_GenSmoothNormals        | // Generate smooth normals if not present
        aiProcess_FlipUVs                 | // Flip UVs (often needed for OpenGL)
        aiProcess_CalcTangentSpace        | // Calculate tangent and bitangent vectors
        aiProcess_JoinIdenticalVertices   | // Join identical vertices for optimization
        aiProcess_RemoveRedundantMaterials| // Remove redundant materials
        aiProcess_OptimizeMeshes            // Optimize meshes for better performance
    );

    // Check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_ERROR(LogModelLoader, std::format("Failed to load model: {} Error: {}", path, importer.GetErrorString()));
        return nullptr;
    }

    // Store the directory path of the model file for texture loading
    const auto directory = std::filesystem::path(path).parent_path();
    auto model = std::make_shared<Model>(path, id);

    // Process Assimp's root node recursively
    ProcessNode(model, scene->mRootNode, scene, directory);
    ComputeBoundingVolume(model);

    LOG_INFO(LogModelLoader, std::format("Model loaded successfully: {}", path));
    LOG_INFO(LogModelLoader, std::format("Processed meshes: {}", scene->mNumMeshes));
    return model;
}

Ref<IResourceLoader::LoadResult> ModelLoader::LoadAsync(const std::string& path, uint32 id, TaskScheduler& scheduler, Callback onFinishCallback)
{
    const auto directory = std::filesystem::path(path).parent_path();

    const auto result = std::make_shared<LoadResult>();
    result->path = path;
    result->status = LoadResult::Status::Loading;

    std::future<Ref<aiScene>> future = std::async(std::launch::async, [path, directory, result, &scheduler, textureManager = m_textureManager]
    {
        if (!textureManager)
        {
            result->error = std::format("Failed to load model from file: {}. Reason: Invalid texture manager", path);
            result->status = LoadResult::Status::Failed;
            throw std::runtime_error(result->error);
        }

        Assimp::Importer importer;
        // The ReadFile method returns an aiScene object.
        // It's crucial to specify post-processing flags for desired data.
        importer.ReadFile(path,
            aiProcess_Triangulate             | // Convert all faces to triangles
            aiProcess_GenSmoothNormals        | // Generate smooth normals if not present
            aiProcess_FlipUVs                 | // Flip UVs (often needed for OpenGL)
            aiProcess_CalcTangentSpace        | // Calculate tangent and bitangent vectors
            aiProcess_JoinIdenticalVertices   | // Join identical vertices for optimization
            aiProcess_RemoveRedundantMaterials| // Remove redundant materials
            aiProcess_OptimizeMeshes            // Optimize meshes for better performance
        );
        auto scene = std::make_shared<aiScene>();
        scene.reset(importer.GetOrphanedScene());

        if (!scene)
        {
            result->error = std::format("Failed to load model from file: {}.", path);
            result->status = LoadResult::Status::Failed;
            throw std::runtime_error(result->error);
        }

        auto preloadedTextures = PreloadTextures(*scene, directory, *textureManager, scheduler);
        while (!preloadedTextures.empty())
        {
            switch (const auto loadResult = preloadedTextures.front(); loadResult->status)
            {
            case LoadResult::Status::Loading:
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                break;
            case LoadResult::Status::Invalid:
            case LoadResult::Status::Failed:
                result->error = std::format("Failed to load model from file: {}. Reason: Texture loading error {}", path, loadResult->path);
                result->status = LoadResult::Status::Failed;
                throw std::runtime_error(result->error);
            case LoadResult::Status::Ready:
                preloadedTextures.pop();
                break;
            }
        }
        return scene;
    });

    const auto waitingTask = std::make_shared<FutureWaitingTask<Ref<aiScene>>>(std::move(future), [this, onFinishCallback, path, directory, id, result](Ref<aiScene> scene)
    {
        if (!scene)
        {
            result->status = LoadResult::Status::Failed;
            onFinishCallback(nullptr);
            return;
        }

        // Store the directory path of the model file for texture loading
        const auto model = std::make_shared<Model>(path, id);

        // Process Assimp's root node recursively
        ProcessNode(model, scene->mRootNode, scene.get(), directory);
        ComputeBoundingVolume(model);

        result->status = LoadResult::Status::Ready;
        result->resource = model;
        onFinishCallback(model);
    }, [](const std::string& error)
    {
        LOG_ERROR(LogModelLoader, error);
    });

    scheduler.ScheduleTask(waitingTask);
    return result;
}

void ModelLoader::ProcessNode(const Ref<Model>& model, const aiNode* node, const aiScene* scene, std::filesystem::path directory)
{
    // Process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(model, mesh, scene, directory);
    }
    // Then do the same for its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(model, node->mChildren[i], scene, directory);
    }
}

void ModelLoader::ProcessMesh(const Ref<Model>& model, const aiMesh* mesh, const aiScene* scene, const std::filesystem::path& directory) const
{
    const auto newMesh = std::make_shared<Mesh>(mesh->mName.C_Str());
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;
    Ref<VertexBuffer> vertexBuffer;
    Ref<IndexBuffer> indexBuffer;
    vertexBuffer.reset(m_renderingInterface->CreateVertexBuffer());
    indexBuffer.reset(m_renderingInterface->CreateIndexBuffer());

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        if (mesh->HasTangentsAndBitangents())
        {
            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;
        }

        // TODO: Multiple texture coordinates
        if (mesh->mTextureCoords[0])
        {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        vertices.emplace_back(vertex);
    }

    auto vertexData = std::make_shared<OwningBuffer>();
    vertexData->Copy<Vertex>(vertices);

    vertexBuffer->Begin()
        .AddAttribute(VertexAttribute::VertexPosition3D)
        .AddAttribute(VertexAttribute::VertexNormal)
        .AddAttribute(VertexAttribute::VertexTangent)
        .AddAttribute(VertexAttribute::VertexTexCoord0)
        .SetUsage(BufferUsage::StaticDraw)
        .SetVertices(vertexData)
    .Build();
    newMesh->SetVertexBuffer(vertexBuffer);

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.emplace_back(face.mIndices[j]);
        }
    }

    auto indexData = std::make_shared<OwningBuffer>();
    indexData->Copy<uint32>(indices);

    indexBuffer->Begin()
        .SetIndices(indexData, FrontFace::CounterClockWise)
        .SetUsage(BufferUsage::StaticDraw)
        .SetDrawMode(DrawMode::Triangle)
    .Build();
    newMesh->SetIndexBuffer(indexBuffer);

    ProcessMaterial(newMesh, mesh, scene, directory);

    model->AddMesh(newMesh);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ModelLoader::ProcessMaterial(const Ref<Mesh>& newMesh, const aiMesh* mesh, const aiScene* scene, const std::filesystem::path& directory) const
{
    // A mesh has only one material. If an imported model uses multiple materials,
    // the importer splits up the mesh.
    const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    const Ref<Material> newMat = m_materialManager->GetOrCreate(material->GetName().C_Str());

#define READ_BOOL_PROPERTY(key, property) \
    if (int32 value; material->Get(key, value) == AI_SUCCESS) { \
        newMat->property = value; \
    }
#define READ_INT_PROPERTY(key, property) \
    if (int32 value; material->Get(key, value) == AI_SUCCESS) { \
        newMat->property = value; \
    }
#define READ_FLOAT_PROPERTY(key, property) \
    if (float value; material->Get(key, value) == AI_SUCCESS) { \
        newMat->property = value; \
    }
#define READ_COLOR_PROPERTY(key, property) \
    if (aiColor3D color; material->Get(key, color) == AI_SUCCESS) { \
        newMat->property.r = color.r; newMat->property.g = color.g; newMat->property.b = color.b; \
    }
#define READ_ENUM_PROPERTY(key, property, enumType) \
    if (int32 value; material->Get(key, value) == AI_SUCCESS) { \
        newMat->property = CAST<enumType>(value); \
    }

    READ_COLOR_PROPERTY(AI_MATKEY_COLOR_AMBIENT, ambient);
    READ_COLOR_PROPERTY(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    READ_COLOR_PROPERTY(AI_MATKEY_COLOR_SPECULAR, specular);
    READ_COLOR_PROPERTY(AI_MATKEY_COLOR_EMISSIVE, emissive);
    READ_FLOAT_PROPERTY(AI_MATKEY_SHININESS, shininess);
    READ_ENUM_PROPERTY(AI_MATKEY_BLEND_FUNC, blendMode, BlendMode);
    READ_BOOL_PROPERTY(AI_MATKEY_TWOSIDED, cull);
    READ_BOOL_PROPERTY(AI_MATKEY_ENABLE_WIREFRAME, wireframe);

#undef READ_BOOL_PROPERTY
#undef READ_INT_PROPERTY
#undef READ_FLOAT_PROPERTY
#undef READ_COLOR_PROPERTY
#undef READ_ENUM_PROPERTY

    ProcessTextures(newMat, material, directory);

    if (newMat->GetShader() == nullptr) {
        newMat->CreateDefaultShader(m_renderingInterface);
    }
    newMesh->SetMaterial(newMat);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ModelLoader::ProcessTextures(const Ref<Material>& newMat, const aiMaterial* material, const std::filesystem::path& directory) const
{
    for (const auto& [aiType, textureType] : s_textureTypeMap)
    {
        for (int32 i = 0; i < material->GetTextureCount(aiType); ++i)
        {
            aiString path;
            material->GetTexture(aiType, i, &path);

            const std::string texturePath = (directory / path.C_Str()).string();
            if (const auto texture = m_textureManager->Get(texturePath)) {
                newMat->AddTexture(texture, textureType);
            }
        }
    }
}

void ModelLoader::ComputeBoundingVolume(const Ref<Model>& model)
{
    glm::vec3 min{FLT_MAX}, max{FLT_MIN}, center{};
    for (const auto& mesh : model->GetMeshes())
    {
        const auto vertexBuffer = mesh->GetVertexBuffer();
        const auto numVertex = vertexBuffer->GetNumVertex();

        for (auto vertexItr = vertexBuffer->Begin<Vertex>(); vertexItr != vertexBuffer->End<Vertex>(); ++vertexItr)
        {
            const auto& vertex = *vertexItr;
            min.x = std::min(min.x, vertex.position.x);
            min.y = std::min(min.y, vertex.position.y);
            min.z = std::min(min.z, vertex.position.z);

            max.x = std::max(max.x, vertex.position.x);
            max.y = std::max(max.y, vertex.position.y);
            max.z = std::max(max.z, vertex.position.z);

            center += vertex.position / (float)numVertex;
        }
    }

    LOG_DEBUG(LogModelLoader, std::format("center({}, {}, {}) max({}, {}, {}) min({}, {}, {})",
        center.x, center.y, center.z,
        max.x, max.y, max.z,
        min.x, min.y, min.z
    ));
    model->SetBoundingBox(center, max - center);

    float radius = 0;
    for (const auto& mesh : model->GetMeshes())
    {
        const auto vertexBuffer = mesh->GetVertexBuffer();
        for (auto vertexItr = vertexBuffer->Begin<Vertex>(); vertexItr != vertexBuffer->End<Vertex>(); ++vertexItr)
        {
            const auto& vertex = *vertexItr;
            radius = std::max(radius, glm::length(center - vertex.position));
        }
    }
    model->SetBoundingSphere(center, radius);
}
