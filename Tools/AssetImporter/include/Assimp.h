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
        bool ProcessAssimpNode(JonsEngine::PackageModel& pkgModel, JonsEngine::PackageNode& pkgNode, const aiScene* scene, const aiNode* node, const MaterialMap& materialMap, const JonsEngine::Mat4& parentTransform, JonsEngine::Vec3& modelMinBounds,
            JonsEngine::Vec3& modelMaxBounds, const JonsEngine::PackageNode::NodeIndex nextNodeIndex, const JonsEngine::PackageNode::NodeIndex parentNodeIndex);
        bool ProcessAssimpMesh(JonsEngine::PackageMesh& pkgMesh, const aiMesh* mesh, const MaterialMap& materialMap, const JonsEngine::Mat4& nodeTransform, JonsEngine::Vec3& nodeMinBounds, JonsEngine::Vec3& nodeMaxBounds);
        bool ProcessAssimpAnimations(JonsEngine::PackageModel& model, const aiScene* scene);


        ::Assimp::Importer mImporter;
    };
}