add_executable( JonsAssetImporter "" )

target_sources( JonsAssetImporter
    PRIVATE
        AssetImporter.cpp
        AssetImporter.h
        main.cpp
        Assimp.cpp
        Assimp.h
        FreeImageParser.cpp
        FreeImageParser.h
        Utils.cpp
        Utils.h
)

get_target_property( SOURCES JonsAssetImporter SOURCES )
source_group("" FILES ${SOURCES} )

target_link_libraries( JonsAssetImporter JonsEngine FreeImage assimp toml11 )
set_target_properties( JonsAssetImporter PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin" )
