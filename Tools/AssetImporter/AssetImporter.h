#pragma once

#include "JonsPackage.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "FreeImage.h"
#include <vector>
#include <string>

namespace JonsAssetImporter
{
    enum class Command
    {
        UNKNOWN = 0,

        /*
        * 'import' : <name_of_jons_pkg> [OPTIONS]
        * -a <asset_1>, ..., <asset_n>
        * -n <asset_1_name>, ..., <asset_name_n>
        * -p <package_name>
        */
        IMPORT
    };

    struct ParseResult
    {
        bool mSuccess;
        const std::string& mLog;
    };

    ParseResult ParseCommands(const Command command, std::vector<std::string>& parameters);
}