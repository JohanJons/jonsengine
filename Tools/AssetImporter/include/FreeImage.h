#pragma once

#include "include/Utils.h"
#include "include/Resources/JonsPackage.h"

#include <string>

namespace JonsAssetImporter
{
    struct FIBITMAP;

    class FreeImage
    {
    public:
        FreeImage();
        ~FreeImage();

        bool ProcessMaterial(const boost::filesystem::path& assetPath, const std::string& textureName, const JonsEngine::TextureType textureType, JonsEngine::JonsPackagePtr pkg);
        bool ProcessSkybox(const boost::filesystem::path& assetPath, const std::string& textureName, JonsEngine::JonsPackagePtr pkg);
        bool ProcessTexture(JonsEngine::PackageTexture& texture, const boost::filesystem::path& assetPath);
        

    private:
        bool ProcessTexture(JonsEngine::PackageTexture& texture, FIBITMAP* bitmap);
        bool ProcessTexture(JonsEngine::PackageTexture& texture, FIBITMAP* bitmap, const uint32_t offsetWidth, const uint32_t offsetHeight, const uint32_t width, const uint32_t height);
    };
}