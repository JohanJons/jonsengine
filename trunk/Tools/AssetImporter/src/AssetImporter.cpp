#include "include/AssetImporter.h"

#include "include/Utils.h"
#include "include/Assimp.h"
#include "include/FreeImage.h"

using namespace JonsEngine;

namespace JonsAssetImporter
{
    bool Import(const std::string& packageName, const std::vector<boost::filesystem::path>& assets, const std::vector<std::string>& assetNames, Assimp& assimpImporter, FreeImage& freeimageParser);


    ParseResult ParseCommands(const Command command, std::vector<std::string>& parameters)
    {
        Assimp assimpImporter;
        FreeImage freeimageParser;
        ParseResult result = { false, GetLog() };

        if (parameters.size() <= 0)
        {
            Log("-JonsAssetImporter: ERROR: No commands given");
            return result;
        }

        enum ImportFlag
        {
            UNKNOWN_FLAG = 0,
            ASSET,
            ASSET_NAME,
            PACKAGE,
        } flag(UNKNOWN_FLAG);

        std::vector<boost::filesystem::path> assets;
        std::vector<std::string> assetNames;
        std::string package;

        for (const std::string& parameter : parameters)
        {
            if (parameter.compare("-a") == 0)
            {
                flag = ASSET;
                continue;
            }
            else if (parameter.compare("-n") == 0)
            {
                flag = ASSET_NAME;
                continue;
            }
            else if (parameter.compare("-p") == 0)
            {
                flag = PACKAGE;
                continue;
            }

            switch (flag)
            {
                case ASSET:
                {
                    assets.push_back(parameter);
                    break;
                }

                case ASSET_NAME:
                {
                    assetNames.push_back(parameter);
                    break;
                }

                case PACKAGE:
                {
                    if (!package.empty())
                    {
                        Log("-JonsAssetImporter: ERROR: Several package names given");
                        return result;
                    }

                    package = parameter;
                    if (package.size() <= 5 || (package.compare(package.size() - 5, 5, ".jons") != 0))
                        package.append(".jons");
                    break;
                }

                default:
                {
                    Log("-JonsAssetImporter: ERROR: No parameter flag set");
                    return result;
                }
            }
        }

        // verify parameters
        if (assets.size() <= 0) {
            Log("-JonsAssetImporter: ERROR: No assets supplied");
            return result;
        }

        if (package.empty()) {
            Log("-JonsAssetImporter: ERROR: No package name given");
            return result;
        }

        bool cmdResult = false;
        switch (command)
        {
            case Command::IMPORT:
            {
                cmdResult = Import(package, assets, assetNames, assimpImporter, freeimageParser);
                break;
            }

            default:
                Log("-JonsAssetImporter: ERROR: Unknown command");
        }

        result.mSuccess = cmdResult;

        return result;
    }

    bool Import(const std::string& packageName, const std::vector<boost::filesystem::path>& assets, const std::vector<std::string>& assetNames, Assimp& assimpImporter, FreeImage& freeimageParser)
    {
        JonsPackagePtr pkg;// = ReadJonsPkg(packageName);   // TODO: support opening previous package
        uint32_t materialNum = 0;
        if (!pkg)
            pkg = JonsPackagePtr(new JonsPackage());

        bool ret;
        std::vector<std::string>::const_iterator assetName = assetNames.begin();
        for (const boost::filesystem::path& asset : assets)
        {
            if (!boost::filesystem::exists(asset) || !boost::filesystem::is_regular_file(asset))
            {
                Log("-JonsAssetImporter: No such file: " + asset.string());
                continue;
            }

            switch (GetAssetType(asset.string().c_str()))
            {
                case MODEL:
                {
                    const std::string modelName = assetName != assetNames.end() ? *assetName : std::string();
                    ret = assimpImporter.ProcessScene(asset, modelName, freeimageParser, pkg);

                    break;
                }

                case MATERIAL:
                {
                    // assume diffuse texture - could it be anything else?
                    const std::string textureName = assetName != assetNames.end() ? *assetName : asset.filename().string();
                    ret = freeimageParser.ProcessMaterial(asset, textureName, TEXTURE_TYPE_DIFFUSE, pkg);

                    break;
                }

                default:
                    break;
            }

            if (assetName != assetNames.end())
                assetName++;
        }

        WriteJonsPkg(packageName, pkg);

        return ret;
    }
}