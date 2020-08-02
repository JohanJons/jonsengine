#pragma once

#include "Resources/JonsPackage.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "FreeImage.h"
#include <vector>
#include <string>

namespace JonsAssetImporter
{
    struct ParseResult
    {
        bool mSuccess;
        std::string mLog;
    };

    ParseResult ParseCommands( const std::vector<std::string>& parameters );
}