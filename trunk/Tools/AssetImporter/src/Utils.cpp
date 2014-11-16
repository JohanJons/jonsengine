#include "include/Utils.h"

#include "FreeImage.h"
#include <sstream>

namespace JonsAssetImporter
{
    static std::string gLog;


    AssetType GetAssetType(const char* assetName)
    {
        // check if texture
        FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(assetName);
        if (imageFormat == FIF_UNKNOWN)
            imageFormat = FreeImage_GetFIFFromFilename(assetName);

        if (imageFormat != FIF_UNKNOWN || FreeImage_FIFSupportsReading(imageFormat))
            return MATERIAL;
        else
            // default: try as model
            // TODO: check if loadable by assimp?
            return MODEL;
    }


    void Log(const std::string& msg)
    {
        gLog.append(msg);
        gLog.append("\n");
    }

    const std::string& GetLog()
    {
        return gLog;
    }
}