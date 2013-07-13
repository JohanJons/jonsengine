#pragma once

#include "include/Resources/JonsPackage.h"
#include "include/Core/EngineDefs.h"

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
        enum Command
        {
            UNKNOWN_COMMAND = 0,
            IMPORT,
        };

        AssetImporter();
        ~AssetImporter();

        /*
         * Commands: 'import' : <name_of_jons_pkg> [OPTIONS]
         * -a <asset_1>, ..., <asset_n> 
         * -n <asset_1_name>, ..., <asset_name_n>
         * -p <package_name>
         */
        bool ParseCommands(const Command command,  std::vector<std::string>& parameters);

        const std::string& GetErrorLog() const;


    private:
        /* ImportFlag definition */
        enum ImportFlag
        {
            UNKNOWN_FLAG = 0,
            ASSET,
            ASSET_NAME,
            PACKAGE,
        };

        enum AssetType
        {
            UNKNOWN_TYPE = 0,
            MODEL,
            MATERIAL,
        };

        /* MeshMaterialMap definition */
        typedef std::map<uint32_t, uint32_t> MaterialMap;       // assimp material index - to - JonsPackage material index
        typedef std::pair<uint32_t, uint32_t> MaterialPair;


        bool Import(const std::string& packageName, const std::vector<boost::filesystem::path>& assets, const std::vector<std::string>& assetNames, Assimp::Importer importer);

        void ProcessScene(const aiScene* scene, const boost::filesystem::path& modelPath, const std::string& modelName, JonsEngine::JonsPackagePtr pkg);
        void ProcessAssimpMaterials(const aiScene* scene, const boost::filesystem::path& modelPath, MaterialMap& materialMap, JonsEngine::JonsPackagePtr pkg);
        JonsEngine::PackageModel ProcessAssimpModelGeometry(const aiScene* scene, const aiNode* node, const MaterialMap& materialMap);
        JonsEngine::PackageTexture ProcessDiffuseTexture(const boost::filesystem::path& assetPath); 

        AssetType GetAssetType(const char* textureName) const;
        std::string GetDefaultAssetName(AssetType assetType, uint32_t assetTypeNumber) const;
        JonsEngine::Mat4 aiMat4ToJonsMat4(const aiMatrix4x4& mat) const;
        JonsEngine::Vec3 aiColor3DToJonsVec3(const aiColor3D& color) const;
        void Log(const std::string& msg);
        static void FreeImageErrorHandler(FREE_IMAGE_FORMAT imageFormat, const char* message);

        std::string mErrorLog;
    };
}