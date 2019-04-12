#pragma once

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include "boost/filesystem.hpp"
#include <map>
#include <cstdint>
#include <string>

namespace JonsAssetImporter
{
    typedef std::map<uint32_t, uint32_t> MaterialMap;       // assimp material index - to - JonsPackage material index
    typedef std::pair<uint32_t, uint32_t> MaterialPair;


    void Log(const std::string& msg);
    const std::string& GetLog();
}