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
#include "Renderer.h"
#include "RenderSystem.h"

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
        std::unique_ptr<IHCEngine::Graphics::IHCDevice> ihcDevice;
        std::unique_ptr<IHCEngine::Graphics::Renderer> renderer; 
        Camera camera;
        std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> uboBuffers;

        // customizable
        std::unique_ptr<IHCEngine::Graphics::RenderSystem> basicRenderSystem;
 


        std::vector<IHCEngine::Core::GameObject> gameObjects;


        // graphcis pipeline
        VkDescriptorSetLayout descriptorSetLayout;

        // model
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        // Shader data
        //VkBuffer vertexBuffer;
        //VkDeviceMemory vertexBufferMemory;
        //VkBuffer indexBuffer;
        //VkDeviceMemory indexBufferMemory;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;
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