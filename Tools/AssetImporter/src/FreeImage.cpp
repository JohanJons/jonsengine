#include "include/FreeImage.h"

#include "FreeImage.h"
#include <utility>

using namespace JonsEngine;

namespace JonsAssetImporter
{
    FIBITMAP* GetBitmap(const std::string& filePath, const std::string& filename);
    void FreeImageErrorHandler(FREE_IMAGE_FORMAT imageFormat, const char* message);

    // [width, height] offsets per face [+x, -x, +y, -y, +z, -z]
    static const std::array<std::pair<uint32_t, uint32_t>, 6> gSkyboxHorizontalOffsets = { { { 2, 2 }, { 0, 2 }, { 1, 3 }, { 1, 1 }, { 1, 2 }, { 3, 2 } } };
    static const std::array<std::pair<uint32_t, uint32_t>, 6> gSkyboxVerticalOffsets =   { { { 2, 3 }, { 0, 3 }, { 1, 4 }, { 1, 2 }, { 1, 3 }, { 1, 1 } } };


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


    bool FreeImage::ProcessSkybox(const boost::filesystem::path& assetPath, const std::string& textureName, JonsEngine::JonsPackagePtr pkg)
    {
        pkg->mTextures.emplace_back(textureName, TextureType::Skybox);

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
        auto& skybox = pkg->mTextures.back();
        const auto& skyboxOffsets = isHorizontalSkybox ? gSkyboxHorizontalOffsets : gSkyboxVerticalOffsets;
        for (const auto& offsets : skyboxOffsets)
        {
            if (!ProcessTextureRGB(skybox, bitmap, offsets.first * textureWidth, (offsets.second * textureHeight) - 1, textureWidth, textureHeight))
                return false;
        }

        // width/height per subtexture
        skybox.mTextureWidth = textureWidth;
        skybox.mTextureHeight = textureHeight;

        FreeImage_Unload(bitmap);

        return true;
    }

	bool FreeImage::ProcessTexture2D(const boost::filesystem::path& assetPath, const std::string& textureName, const TextureType textureType, JonsPackagePtr pkg)
	{
		assert(textureType != TextureType::Skybox);

		const std::string fileName = assetPath.filename().string();
		const std::string filePath = assetPath.string();

		FIBITMAP* bitmap = GetBitmap(filePath, fileName);
		if (!bitmap)
			return false;

		pkg->mTextures.emplace_back(textureName, textureType);
		auto& texture = pkg->mTextures.back();

		const bool ret = ProcessTexture( texture, bitmap, textureType );

		FreeImage_Unload(bitmap);

		return ret;
	}


    bool FreeImage::ProcessTexture( PackageTexture& texture, FIBITMAP* bitmap, TextureType textureType )
    {
        const uint32_t widthInPixels = FreeImage_GetWidth(bitmap);
        const uint32_t heightInPixels = FreeImage_GetHeight(bitmap);

		texture.mTextureWidth = widthInPixels;
		texture.mTextureHeight = heightInPixels;

		switch ( textureType )
		{
			case TextureType::Diffuse:
			case TextureType::Normal:	// TODO: normals into R8...
			case TextureType::Skybox:
				return ProcessTextureRGB( texture, bitmap, 0, heightInPixels - 1, widthInPixels, heightInPixels );
			case TextureType::Height:
				return ProcessTextureGreyscale( texture, bitmap, 0, heightInPixels - 1, widthInPixels, heightInPixels );
			default:
				break;
		}

		return false;
    }

    bool FreeImage::ProcessTextureRGB(JonsEngine::PackageTexture& texture, FIBITMAP* bitmap, const uint32_t offsetWidth, const uint32_t offsetHeight, const uint32_t width, const uint32_t height)
    {
        assert( bitmap );

        const FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(bitmap);
        const uint32_t bytesPerPixel = FreeImage_GetBPP(bitmap) / 8;

        // R8B8G8 or R8B8G8A8
        assert(bytesPerPixel == 3 || bytesPerPixel == 4);

        // NOTE: FreeImage implicitly converts image format to RGB/RGBA from BRG/BRGA
        for (unsigned y = 0; y < height; ++y) {
            BYTE *bits = FreeImage_GetScanLine(bitmap, offsetHeight - y);
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

	bool FreeImage::ProcessTextureGreyscale( JonsEngine::PackageTexture& texture, FIBITMAP* bitmap, const uint32_t offsetWidth, const uint32_t offsetHeight, const uint32_t width, const uint32_t height )
	{
		assert( bitmap );

		const FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType( bitmap );
		const uint32_t bytesPerPixel = FreeImage_GetBPP( bitmap ) / 8;

		// R8 only
		assert( bytesPerPixel == 1 );

		for ( unsigned y = 0; y < height; ++y ) {
			BYTE *bits = FreeImage_GetScanLine( bitmap, offsetHeight - y );
			bits += ( offsetWidth * bytesPerPixel );

			for ( unsigned x = 0; x < width; ++x ) {
				texture.mTextureData.push_back( bits[ FI_RGBA_RED ] );
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