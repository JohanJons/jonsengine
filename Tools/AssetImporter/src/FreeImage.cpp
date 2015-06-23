#include "include/FreeImage.h"

#include "FreeImage.h"
#include <utility>

using namespace JonsEngine;

namespace JonsAssetImporter
{
    FIBITMAP* GetBitmap(const std::string& filePath, const std::string& filename);
    void FreeImageErrorHandler(FREE_IMAGE_FORMAT imageFormat, const char* message);

    // [width, height] offsets per face [+x, -x, +y, -y, +z, -z]
    static const std::array<std::pair<uint32_t, uint32_t>, 6> gSkyboxHorizontalOffsets = { { { 0, 1 }, { 2, 1 }, { 1, 2 }, { 1, 0 }, { 1, 1 }, { 3, 1 } } };
    static const std::array<std::pair<uint32_t, uint32_t>, 6> gSkyboxVerticalOffsets =   { { { 0, 2 }, { 2, 2 }, { 1, 3 }, { 1, 1 }, { 1, 2 }, { 1, 0 } } };


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


    bool FreeImage::ProcessMaterial(const boost::filesystem::path& assetPath, const std::string& textureName, const JonsEngine::TextureType textureType, JonsEngine::JonsPackagePtr pkg)
    {
        pkg->mMaterials.emplace_back(textureName, textureType == JonsEngine::TEXTURE_TYPE_DIFFUSE, textureType == JonsEngine::TEXTURE_TYPE_NORMAL);

        const std::string fileName = assetPath.filename().string();
        const std::string filePath = assetPath.string();

        FIBITMAP* bitmap = GetBitmap(filePath, fileName);
        if (!bitmap)
            return false;

        auto& material = pkg->mMaterials.back();
        if (!ProcessTexture(material.mDiffuseTexture, bitmap))
            return false;

        FreeImage_Unload(bitmap);

        return true;
    }

    bool FreeImage::ProcessSkybox(const boost::filesystem::path& assetPath, const std::string& textureName, JonsEngine::JonsPackagePtr pkg)
    {
        pkg->mSkyBoxes.emplace_back(textureName);

        const std::string fileName = assetPath.filename().string();
        const std::string filePath = assetPath.string();

        FIBITMAP* bitmap = GetBitmap(filePath, fileName);
        if (!bitmap)
            return false;

        const uint32_t widthInPixels = FreeImage_GetWidth(bitmap);
        const uint32_t heightInPixels = FreeImage_GetHeight(bitmap);
        const bool isHorizontalSkybox = widthInPixels > heightInPixels;
        const uint32_t numColumns = isHorizontalSkybox ? 4 : 3;
        const uint32_t numRows = isHorizontalSkybox ? 3 : 4;

        const uint32_t textureWidth = widthInPixels / numColumns;
        const uint32_t textureHeight = heightInPixels / numRows;

        // skybox texture format:
        // 0 1 0      0 1 0 0
        // 1 1 1  or  1 1 1 1
        // 0 1 0      0 1 0 0
        // 0 1 0
        auto& skybox = pkg->mSkyBoxes.back();
        const auto& skyboxOffsets = isHorizontalSkybox ? gSkyboxHorizontalOffsets : gSkyboxVerticalOffsets;
        for (const auto& offsets : skyboxOffsets)
        {
            if (!ProcessTexture(skybox.mSkyboxTexture, bitmap, offsets.first * textureWidth, offsets.second * textureHeight, textureWidth, textureHeight))
                return false;
        }

        // width/height per subtexture
        skybox.mSkyboxTexture.mTextureWidth = textureWidth;
        skybox.mSkyboxTexture.mTextureHeight = textureHeight;

        FreeImage_Unload(bitmap);

        return true;
    }

    bool FreeImage::ProcessTexture(JonsEngine::PackageTexture& texture, const boost::filesystem::path& assetPath)
    {
        const std::string fileName = assetPath.filename().string();
        const std::string filePath = assetPath.string();

        FIBITMAP* bitmap = GetBitmap(filePath, fileName);
        if (!bitmap)
            return false;

        const bool ret = ProcessTexture(texture, bitmap);

        FreeImage_Unload(bitmap);

        return ret;
    }

    bool FreeImage::ProcessTexture(JonsEngine::PackageTexture& texture, FIBITMAP* bitmap)
    {
        const uint32_t widthInPixels = FreeImage_GetWidth(bitmap);
        const uint32_t heightInPixels = FreeImage_GetHeight(bitmap);

        return ProcessTexture(texture, bitmap, 0, 0, widthInPixels, heightInPixels);
    }

    bool FreeImage::ProcessTexture(JonsEngine::PackageTexture& texture, FIBITMAP* bitmap, const uint32_t offsetWidth, const uint32_t offsetHeight, const uint32_t width, const uint32_t height)
    {
        assert(bitmap != nullptr);

        const FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(bitmap);
        const uint32_t bytesPerPixel = FreeImage_GetBPP(bitmap) / 8;

        // NOTE: only R8B8G8A8 textures supported in JonsEngine
        // R8B8G8 or R8B8G8A8
        assert(bytesPerPixel == 3 || bytesPerPixel == 4);

        texture.mTextureWidth = width;
        texture.mTextureHeight = height;

        // NOTE: FreeImage implicitly converts image format to RGB/RGBA from BRG/BRGA
        for (unsigned y = offsetHeight; y < offsetHeight + height; ++y) {
            BYTE *bits = FreeImage_GetScanLine(bitmap, y);
            bits += (offsetWidth * bytesPerPixel);

            for (unsigned x = 0; x < width; ++x) {
                texture.mTextureData.push_back(bits[FI_RGBA_RED]);
                texture.mTextureData.push_back(bits[FI_RGBA_GREEN]);
                texture.mTextureData.push_back(bits[FI_RGBA_BLUE]);
                if (colorType == FIC_RGBALPHA)
                    texture.mTextureData.push_back(bits[FI_RGBA_ALPHA]);
                else
                    // this could maybe be optimized for textures without alpha components
                    texture.mTextureData.push_back(0);

                bits += bytesPerPixel;
            }
        }

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






