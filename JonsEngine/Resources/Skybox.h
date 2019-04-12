#pragma once

#include "Types.h"
#include "IDMap.hpp"

#include <string>

namespace JonsEngine
{
    class Skybox
    {
    public:
        Skybox(const std::string& name, const DX11TextureID skyboxTexture);
        ~Skybox();

        const std::string& GetName() const;
        DX11TextureID GetSkyboxTexture() const;


    private:
         std::string mName;
         DX11TextureID mSkyboxTexture;
    };

    typedef IDMap<Skybox>::ItemID SkyboxID;
    static const SkyboxID INVALID_SKYBOX_ID = IDMap<Skybox>::INVALID_ITEM_ID;
}