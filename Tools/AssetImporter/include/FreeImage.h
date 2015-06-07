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
        JonsEngine::PackageTexture ProcessTexture(const boost::filesystem::path& texturePath);
    };
}