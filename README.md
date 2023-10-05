# IHC Graphics Framework
A Vulkan graphics framework create by I Hsuan "Mark" Chen

## Current structure
- Entry Point
- Application
    - Scene
    - CustomBehaviors (User self defined logics like scripting)
- Engine
    - Core
        - Time
        - AssetManager
        - SceneManager/ GameObjects/ Components
    - Graphics
        - Graphics Manager
        - Animation (Model loading/ Animator/ Animation)
        - Vulkan (VKWraps/ different pipelines & shaders)
        - Renderer (Swapchain)
    - Imgui (debugging interface)
    - Math (custom quaternioin & VQS)
    - Window

## 3rd party libraries
- glfw 3.3.8.bin.WIN64
- glm 0.9.9.8.
- Vulkan 1.3.243.0
- imgui
- stb_image
- Open Asset Import Library (assimp)
- tinyobjloader

## Last Update -  (2023/10/01)