#include "include/FreeImage.h"

#include "FreeImage.h"

using namespace JonsEngine;

namespace JonsAssetImporter
{
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
        PackageMaterial material;

        material.mName = textureName;
        material.mDiffuseTexture = ProcessTexture(texturePath);
        material.mHasDiffuseTexture = true;
        pkg->mMaterials.push_back(material);

        return !material.mDiffuseTexture.mTextureData.empty();
    }

    PackageTexture FreeImage::ProcessTexture(const boost::filesystem::path& assetPath)
    {
        PackageTexture texture;
        const std::string filename = assetPath.filename().string();

        FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(filename.c_str());
        if (imageFormat == FIF_UNKNOWN)
            imageFormat = FreeImage_GetFIFFromFilename(filename.c_str());

        if (imageFormat == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(imageFormat))
        {
            Log("-JonsAssetImporter: Unable to open file: " + filename);
            return texture;
        }

        FIBITMAP* bitmap = FreeImage_Load(imageFormat, assetPath.string().c_str());
        if (!bitmap || !FreeImage_GetBits(bitmap) || !FreeImage_GetWidth(bitmap) || !FreeImage_GetHeight(bitmap))
        {
            Log("-JonsAssetImporter: Invalid image data: " + filename);
            return texture;
        }

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

        return texture;
    }


    void FreeImageErrorHandler(FREE_IMAGE_FORMAT imageFormat, const char* message)
    {
        Log(message);
    }
}