#pragma once

#include "Resources/JonsPackage.h"
#include "Utils.h"

#include <string>

struct FIBITMAP;

namespace JonsAssetImporter
{
    class FreeImage
    {
    public:
        FreeImage();
        ~FreeImage();

        bool ProcessSkybox(const boost::filesystem::path& assetPath, const std::string& textureName, JonsEngine::JonsPackagePtr pkg);
        bool ProcessTexture2D(const boost::filesystem::path& assetPath, const std::string& textureName, const JonsEngine::TextureType textureType, JonsEngine::JonsPackagePtr pkg);
        

    private:
        bool ProcessTexture(JonsEngine::PackageTexture& texture, FIBITMAP* bitmap, JonsEngine::TextureType textureType );
        bool ProcessTextureRGB(JonsEngine::PackageTexture& texture, FIBITMAP* bitmap, const uint32_t offsetWidth, const uint32_t offsetHeight, const uint32_t width, const uint32_t height);
		bool ProcessTextureGreyscale( JonsEngine::PackageTexture& texture, FIBITMAP* bitmap, const uint32_t offsetWidth, const uint32_t offsetHeight, const uint32_t width, const uint32_t height );
    };
}