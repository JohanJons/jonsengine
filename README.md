# jonsengine
DirectX11 game engine

# List of features (2015-07-04)

Core:
- File logging
- Basic memory management
- Basic debugging features

Resources:
- Custom binary file format for parsing game assets
- Offline asset exporter tool for above mentioned file format
- Various thirdparty libraries to parse a wide range of assets into a unified asset format
- Create basic geometric shapes at runtime

Rendering:
- Directx 11 renderer
- Deferred shading pipeline with phong shading
- Directional/Point/Ambient lights with shadowmapping
- Cascaded shadow mapping (CSM) with Sample distribution shadow maps (SDSM)
- Shadow filtering (OptimizedPCF)
- Screenspace ambient occlusion (Scalable Ambient Obscurance)
- Antialiasing (FXAA)
- Gamma corrected lighting
- Skyboxes

Scene:
- Basic hierarchical scenegraph structure
- Models, materials, cameras, directional/point/ambient lights, skyboxes
- Basic renderqueue with mesh culling for camera and directional/point lights

Window:
- Basic window management
- Raw keyboard/mouse input handling




![Example screenshot](Example/example.jpg)
