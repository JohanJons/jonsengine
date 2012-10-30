#include "include/Core/EngineSettings.h"

namespace JonsEngine
{
    EngineSettings::EngineSettings() :  RenderBackend(RENDER_OPENGL), WindowTitle("JonsEngine Game"),
                                        InputBackend(InputBackend::GLFW)
    {
    }
}