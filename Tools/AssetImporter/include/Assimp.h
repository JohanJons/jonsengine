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
        void ProcessMaterials(const aiScene* scene, const boost::filesystem::path& modelPath, MaterialMap& materialMap, FreeImage& freeimageImporter, JonsEngine::JonsPackagePtr pkg);
        bool ProcessModel(const aiScene* scene, const std::string& modelName, const MaterialMap& materialMap, JonsEngine::JonsPackagePtr pkg);
        bool ProcessNode(std::vector<JonsEngine::PackageNode>& nodeContainer, const std::vector<JonsEngine::PackageMesh>& meshContainer, const aiScene* scene, const aiNode* assimpNode, const JonsEngine::PackageNode::NodeIndex parentNodeIndex);
        bool ProcessMeshes(std::vector<JonsEngine::PackageMesh>& meshContainer, const aiScene* scene, const MaterialMap& materialMap);
        bool ProcessMeshGeometricData(JonsEngine::PackageMesh& jonsMesh, const aiMesh* assimpMesh, const aiScene* scene, const uint32_t meshIndex);
        bool ProcessBones(std::vector<JonsEngine::PackageBone>& boneContainer, const aiMesh* assimpMesh);
        bool ProcessVertexBoneWeights(std::vector<uint8_t>& boneIndices, std::vector<float>& boneWeights, const aiMesh* assimpMesh);
        bool ProcessAnimations(JonsEngine::PackageModel& model, const aiScene* scene);


        ::Assimp::Importer mImporter;
    };
}