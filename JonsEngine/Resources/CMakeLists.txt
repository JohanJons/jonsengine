target_sources( JonsEngine
    PRIVATE
        Animation.cpp
        Animation.h
        AnimationInstance.cpp
        AnimationInstance.h
        Bone.cpp
        Bone.h
        BoneKeyframe.cpp
        BoneKeyframe.h
        JonsPackage.cpp
        JonsPackage.h
        Material.cpp
        Material.h
        Mesh.cpp
        Mesh.h
        Model.cpp
        Model.h
        ModelNode.cpp
        ModelNode.h
        ResourceManifest.cpp
        ResourceManifest.h
        Skybox.cpp
        Skybox.h
        TerrainData.cpp
        TerrainData.h
)

target_include_directories( JonsEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR})
