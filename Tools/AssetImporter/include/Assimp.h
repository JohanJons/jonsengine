#pragma once

#include "include/Utils.h"
#include "include/Resources/JonsPackage.h"
#include "include/Resources/Animation.h"
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
        bool ParseModel(const aiScene* scene, const std::string& modelName, const MaterialMap& materialMap, JonsEngine::JonsPackagePtr pkg);
        bool ParseNodeHeirarchy(std::vector<JonsEngine::PackageNode>& nodeContainer, const std::vector<JonsEngine::PackageMesh>& meshContainer, const aiScene* scene, const aiNode* assimpNode,
            const JonsEngine::PackageNode::NodeIndex parentNodeIndex, const JonsEngine::Mat4& parentTransform);
        bool ProcessMeshes(std::vector<JonsEngine::PackageMesh>& meshContainer, const aiScene* scene, const MaterialMap& materialMap, JonsEngine::Vec3& modelMinBounds, JonsEngine::Vec3& modelMaxBounds);
        bool AddMeshGeometricData(JonsEngine::PackageMesh& jonsMesh, const aiMesh* assimpMesh, const aiScene* scene, const uint32_t meshIndex, JonsEngine::Vec3& modelMinBounds, JonsEngine::Vec3& modelMaxBounds);
		bool ProcessBones(JonsEngine::BoneParentMap& parentMap, std::vector<JonsEngine::PackageBone>& bones, std::vector<JonsEngine::PackageMesh>& meshes, const aiScene* scene);
        bool ProcessVertexBoneWeights(std::vector<JonsEngine::PackageBone>& bones, std::vector<JonsEngine::PackageMesh>& meshes, const aiScene* scene);
        bool ProcessAnimations(JonsEngine::PackageModel& model, const aiScene* scene);


        ::Assimp::Importer mImporter;
    };
}