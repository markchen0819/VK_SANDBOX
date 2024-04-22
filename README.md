# IHC Graphics Framework
A Vulkan graphics framework create by I Hsuan "Mark" Chen

## Current structure
- Entry Point
- Application
    - SampleApplication (User defined application)
    - Scene (User defined scenes)
    - CustomBehaviors (User self defined logics like scripting)
        - CPUFrustumCulling (specifically for grass at the moment)
        - ImguiContext (UI Helper for debugging)
- Engine
    - Core
        - Time
        - AssetManager
        - SceneManager/ GameObjects/ Components
    - Graphics
        - Graphics Manager
        - Animation (Model loading/ Animator/ Animation/ Inverse Kinematics)
        - Particle 
        - Grass
        - Fluid
        - Vulkan (VKWraps/ RenderSystems/ different pipelines & shaders/ compute shaders)
        - Camera
        - Renderer (Swapchain)
        - GraphicsAssetCreator (between assets & Vulkan resource allocation(descriptorsets/ buffers/ image)
    - Imgui (debugging interface)
    - Input
    - Math (Custom quaternion/ VQS/ Path)
    - Window
    - Physics (Softbody/ Spring)
## Build Instructions
1. Download assets and extract to corresponding location  <br> 
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
F1: Model loading/ Skeletal Animation and Skinning <br>
<img src="https://drive.google.com/uc?export=view&id=1uuAaO-5SR1i0GL2IXsLl_FbAVHWWvbTY" alt="Description of the Image" width="500"> <br>
F2: Path Following/ Orientation Control/ Motion Blending <br>
<img src="https://drive.google.com/uc?export=view&id=1GR3PNG_2mk9tmkcBVtWzmMP6yGWvlDat" alt="Description of the Image" width="500"> <br>
F3: Inverse Kinematics with FABRIK <br>
<img src="https://drive.google.com/uc?export=view&id=1cHNVNlCONACe0I46ATW3nzpniolvrhvF" alt="Description of the Image" width="500"> <br>
F4: Cloth Simulation <br>
<img src="https://drive.google.com/uc?export=view&id=19aawk56NNIXL8RZxzxlzTz8YyMxMzlG0" alt="Description of the Image" width="500"> <br>
Breakdown of F1~F4: https://youtu.be/zH8QJtPN9b0 <br> 

F5: Particle System (Compute Shader + GPU Instancing) <br>
Breakdown: https://youtu.be/6IlP3RwXFX0 <br> 
<img src="https://drive.google.com/uc?export=view&id=1G5tSJJLQWNfndXectGil-q4deC3PViLG" alt="Description of the Image" width="500">

F6: Grass Simulation  <br>
Breakdown: https://youtu.be/SMVMA5gTPYA <br> 
<img src="https://drive.google.com/uc?export=view&id=1Xg_k1JH7E7eNLqJ4mBmTS_PaTR3LBGQV" alt="Description of the Image" width="500">

F7: Fluid Simulation  <br>
Breakdown: https://youtu.be/Yuo_39or1Vw <br> 
<img src="https://drive.google.com/uc?export=view&id=1Vh9W1kNxDH0Iy_IGum9rxzzpx3hi-rfL" alt="Description of the Image" width="500">

Organized Breakdown: https://youtu.be/fvPsugjBFNg <br> 
## References
- Vulkan <br> 
Vulkan Tutorial - https://vulkan-tutorial.com/ <br> 
Vulkan C++ examples and demos - https://github.com/SaschaWillems/Vulkan <br> 
Vulkan Game Engine Tutorial - https://youtu.be/Y9U9IE0gVHA?feature=shared <br>
- Animation <br> 
Skeletal Animation - https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation  <br>
FABRIK: a fast, iterative solver for the inverse kinematics problem - http://www.andreasaristidou.com/FABRIK.html <br>
- Grass <br>
Procedural Grass in 'Ghost of Tsushima' - https://youtu.be/Ibe1JBF5i5Y?feature=shared <br>
Modern Foliage Rendering - https://youtu.be/jw00MbIJcrk?feature=shared <br>
Interactive Wind and Vegetation in 'God of War' - https://youtu.be/MKX45_riWQA?feature=shared <br>
How do Major Video Games Render Grass? - https://youtu.be/bp7REZBV4P4?feature=shared <br>
Between Tech and Art: The Vegetation of Horizon Zero Dawn - https://youtu.be/wavnKZNSYqU?si=KXW5g2xCANX0SBuH <br>
Chapter 7. Rendering Countless Blades of Waving Grass - https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-7-rendering-countless-blades-waving-grass <br>
Frustum Culling - https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
- Fluid <br>
Particle-Based Fluid Simulation for Interactive Applications - https://matthias-research.github.io/pages/publications/sca03.pdf <br>
Coding Adventure: Simulating Fluids - https://youtu.be/rSKMYc1CQHE?si=OsuSGhJP9wxaHWtV <br>
Optimizing our Realtime Fluid Simulation in Unity - https://youtu.be/9M72KrGhYuE?si=2U6MzH_OsqHPKild <br>
## Last Update -  (2024/04/19)
