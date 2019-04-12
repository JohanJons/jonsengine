#pragma once

#include "Utils.h"
#include "JonsPackage.h"
#include "Animation.h"
#include "Types.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Importer.hpp>

namespace JonsAssetImporter
{
    class FreeImage;

	// <pkgName, assimpMeshIndex>
	typedef std::map<std::string, uint32_t> MeshNameMap;

    class Assimp
    {
    public:
        Assimp();
        ~Assimp();

        bool ProcessScene(const boost::filesystem::path& modelPath, const std::string& modelName, FreeImage& freeimageImporter, JonsEngine::JonsPackagePtr pkg);


    private:
        bool ProcessMaterials(std::vector<JonsEngine::PackageMaterial>& materials, const aiScene* scene, const boost::filesystem::path& modelPath, MaterialMap& materialMap, FreeImage& freeimageImporter, JonsEngine::JonsPackagePtr pkg);
        bool ParseModel(const aiScene* scene, const std::string& modelName, const MaterialMap& materialMap, JonsEngine::JonsPackagePtr pkg);
        bool ParseNodeHeirarchy(std::vector<JonsEngine::PackageNode>& nodeContainer, const std::vector<JonsEngine::PackageMesh>& meshContainer, const aiScene* scene, const aiNode* assimpNode,
            const JonsEngine::PackageNode::NodeIndex parentNodeIndex, const JonsEngine::Mat4& parentTransform);
        bool ProcessMeshes(std::vector<JonsEngine::PackageMesh>& meshContainer, const aiScene* scene, const MaterialMap& materialMap, MeshNameMap& meshNameMap, JonsEngine::Vec3& modelMinBounds, JonsEngine::Vec3& modelMaxBounds);
        bool AddMeshGeometricData(JonsEngine::PackageMesh& jonsMesh, const aiMesh* assimpMesh, const aiScene* scene, const uint32_t meshIndex);
		bool ProcessBones(JonsEngine::BoneParentMap& parentMap, std::vector<JonsEngine::PackageBone>& bones, std::vector<JonsEngine::PackageMesh>& meshes, const MeshNameMap& meshNameMap, const aiScene* scene);
        bool ProcessVertexBoneWeights(std::vector<JonsEngine::PackageBone>& bones, std::vector<JonsEngine::PackageMesh>& meshes, const MeshNameMap& meshNameMap, const aiScene* scene);
        bool ProcessAnimations(JonsEngine::PackageModel& model, const aiScene* scene);


        ::Assimp::Importer mImporter;
    };
}