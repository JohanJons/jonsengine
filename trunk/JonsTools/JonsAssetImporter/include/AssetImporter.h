#pragma once

#include "include/Resources/JonsPackage.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include <vector>
#include <string>

namespace JonsAssetImporter
{
    /*
     * Commands: 'import' : <name_of_jons_pkg> <asset_1>, ..., <asset_n>
     */
    std::string ParseCommands(const std::vector<std::string>& cmds);
    bool Import(const std::string& jonsPkgName, const std::vector<std::string>::const_iterator assetsCurrent, const std::vector<std::string>::const_iterator assetsEnd, Assimp::Importer importer);

    void ProcessScene(const aiScene* scene, JonsEngine::JonsPackagePtr pkg);
    void ProcessMeshes(const aiScene* scene, JonsEngine::JonsPackagePtr pkg);
    JonsEngine::PackageModel ProcessModel(const aiScene* scene, const aiNode* node);
    JonsEngine::Mat4 aiMat4ToJonsMat4(aiMatrix4x4 mat);
}