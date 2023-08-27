#pragma once
#include <stb_image.h>

// Window
#include "../Window/AppWindow.h"

// Time
#include "../Core/Time/Time.h"

// Vulkan
#include "VKWraps/VKHelpers.h"
#include "VKWraps/IHCDevice.h"
#include "VKWraps/IHCDescriptors.h"
#include "RenderSystems/RenderSystem.h" // contains IHCPipeline
#include "Renderer.h" // contains IHCSwapchain

// Camera
#include "Camera.h"

// GameObject
#include "VKWraps/IHCModel.h"
#include "../Core/Scene/GameObject.h"

namespace IHCEngine::Graphics
{
    class GraphicsManager
    {
    public:

        GraphicsManager(std::unique_ptr< Window::AppWindow>& w);
        ~GraphicsManager() {};

        void Init();
        void Update();
        void Shutdown();

        // no duplication
        GraphicsManager(const  GraphicsManager&) = delete;
        GraphicsManager& operator=(const  GraphicsManager&) = delete;

    private:

        void initVulkan();
        void setupBasicRenderSystem();
        Window::AppWindow& appWindow;
        Camera camera;
        //// order of declarations matter for vulkan cleanup ////
        std::unique_ptr<IHCEngine::Graphics::IHCDevice> ihcDevice;
        std::unique_ptr<IHCEngine::Graphics::Renderer> renderer; // Swapchain
        // For shaders
        std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> uboBuffers; // matrices
        std::unique_ptr<IHCDescriptorPool> globalDescriptorPool{};
        std::vector<VkDescriptorSet> globalDescriptorSets;
        std::unique_ptr<IHCDescriptorPool> localDescriptorPool{};
        std::vector<VkDescriptorSet> localDescriptorSets;
        // rendersystem
        std::unique_ptr<IHCEngine::Graphics::RenderSystem> basicRenderSystem; // 
 
        // Logic breakdown for GraphicsManager
        // 
        // Init()
        // 1. Initialize the Vulkan Context
        // 2. Window and Presentation Setup (Swapchain)
        // 3. Resource Setup (UniformBuffers, textures, Descriptors)
        // 4. Rendering Setup (RenderSystems (or Pipelines)
        //
        // Update()
        // 1. Handle System Events:
        // 2. Pre-rendering Updates (Update Uniform Buffers)
        // 3. Render
        //    BeginSwapChainRenderPass
        //    RenderGameObjects
        //        Bind Global Pipeline and Descriptors
        //        For each GameObject
        //            Update push constants (per-object)
        //            Bind material
        //            Bind model(mesh)
        //            Draw the object
        //    EndSwapChainRenderPass
        //

        // Temporary
        //std::unique_ptr<IHCEngine::Core::GameObject> testGobj = nullptr;
        std::unique_ptr<IHCEngine::Core::GameObject> testGobj1 = nullptr;
        std::unique_ptr<IHCEngine::Core::GameObject> testGobj2 = nullptr;
        std::unordered_map<unsigned int, IHCEngine::Core::GameObject*> gameObjects;
        std::unordered_map<unsigned int, std::shared_ptr<IHCTexture>> textures;
        
        std::unordered_map<IHCEngine::Core::GameObject*, VkDescriptorSet> gameObjectToDescriptorSet;
        void loadGameObjects();

    };
}