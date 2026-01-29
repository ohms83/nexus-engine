#pragma once

#include "nexus/NxsDefine.h"

#include <filesystem>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

NXS_NAMESPACE
{
    class SceneNode;
    class Mesh;
    class Material;

    class ModelImporter
    {
    public:
        ModelImporter() = default;

        static Ref<SceneNode> Import(const std::string& path);

    private:
        static void ProcessNode(Ref<SceneNode> thisNode, const aiNode* ai_node, const aiScene* ai_scene, const std::filesystem::path& directory);
        static void ProcessMesh(Ref<SceneNode> sceneNode, const aiMesh* ai_mesh, const aiScene* ai_scene, const std::filesystem::path& directory);
        static void ProcessMaterial(Ref<Mesh> newMesh, const aiMesh* ai_mesh, const aiScene* ai_scene, const std::filesystem::path& directory);
        static void ProcessTextures(Ref<Material> newMat, const aiMaterial* ai_material, const std::filesystem::path& directory);
        static void ComputeBoundingVolume(Ref<SceneNode> model);
    };
}