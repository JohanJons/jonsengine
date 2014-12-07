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
        JonsEngine::PackageModel ProcessAssimpModelGeometry(const aiScene* scene, const aiNode* node, const MaterialMap& materialMap);
        JonsEngine::PackageNode ProcessAssimpNode(const aiScene* scene, const aiNode* node, const MaterialMap& materialMap, JonsEngine::Vec3& modelMinBounds, JonsEngine::Vec3& modelMaxBounds);
        JonsEngine::PackageMesh ProcessAssimpMesh(const aiMesh* mesh, const MaterialMap& materialMap, JonsEngine::Vec3& nodeMinBounds, JonsEngine::Vec3& nodeMaxBounds);

        ::Assimp::Importer mImporter;
    };
}