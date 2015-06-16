#pragma once

#include "include/Core/Types.h"

#include <string>

namespace JonsEngine
{
    class Skybox
    {
    public:
        Skybox(const std::string& name, const TextureID skyboxTexture);


        const std::string mName;
        const TextureID mSkyboxTexture;
    };
}