#pragma once
#include <stack>
#include "VKWraps/VKHelpers.h"
#include "VKWraps/IHCMesh.h"

// Forward declaration
namespace IHCEngine::Window
{
    class AppWindow;
}
namespace IHCEngine::Core
{
    class Time;
    class Scene;
}
namespace IHCEngine::Graphics
{
	class Model;
	class IHCDevice;
    class IHCBuffer;
    class Renderer;  // contains IHCSwapchain
    class IHCDescriptorManager;
    class IHCTexture;
    class IHCMesh;
    class RenderSystem; // contains IHCPipeline
}
struct ImGui_ImplVulkan_InitInfo;

namespace IHCEngine::Graphics
{
    class GraphicsManager
    {
    public:

        GraphicsManager(std::unique_ptr< Window::AppWindow>& w);
        ~GraphicsManager();
        // no duplication
        GraphicsManager(const  GraphicsManager&) = delete;
        GraphicsManager& operator=(const  GraphicsManager&) = delete;

        void Init();
        void Update(IHCEngine::Core::Scene* scene);  
        void Shutdown();


        // Helper Functions for AssetManagement
        std::unique_ptr<IHCEngine::Graphics::IHCTexture> CreateTexture(std::string assetName, std::string path);
        void DestroyTexture(std::string assetName);
        std::unique_ptr<IHCEngine::Graphics::IHCMesh> CreateMesh(std::string assetName, std::string path);
        std::unique_ptr<IHCEngine::Graphics::IHCMesh> CreateMesh(std::string assetName, IHCEngine::Graphics::IHCMesh::Builder& builder);
        void DestroyMesh(std::string assetName);
        std::unique_ptr<IHCEngine::Graphics::Model> CreateModel(std::string assetName, std::string path);
        void DestroyModel(std::string assetName);


        // Helper for ImGui
        bool usingIMGUI = true;
        VkRenderPass GetRenderPass();
        IHCEngine::Graphics::IHCDevice* GetIHCDevice();
        ImGui_ImplVulkan_InitInfo GetImGui_ImplVulkan_InitInfo();
        VkCommandBuffer BeginSingleTimeImGuiCommandBuffer();
        void EndSingleTimeImGuiCommandBuffer(VkCommandBuffer cmdBuffer);

    private:

        void initVulkan();
        void setupBasicRenderSystem();
        Window::AppWindow& appWindow;

        //// order of declarations matter for vulkan cleanup ////
        std::unique_ptr<IHCEngine::Graphics::IHCDevice> ihcDevice;
        std::unique_ptr<IHCEngine::Graphics::Renderer> renderer; // Swapchain
        std::unique_ptr<IHCEngine::Graphics::IHCDescriptorManager> descriptorManager;

        // rendersystems
        std::unique_ptr<IHCEngine::Graphics::RenderSystem> basicRenderSystem; 
        // std::unique_ptr<IHCEngine::Graphics::RenderSystem> particleSystem;
        // std::unique_ptr<IHCEngine::Graphics::RenderSystem> rayTracingSystem;
        // std::unique_ptr<IHCEngine::Graphics::RenderSystem> simulationSystem;

    };
}


// Logic breakdown for GraphicsManager
// 
// Init()
// 1. Initialize the Vulkan Context
// 2. Window and Presentation Setup (Swapchain)
// 
// setupBasicRenderSystem()
// 1. DescriptorManager set up for pipeline creations in rendersystems
// 2. Rendering Setup (RenderSystems (Pipelines)
//
// Update()
// 1. Handle System Events:
// 2. Pre-rendering Updates (Update GlobalUBOs)
// 3. Render
//    BeginSwapChainRenderPass
//    RenderGameObjects
//        Bind Target Pipeline and GlobalDescriptors 
//        For each GameObject
//            Bind LocalDescriptors
//            Update push constants (per-object)
//            Bind model(mesh)
//            Draw the object
//    EndSwapChainRenderPass
//
