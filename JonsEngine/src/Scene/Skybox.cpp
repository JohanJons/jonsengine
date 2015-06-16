#include "include/Scene/Skybox.h"

namespace JonsEngine
{
    Skybox::Skybox(const std::string& name, const TextureID skyboxTexture) : mName(name), mSkyboxTexture(skyboxTexture)
    {
    }

    Skybox::~Skybox()
    {
    }
}