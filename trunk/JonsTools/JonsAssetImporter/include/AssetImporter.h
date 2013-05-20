#pragma once

#include "include/Resources/JonsPackage.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include "boost/filesystem.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "FreeImage.h"
#include <vector>
#include <string>
#include <map>

namespace JonsAssetImporter
{
    class AssetImporter
    {
    public:
        /* ParseCmdResult definition */
        enum ParseCmdResult
        {
            SUCCESS = 0,
            FAIL
        };


        AssetImporter();
        ~AssetImporter();

        /*
         * Commands: 'import' : <name_of_jons_pkg> [OPTIONS]
         * -a <asset_1>, ..., <asset_n> 
         * -n <asset_1_name>, ..., <asset_name_n>
         * -p <package_name>
         */
        ParseCmdResult ParseCommands(const std::vector<std::string>& cmds);

        const std::string& GetErrorLog() const;


    private:
        /* ImportFlag definition */
        enum ImportFlag
        {
            NONE = 0,
            ASSET,
            ASSET_NAME,
            PACKAGE,
        };

        /* MeshMaterialMap definition */
        typedef std::map<uint32_t, uint32_t> MaterialMap;       // assimp material index - to - JonsPackage material index
        typedef std::pair<uint32_t, uint32_t> MaterialPair;


        bool Import(const std::string& packageName, const std::vector<boost::filesystem::path>& assets, const std::vector<std::string>& assetNames, Assimp::Importer importer);

        void ProcessScene(const aiScene* scene, const boost::filesystem::path& modelPath, const std::string& modelName, JonsEngine::JonsPackagePtr pkg);
        void ProcessMaterials(const aiScene* scene, const boost::filesystem::path& modelPath, MaterialMap& materialMap, JonsEngine::JonsPackagePtr pkg);
        JonsEngine::PackageModel ProcessModel(const aiScene* scene, const aiNode* node, const MaterialMap& materialMap);
        JonsEngine::PackageTexture ProcessDiffuseTexture(const aiMaterial* material, const boost::filesystem::path& modelPath); 

        JonsEngine::Mat4 aiMat4ToJonsMat4(const aiMatrix4x4& mat);
        JonsEngine::Vec3 aiColor3DToJonsVec3(const aiColor3D& color);
        void Log(const std::string& msg);
        static void FreeImageErrorHandler(FREE_IMAGE_FORMAT imageFormat, const char* message);


        std::string mErrorLog;
    };
}