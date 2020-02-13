#include "AssetImporter.h"

#include "Utils.h"
#include "Assimp.h"
#include "FreeImageParser.h"

using namespace JonsEngine;

namespace JonsAssetImporter
{
    enum class ParamType
    {
        Unknown,
        Model,
		Texture_Diffuse,
		Texture_Normal,
		Texture_Height8,
		Texture_Height16,
        Skybox,
        AssetName,
		Package
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
	bool VerifyNoDuplicateTextureNames(const JonsPackagePtr pkg);


    ParseResult ParseCommands(const Command command, std::vector<std::string>& parameters)
    {
        Assimp assimpImporter;
        FreeImage freeimageParser;

        if (parameters.size() <= 0)
        {
            Log("ERROR: No commands given");
            return { false, GetLog() };
        }


        std::vector<AssetPath> assetPaths;
        std::vector<AssetName> assetNames;
        std::string packageName;

        // parse parameters from input
        ParamType paramType(ParamType::Unknown);
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
                Log("ERROR: Unknown command");
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
				case ParamType::Model:
                {
                    ret = assimpImporter.ProcessScene(assetPath, assetName, freeimageParser, pkg);
                    break;
                }

				case ParamType::Texture_Diffuse:
				case ParamType::Texture_Normal:
				case ParamType::Texture_Height8:
				case ParamType::Texture_Height16:
                {
					TextureType type;
					switch (assetType)
					{
						default:
						case ParamType::Texture_Diffuse: type = TextureType::Diffuse; break;
						case ParamType::Texture_Normal:  type = TextureType::Normal; break;
						case ParamType::Texture_Height8: type = TextureType::Height8; break;
						case ParamType::Texture_Height16: type = TextureType::Height16; break;
					}

					ret = freeimageParser.ProcessTexture2D(assetPath, assetName, type, pkg);

                    break;
                }

                case ParamType::Skybox:
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

		if (!VerifyNoDuplicateTextureNames(pkg))
			return false;

        WriteJonsPkg(packageName, pkg);

        return ret;
    }


    bool CheckForParamType(const std::string& parameter, ParamType& paramType)
    {
        if (parameter == "-model")
        {
            paramType = ParamType::Model;
            return true;
        }
		else if (parameter == "-texture_diffuse")
		{
			paramType = ParamType::Texture_Diffuse;
			return true;
		}
		else if (parameter == "-texture_normal")
		{
			paramType = ParamType::Texture_Normal;
			return true;
		}
		else if (parameter == "-texture_height8")
		{
			paramType = ParamType::Texture_Height8;
			return true;
		}
		else if ( parameter == "-texture_height16" )
		{
			paramType = ParamType::Texture_Height16;
			return true;
		}
        else if (parameter == "-skybox")
        {
            paramType = ParamType::Skybox;
            return true;
        }
        else if (parameter == "-name")
        {
            paramType = ParamType::AssetName;
            return true;
        }
        else if (parameter == "-package")
        {
            paramType = ParamType::Package;
            return true;
        }

        return false;
    }

    bool ParseParam(const std::string& parameter, const ParamType type, std::vector<AssetPath>& assetPaths, std::vector<AssetName>& assetNames, std::string& packageName)
    {
        switch (type)
        {
            case ParamType::Model:
            case ParamType::Texture_Diffuse:
			case ParamType::Texture_Normal:
			case ParamType::Texture_Height8:
			case ParamType::Texture_Height16:
            case ParamType::Skybox:
            {
                assetPaths.emplace_back(type, parameter);
                return true;
            }

            case ParamType::AssetName:
            {
                assetNames.push_back(parameter);
                return true;
            }

            case ParamType::Package:
            {
                if (!packageName.empty())
                {
                    Log("ERROR: Several package names given");
                    return false;
                }

                packageName = parameter;
                if (packageName.size() <= 5 || (packageName.compare(packageName.size() - 5, 5, ".jons") != 0))
                    packageName.append(".jons");
                return true;
            }

            case ParamType::Unknown:
            default:
            {
                Log("ERROR: Bad parameter");
                return false;
            }
        }
    }

    bool VerifyParams(std::vector<AssetPath>& assetPaths, std::vector<AssetName>& assetNames, std::string& packageName)
    {
        // verify parameters
        if (assetPaths.size() <= 0) {
            Log("ERROR: No assets supplied");
            return false;
        }

        if (packageName.empty()) {
            Log("ERROR: No package name given");
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
                Log("ERROR: No such file: " + assetPath.string());
                false;
            }

			assert(assetPath.has_extension());

            assets.emplace_back(assetType, assetPath, assetName != assetNames.end() ? *assetName : assetPath.filename().string());

            if (assetName != assetNames.end())
                assetName++;
        }

        return true;
    }

	bool VerifyNoDuplicateTextureNames(const JonsPackagePtr pkg)
	{
		auto texturesCopy = pkg->mTextures;
		std::sort(texturesCopy.begin(), texturesCopy.end(), [](const auto& text1, const auto& text2) { return text1.mName < text2.mName; });
		auto iter = std::adjacent_find(texturesCopy.begin(), texturesCopy.end(), [](const auto& text1, const auto& text2) { return text1.mName == text2.mName; });

		return iter == texturesCopy.end();
	}
}