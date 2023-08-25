#pragma once
#include "../pch.h"
#include <stb_image.h>

// Window
#include "../Window/AppWindow.h"

// Time
#include "../Core/Time/Time.h"

// Vulkan
#include "VKHelpers.h"
#include "IHCDevice.h"
#include "IHCDescriptors.h"
#include "RenderSystem.h" // contains IHCPipeline
#include "Renderer.h" // contains IHCSwapchain

// Camera
#include "Camera.h"

// GameObject
#include "IHCModel.h"
#include "../Core/GameObject.h"

const std::string TEXTURE_PATH = "Engine/assets/models/viking_room/viking_room.png";

namespace IHCEngine::Graphics
{
    class GraphicsManager
    {
    public:
        void Init(std::unique_ptr< Window::AppWindow>& w);
        void Update();
        void Shutdown();

        // no duplication
        GraphicsManager(const  GraphicsManager&) = delete;
        GraphicsManager& operator=(const  GraphicsManager&) = delete;

    private:

        void initVulkan();
        void loadGameObjects();

        Window::AppWindow& appWindow;
        Camera camera;
        //// order of declarations matter for vulkan cleanup ////
        std::unique_ptr<IHCEngine::Graphics::IHCDevice> ihcDevice;
        std::unique_ptr<IHCEngine::Graphics::Renderer> renderer; // Swapchain
        // For shaders
        std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> uboBuffers; // matrices
        std::unique_ptr<IHCDescriptorPool> globalDescriptorPool{};
        std::vector<VkDescriptorSet> globalDescriptorSets;

        // customizable
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

        std::unique_ptr<IHCEngine::Core::GameObject> testGobj = nullptr;
        std::unordered_map<unsigned int, IHCEngine::Core::GameObject> gameObjects;
        void loadGameObjects()
        {
            std::shared_ptr<IHCModel> testModel =
                IHCModel::CreateModelFromFile
                (
                    *ihcDevice, 
                    "Engine/assets/models/viking_room/viking_room.obj"
                );
            std::shared_ptr<IHCTexture> testTexture =
                std::make_shared < IHCTexture>
                (
                    *ihcDevice,
                    "Engine/assets/models/viking_room/viking_room.png"
                );
            testGobj = std::make_unique<IHCEngine::Core::GameObject>(IHCEngine::Core::GameObject::CreateGameObject());
            testGobj->model = testModel;
            testGobj->texture= testTexture;
            gameObjects.emplace(testGobj->GetUID(), std::move(testGobj));
            
            //testGobj.transform.GetLocalModelMatrix();
        }
    };
}