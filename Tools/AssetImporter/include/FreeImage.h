#pragma once

#include "include/Utils.h"
#include "include/Resources/JonsPackage.h"

#include <string>

namespace JonsAssetImporter
{
    class FreeImage
    {
    public:
        FreeImage();
        ~FreeImage();

        bool ProcessMaterial(const boost::filesystem::path& texturePath, const std::string& textureName, const JonsEngine::TextureType textureType, JonsEngine::JonsPackagePtr pkg);
        bool ProcessSkybox(const boost::filesystem::path& texturePath, const std::string& textureName, JonsEngine::JonsPackagePtr pkg);
        bool ProcessTexture(JonsEngine::PackageTexture& texture, const boost::filesystem::path& assetPath);
        bool ProcessTexture(JonsEngine::PackageTexture& texture, const boost::filesystem::path& assetPath, const uint32_t offsetWidth, const uint32_t offsetHeight, const uint32_t width, const uint32_t height);


    private:

    };
}