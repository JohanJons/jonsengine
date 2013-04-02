#pragma once

#include "include/Resources/JonsPackage.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include <vector>
#include <string>

namespace JonsAssetImporter
{
    enum ImportFlag
    {
        NONE = 0,
        ASSET,
        ASSET_NAME,
        PACKAGE,
    };

    enum ParseCmdResult
    {
        SUCCESS = 0,
        FAIL
    };


    /*
     * Commands: 'import' : <name_of_jons_pkg> [OPTIONS]
     * -a <asset_1>, ..., <asset_n> 
     * -n <asset_1_name>, ..., <asset_name_n>
     * -p <package_name>
     */
    ParseCmdResult ParseCommands(const std::vector<std::string>& cmds, std::string& errorString);
    bool Import(const std::string& packageName, const std::vector<std::string>& assets, const std::vector<std::string>& assetNames, Assimp::Importer importer);

    void ProcessScene(const aiScene* scene, const std::string& modelName, JonsEngine::JonsPackagePtr pkg);
    void ProcessMeshes(const aiScene* scene, JonsEngine::JonsPackagePtr pkg);
    JonsEngine::PackageModel ProcessModel(const aiScene* scene, const aiNode* node);
    JonsEngine::Mat4 aiMat4ToJonsMat4(aiMatrix4x4 mat);
}