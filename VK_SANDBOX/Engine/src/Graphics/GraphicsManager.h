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
        // customizable
        std::unique_ptr<IHCEngine::Graphics::RenderSystem> basicRenderSystem; // 
 


        std::vector<IHCEngine::Core::GameObject> gameObjects;





        // graphcis pipeline
        VkDescriptorSetLayout descriptorSetLayout;


        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;


        // Texture
        uint32_t mipLevels;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;

    };
}