target_sources( ${ENGINE_NAME}
    PUBLIC
        AnimatedActor.cpp
        AnimatedActor.h
        AnimationUpdater.cpp
        AnimationUpdater.h
        BaseActor.cpp
        BaseActor.h
        Camera.cpp
        Camera.h
        DirectionalLight.cpp
        DirectionalLight.h
        PointLight.cpp
        PointLight.h
        Scene.cpp
        Scene.h
        SceneDirtyFlags.h
        SceneManager.cpp
        SceneManager.h
        SceneNode.cpp
        SceneNode.h
        SceneParser.cpp
        SceneParser.h
        SceneSettings.h
        StaticActor.cpp
        StaticActor.h
        Terrain.cpp
        Terrain.h
        TerrainTransforms.cpp
        TerrainTransforms.h
)

target_include_directories( ${ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
