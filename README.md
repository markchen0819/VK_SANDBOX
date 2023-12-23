# IHC Graphics Framework
A Vulkan graphics framework create by I Hsuan "Mark" Chen

## Current structure
- Entry Point
- Application
    - SampleApplication (User defined application)
    - Scene (User defined scenes)
    - CustomBehaviors (User self defined logics like scripting)
- Engine
    - Core
        - Time
        - AssetManager
        - SceneManager/ GameObjects/ Components
    - Graphics
        - Graphics Manager
        - Animation (Model loading/ Animator/ Animation/ Inverse Kinematics)
        - Vulkan (VKWraps/ RenderSystems/ different pipelines & shaders)
        - Camera
        - Renderer (Swapchain)
        - GraphicsAssetCreator (between assets & Vulkan resource allocation(descriptorsets/ buffers/ image)
    - Imgui (debugging interface)
    - Input
    - Math (Custom quaternion/ VQS/ Path)
    - Window
    - Physics (Softbody/ Spring)
## Build Instructions
1. Download assets and extract to corresponding location
   https://drive.google.com/drive/folders/1jxcT7dTBuUUo0AkOVzhjSxDT3kY5J2M6?usp=drive_link
   
3. Build project in VS22

## 3rd party libraries
- glfw 3.3.8.bin.WIN64
- glm 0.9.9.8.
- Vulkan 1.3.243.0
- imgui
- stb_image
- Open Asset Import Library (assimp)
- tinyobjloader

## Applications
F1: Model loading/ Skeletal Animation and Skinning

<img src="https://drive.google.com/uc?export=view&id=1uuAaO-5SR1i0GL2IXsLl_FbAVHWWvbTY" alt="Description of the Image" width="500">


F2: Path Following/ Orientation Control/ Motion Blending

<img src="https://drive.google.com/uc?export=view&id=1GR3PNG_2mk9tmkcBVtWzmMP6yGWvlDat" alt="Description of the Image" width="500">

F3: Inverse Kinematics with FABRIK

<img src="https://drive.google.com/uc?export=view&id=1cHNVNlCONACe0I46ATW3nzpniolvrhvF" alt="Description of the Image" width="500">


F4: Cloth Simulation

<img src="https://drive.google.com/uc?export=view&id=19aawk56NNIXL8RZxzxlzTz8YyMxMzlG0" alt="Description of the Image" width="500">

## Last Update -  (2023/12/22)
