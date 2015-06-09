#include "include/FreeImage.h"

#include "FreeImage.h"

using namespace JonsEngine;

namespace JonsAssetImporter
{
    FIBITMAP* GetBitmap(const std::string& filePath, const std::string& filename);
    void FreeImageErrorHandler(FREE_IMAGE_FORMAT imageFormat, const char* message);


    FreeImage::FreeImage()
    {
        FreeImage_Initialise(false);
        FreeImage_SetOutputMessage(FreeImageErrorHandler);
    }

    FreeImage::~FreeImage()
    {
        FreeImage_DeInitialise();
        FreeImage_SetOutputMessage(NULL);
    }


    bool FreeImage::ProcessMaterial(const boost::filesystem::path& texturePath, const std::string& textureName, const JonsEngine::TextureType textureType, JonsEngine::JonsPackagePtr pkg)
    {
        pkg->mMaterials.emplace_back(textureName, textureType == JonsEngine::TEXTURE_TYPE_DIFFUSE, textureType == JonsEngine::TEXTURE_TYPE_NORMAL);

        auto& material = pkg->mMaterials.back();
        if (!ProcessTexture(material.mDiffuseTexture, texturePath))
            return false;

        return true;
    }

    bool FreeImage::ProcessSkybox(const boost::filesystem::path& texturePath, const std::string& textureName, JonsEngine::JonsPackagePtr pkg)
    {
        pkg->mSkyBoxes.emplace_back(textureName);

        auto& skybox = pkg->mSkyBoxes.back();
        const uint32_t numTextures = 6;
        for (uint32_t index = 0; index < numTextures; ++index)
        {
            if (!ProcessTexture(skybox.mTextures.at(index), texturePath))
                return false;
        }

        return true;
    }

    bool FreeImage::ProcessTexture(JonsEngine::PackageTexture& texture, const boost::filesystem::path& assetPath)
    {
        const std::string fileName = assetPath.filename().string();
        const std::string filePath = assetPath.string();

        FIBITMAP* bitmap = GetBitmap(filePath, fileName);
        if (!bitmap)
            return false;

        FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(bitmap);
        uint32_t bytesPerPixel = FreeImage_GetBPP(bitmap) / 8;
        uint32_t widthInPixels = FreeImage_GetWidth(bitmap);
        uint32_t heightInPixels = FreeImage_GetHeight(bitmap);

        // should be OK now
        texture.mName = assetPath.filename().string();
        texture.mTextureWidth = widthInPixels;
        texture.mTextureHeight = heightInPixels;
        // FreeImage implicitly converts image format to RGB/RGBA from BRG/BRGA

        for (unsigned y = 0; y < FreeImage_GetHeight(bitmap); y++) {
            BYTE *bits = FreeImage_GetScanLine(bitmap, y);

            for (unsigned x = 0; x < FreeImage_GetWidth(bitmap); x++) {
                texture.mTextureData.push_back(bits[FI_RGBA_RED]);
                texture.mTextureData.push_back(bits[FI_RGBA_GREEN]);
                texture.mTextureData.push_back(bits[FI_RGBA_BLUE]);
                if (colorType == FIC_RGBALPHA)
                    texture.mTextureData.push_back(bits[FI_RGBA_ALPHA]);
                else
                    texture.mTextureData.push_back(0);

                bits += bytesPerPixel;
            }
        }

        FreeImage_Unload(bitmap);

        return true;
    }

    bool ProcessTexture(JonsEngine::PackageTexture& texture, const boost::filesystem::path& assetPath, const uint32_t offsetWidth, const uint32_t offsetHeight, const uint32_t width, const uint32_t height)
    {
        const std::string fileName = assetPath.filename().string();
        const std::string filePath = assetPath.string();

        FIBITMAP* bitmap = GetBitmap(filePath, fileName);
        if (!bitmap)
            return false;

        FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(bitmap);
        uint32_t bytesPerPixel = FreeImage_GetBPP(bitmap) / 8;
        uint32_t widthInPixels = FreeImage_GetWidth(bitmap);
        uint32_t heightInPixels = FreeImage_GetHeight(bitmap);

        texture.mName = assetPath.filename().string();
        texture.mTextureWidth = width;
        texture.mTextureHeight = height;

        return true;
    }


    FIBITMAP* GetBitmap(const std::string& filePath, const std::string& filename)
    {
        FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(filename.c_str());
        if (imageFormat == FIF_UNKNOWN)
            imageFormat = FreeImage_GetFIFFromFilename(filename.c_str());

        if (imageFormat == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(imageFormat))
        {
            Log("ERROR: Unable to open file: " + filename);
            return nullptr;
        }

        FIBITMAP* bitmap = FreeImage_Load(imageFormat, filePath.c_str());
        if (!bitmap || !FreeImage_GetBits(bitmap) || !FreeImage_GetWidth(bitmap) || !FreeImage_GetHeight(bitmap))
        {
            Log("ERROR: Invalid image data: " + filename);
            return nullptr;
        }

        return bitmap;
    }

    void FreeImageErrorHandler(FREE_IMAGE_FORMAT imageFormat, const char* message)
    {
        Log(message);
    }
}