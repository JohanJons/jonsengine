#pragma once

#include "include/Utils.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Importer.hpp>

namespace JonsAssetImporter
{
    class FreeImage;

    class Assimp
    {
    public:
        Assimp();
        ~Assimp();

        bool ProcessScene(const boost::filesystem::path& modelPath, const std::string& modelName, FreeImage& freeimageImporter, JonsEngine::JonsPackagePtr pkg);


    private:
        void ProcessAssimpMaterials(const aiScene* scene, const boost::filesystem::path& modelPath, MaterialMap& materialMap, FreeImage& freeimageImporter, JonsEngine::JonsPackagePtr pkg);
        bool ProcessAssimpModel(const aiScene* scene, const std::string& modelName, const MaterialMap& materialMap, JonsEngine::JonsPackagePtr pkg);
        bool ProcessAssimpNode(std::vector<JonsEngine::PackageNode>& nodeContainer, const std::vector<JonsEngine::PackageMesh>& meshContainer, const aiScene* scene, const aiNode* assimpNode, const JonsEngine::PackageNode::NodeIndex parentNodeIndex);
        bool ProcessAssimpMeshes(std::vector<JonsEngine::PackageMesh>& meshContainer, const aiScene* scene, const MaterialMap& materialMap);
        bool ProcessAssimpBones(std::vector<JonsEngine::PackageBone>& boneContainer, const aiMesh* assimpMesh);
        bool ProcessAssimpAnimations(JonsEngine::PackageModel& model, const aiScene* scene);


        ::Assimp::Importer mImporter;
    };
}