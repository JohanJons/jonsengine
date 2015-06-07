#include "include/AssetImporter.h"

#include "include/Utils.h"
#include "include/Assimp.h"
#include "include/FreeImage.h"
#include <tuple>

using namespace JonsEngine;

namespace JonsAssetImporter
{
    enum class ParamType
    {
        UNKNOWN,
        MODEL,
        MATERIAL,
        SKYBOX,
        ASSET_NAME,
        PACKAGE
    };

    typedef boost::filesystem::path FilePath;
    typedef std::string AssetName;
    typedef std::tuple<ParamType, FilePath> AssetPath;
    typedef std::tuple<ParamType, FilePath, AssetName> Asset;

    bool Import(const std::string& packageName, const std::vector<Asset>& assets, Assimp& assimpImporter, FreeImage& freeimageParser);

    bool CheckForParamType(const std::string& parameter, ParamType& paramType);
    bool ParseParam(const std::string& parameter, const ParamType type, std::vector<AssetPath>& assetPaths, std::vector<AssetName>& assetNames, std::string& packageName);
    bool VerifyParams(std::vector<AssetPath>& assetPaths, std::vector<AssetName>& assetNames, std::string& packageName);
    bool MatchAssetPathAndName(std::vector<AssetPath>& assetPaths, std::vector<AssetName>& assetNames, std::vector<Asset>& assets);


    ParseResult ParseCommands(const Command command, std::vector<std::string>& parameters)
    {
        Assimp assimpImporter;
        FreeImage freeimageParser;

        if (parameters.size() <= 0)
        {
            Log("-JonsAssetImporter: ERROR: No commands given");
            return { false, GetLog() };
        }


        std::vector<AssetPath> assetPaths;
        std::vector<AssetName> assetNames;
        std::string packageName;

        // parse parameters from input
        ParamType paramType(ParamType::UNKNOWN);
        for (const std::string& parameter : parameters)
        {
            if (CheckForParamType(parameter, paramType))
                continue;

            if (!ParseParam(parameter, paramType, assetPaths, assetNames, packageName))
                return ParseResult { false, GetLog() };
        }

        // verify params
        if (!VerifyParams(assetPaths, assetNames, packageName))
            return ParseResult { false, GetLog() };

        // build asset info
        std::vector<Asset> assets;
        if (!MatchAssetPathAndName(assetPaths, assetNames, assets))
            return ParseResult{ false, GetLog() };

        bool cmdResult = false;
        switch (command)
        {
            case Command::IMPORT:
            {
                cmdResult = Import(packageName, assets, assimpImporter, freeimageParser);
                break;
            }

            default:
                Log("-JonsAssetImporter: ERROR: Unknown command");
        }

        return { cmdResult, GetLog() };
    }

    bool Import(const std::string& packageName, const std::vector<Asset>& assets, Assimp& assimpImporter, FreeImage& freeimageParser)
    {
        JonsPackagePtr pkg;// = ReadJonsPkg(packageName);   // TODO: support opening previous package
        uint32_t materialNum = 0;
        if (!pkg)
            pkg = JonsPackagePtr(new JonsPackage());

        bool ret;
        for (const Asset& asset : assets)
        {
            const auto& assetType = std::get<0>(asset);
            const auto& assetPath = std::get<1>(asset);
            const auto& assetName = std::get<2>(asset);

            switch (assetType)
            {
                case ParamType::MODEL:
                {
                    ret = assimpImporter.ProcessScene(assetPath, assetName, freeimageParser, pkg);
                    break;
                }

                case ParamType::MATERIAL:
                {
                    // TODO: assume diffuse texture - support normal maps?
                    ret = freeimageParser.ProcessMaterial(assetPath, assetName, TEXTURE_TYPE_DIFFUSE, pkg);
                    break;
                }

                case ParamType::SKYBOX:
                {
                    ret = freeimageParser.ProcessSkybox(assetPath, assetName, pkg);
                    break;
                }

                default:
                    return false;
            }

            if (!ret)
                return false;
        }

        if (ret)
            WriteJonsPkg(packageName, pkg);

        return ret;
    }


    bool CheckForParamType(const std::string& parameter, ParamType& paramType)
    {
        if (parameter.compare("-model") == 0)
        {
            paramType = ParamType::MODEL;
            return true;
        }
        else if (parameter.compare("-material") == 0)
        {
            paramType = ParamType::MATERIAL;
            return true;
        }
        else if (parameter.compare("-skybox") == 0)
        {
            paramType = ParamType::SKYBOX;
            return true;
        }
        else if (parameter.compare("-name") == 0)
        {
            paramType = ParamType::ASSET_NAME;
            return true;
        }
        else if (parameter.compare("-package") == 0)
        {
            paramType = ParamType::PACKAGE;
            return true;
        }

        return false;
    }

    bool ParseParam(const std::string& parameter, const ParamType type, std::vector<AssetPath>& assetPaths, std::vector<AssetName>& assetNames, std::string& packageName)
    {
        switch (type)
        {
            case ParamType::MODEL:
            case ParamType::MATERIAL:
            case ParamType::SKYBOX:
            {
                assetPaths.emplace_back(type, parameter);
                return true;
            }

            case ParamType::ASSET_NAME:
            {
                assetNames.push_back(parameter);
                return true;
            }

            case ParamType::PACKAGE:
            {
                if (!packageName.empty())
                {
                    Log("-JonsAssetImporter: ERROR: Several package names given");
                    return false;
                }

                packageName = parameter;
                if (packageName.size() <= 5 || (packageName.compare(packageName.size() - 5, 5, ".jons") != 0))
                    packageName.append(".jons");
                return true;
            }

            case ParamType::UNKNOWN:
            default:
            {
                Log("-JonsAssetImporter: ERROR: Bad parameter");
                return false;
            }
        }
    }

    bool VerifyParams(std::vector<AssetPath>& assetPaths, std::vector<AssetName>& assetNames, std::string& packageName)
    {
        // verify parameters
        if (assetPaths.size() <= 0) {
            Log("-JonsAssetImporter: ERROR: No assets supplied");
            return false;
        }

        if (packageName.empty()) {
            Log("-JonsAssetImporter: ERROR: No package name given");
            return false;
        }

        return true;
    }

    bool MatchAssetPathAndName(std::vector<AssetPath>& assetPaths, std::vector<AssetName>& assetNames, std::vector<Asset>& assets)
    {
        std::vector<AssetName>::const_iterator assetName = assetNames.begin();
        for (const AssetPath& path : assetPaths)
        {
            const auto& assetType = std::get<0>(path);
            const auto& assetPath = std::get<1>(path);

            if (!boost::filesystem::exists(assetPath) || !boost::filesystem::is_regular_file(assetPath))
            {
                Log("-JonsAssetImporter: No such file: " + assetPath.string());
                false;
            }

            assets.emplace_back(assetType, assetPath, assetName != assetNames.end() ? *assetName : assetPath.filename().string());

            if (assetName != assetNames.end())
                assetName++;
        }

        return true;
    }
}