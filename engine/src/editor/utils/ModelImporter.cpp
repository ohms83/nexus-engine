#include "editor/utils/ModelImporter.h"

#include "Engine.h"

#include "core/LogDispatcher.h"
#include "core/task/FutureWaitingTask.h"
#include "core/resource/ResourceManager.h"
#include "core/serialize/JsonSerializer.h"
#include "core/serialize/MsgPackSerializer.h"

#include "geom/Box.h"
#include "geom/Sphere.h"

#include "graphics/Mesh.h"
#include "graphics/Material.h"

#include "scene/Scene.h"
#include "scene/SceneNode.h"
#include "scene/SceneNode3D.h"
#include "scene/component/MeshComponent.h"
#include "scene/component/GeometryComponent.h"

#include "memory/OwningBuffer.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/mesh.h"

#include <glm/vec3.hpp>

#include <future>
#include <queue>
#include <map>

USING_NAMESPACE_NXS;

DEFINE_LOG(ModelImporter);

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
static std::queue<Ref<IResourceLoader::LoadResult>> PreloadTextures(const aiScene& scene, const std::filesystem::path directory, ResourceManager* manager, TaskScheduler& taskScheduler)
{
    std::queue<Ref<IResourceLoader::LoadResult>> result;
    for (int i = 0; i < scene.mNumTextures; i++)
    {
        const aiTexture* texture = scene.mTextures[i];
        auto texturePath = directory / texture->mFilename.C_Str();
        auto loadResult = manager->GetResourceAsync(typeid(Texture), texturePath.string(), taskScheduler);

        if (loadResult) result.push(loadResult);
    }

    std::vector<aiTextureType> aiTextures = {};
    for (const auto& aitTexture : s_textureTypeMap | std::views::keys)
    {
        aiTextures.push_back(aitTexture);
    }

    for (unsigned int i = 0; i < scene.mNumMaterials; i++)
    {
        const aiMaterial* material = scene.mMaterials[i];
        LOG_INFO(LogModelImporter, std::format("Loading texture: Material={}", material->GetName().C_Str()));
        for (const auto aiTexture : aiTextures)
        {
            const auto numTexture = material->GetTextureCount(aiTexture);
            for (unsigned int j = 0; j < numTexture; j++)
            {
                aiString path;
                material->GetTexture(aiTexture, j, &path);
                auto texturePath = directory / path.C_Str();
                auto loadResult = manager->GetResourceAsync(typeid(Texture), texturePath.string(), taskScheduler);

                if (loadResult) result.push(loadResult);
            }
        }
    }
    return result;
}

Ref<SceneNode> ModelImporter::Import(const std::string& path)
{
    Assimp::Importer importer;
    // The ReadFile method returns an aiScene object.
    // It's crucial to specify post-processing flags for desired data.
    const aiScene* ai_scene = importer.ReadFile(path,
        aiProcess_Triangulate             | // Convert all faces to triangles
        aiProcess_GenSmoothNormals        | // Generate smooth normals if not present
        aiProcess_FlipUVs                 | // Flip UVs (often needed for OpenGL)
        aiProcess_CalcTangentSpace        | // Calculate tangent and bitangent vectors
        aiProcess_JoinIdenticalVertices   | // Join identical vertices for optimization
        aiProcess_RemoveRedundantMaterials| // Remove redundant materials
        aiProcess_OptimizeMeshes            // Optimize meshes for better performance
    );

    // Check for errors
    if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
        LOG_ERROR(LogModelImporter, std::format("Failed to load model: {} Error: {}", path, importer.GetErrorString()));
        return nullptr;
    }

    // Store the directory path of the model file for texture loading
    const auto directory = std::filesystem::path(path).parent_path();
    auto thisScene = Engine::Instance().GetSceneManager()->GetCurrentScene();    
    auto thisNode = thisScene->EmplaceChild<SceneNode3D>(ai_scene->mRootNode->mName.C_Str());
    ProcessNode(thisNode, ai_scene->mRootNode, ai_scene, directory);

    auto sphereComp = thisNode->AddComponent<SphereComponent>();
    auto boxComp = thisNode->AddComponent<BoxComponent>();
    ComputeBoundingVolume(thisNode);

    LOG_INFO(LogModelImporter, "Model loaded successfully!!");
    // LOG_INFO(LogModelImporter, model->DumpStats());
    return nullptr;
}

void ModelImporter::ProcessNode(Ref<SceneNode> thisNode, const aiNode* ai_node, const aiScene* ai_scene, const std::filesystem::path& directory)
{
    // Process all the node's meshes (if any)
    for (unsigned int i = 0; i < ai_node->mNumMeshes; i++) {
        const aiMesh* mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
        ProcessMesh(thisNode, mesh, ai_scene, directory);
    }
    // Then do the same for its children
    for (unsigned int i = 0; i < ai_node->mNumChildren; i++) {
        auto childNode = thisNode->EmplaceChild<SceneNode3D>(ai_node->mChildren[i]->mName.C_Str());
        ProcessNode(childNode, ai_node->mChildren[i], ai_scene, directory);
    }
}


void ModelImporter::ProcessMesh(Ref<SceneNode> sceneNode, const aiMesh* ai_mesh, const aiScene* ai_scene, const std::filesystem::path& directory)
{
    const auto newMesh = std::make_shared<Mesh>(ai_mesh->mName.C_Str());
    auto meshComp = sceneNode->AddComponent<MeshComponent>();
    meshComp->SetMesh(newMesh);
    
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;
    Ref<VertexBuffer> vertexBuffer;
    Ref<IndexBuffer> indexBuffer;
    auto renderingInterface = Engine::Instance().GetRenderingInterface();
    
    vertexBuffer.reset(renderingInterface->CreateVertexBuffer());
    indexBuffer.reset(renderingInterface->CreateIndexBuffer());

    vertices.reserve(ai_mesh->mNumVertices);
    indices.reserve(ai_mesh->mNumFaces * 3);

    for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
    {
        Vertex vertex{};
        vertex.position.x = ai_mesh->mVertices[i].x;
        vertex.position.y = ai_mesh->mVertices[i].y;
        vertex.position.z = ai_mesh->mVertices[i].z;

        if (ai_mesh->HasNormals())
        {
            vertex.normal.x = ai_mesh->mNormals[i].x;
            vertex.normal.y = ai_mesh->mNormals[i].y;
            vertex.normal.z = ai_mesh->mNormals[i].z;
        }

        if (ai_mesh->HasTangentsAndBitangents())
        {
            vertex.tangent.x = ai_mesh->mTangents[i].x;
            vertex.tangent.y = ai_mesh->mTangents[i].y;
            vertex.tangent.z = ai_mesh->mTangents[i].z;
        }

        // TODO: Multiple texture coordinates
        if (ai_mesh->mTextureCoords[0])
        {
            vertex.texCoords.x = ai_mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = ai_mesh->mTextureCoords[0][i].y;
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

    for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
    {
        const aiFace face = ai_mesh->mFaces[i];
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

    newMesh->ComputeBounds();
    ProcessMaterial(newMesh, ai_mesh, ai_scene, directory);
}

void ModelImporter::ProcessMaterial(Ref<Mesh> newMesh, const aiMesh* ai_mesh, const aiScene* ai_scene, const std::filesystem::path& directory)
{
    // A mesh has only one material. If an imported model uses multiple materials,
    // the importer splits up the mesh.
    const aiMaterial* material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
    const std::string materialName = material->GetName().C_Str();
    auto resourceManager = Engine::Instance().GetResourceManager();

    if (resourceManager->IsExist(typeid(Material), materialName))
    {
        newMesh->SetMaterial(resourceManager->Get<Material>(materialName));
        return;
    }

    const Ref<Material> newMat = resourceManager->Create<Material>(materialName);
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
        newMat->CreateDefaultShader(*resourceManager);
    }
    newMesh->SetMaterial(newMat);
}

void ModelImporter::ProcessTextures(Ref<Material> newMat, const aiMaterial* ai_material, const std::filesystem::path& directory)
{
    auto resourceManager = Engine::Instance().GetResourceManager();
    for (const auto& [ai_type, textureType] : s_textureTypeMap)
    {
        const auto numTexture = ai_material->GetTextureCount(ai_type);
        for (int32 i = 0; i < numTexture; ++i)
        {
            aiString path;
            ai_material->GetTexture(ai_type, i, &path);

            const std::string texturePath = (directory / path.C_Str()).string();
            if (const auto texture = resourceManager->Get<Texture>(texturePath)) {
                newMat->AddTexture(texture, textureType);
            }
        }
    }
}

void ModelImporter::ComputeBoundingVolume(Ref<SceneNode> node)
{
    SceneNode::ChildList children;
    node->GetAllDescendants(children, true);

    for (auto child : children)
    {
        auto meshComp = child->GetComponent<MeshComponent>();
        if (!meshComp) continue;

        auto mesh = meshComp->GetMesh();
        if (!mesh) continue;

        mesh->ComputeBounds();
    }
}