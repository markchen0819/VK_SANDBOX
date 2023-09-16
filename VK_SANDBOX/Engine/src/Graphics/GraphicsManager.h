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
    class IHCDescriptorPool;
    class IHCDescriptorSetLayout;
    class IHCDescriptorWriter;
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


        // For accessing between shader(pipeline) and resources
        // The followings are for
        // basicRenderSystem: ihcpipeline & wireframe pipeline
        // both using same shaders and pipeline layout (therefore descritpor layout)
        std::unique_ptr<IHCDescriptorPool> globalDescriptorPool{}; // memory pool
        std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> uboBuffers; // resource having camera matrices
        std::vector<VkDescriptorSet> globalDescriptorSets; // pointer to resource: uboBuffers
        std::unique_ptr <IHCDescriptorSetLayout> globalDescriptorSetLayout; // slots on the pipeline accepting descriptor sets to be bound
        int TEXTURE_COUNT_LIMIT = 200;
        std::unique_ptr<IHCDescriptorPool> localDescriptorPool{}; // memory pool
        // resource having textures are in assetManager texture repo
        std::vector<VkDescriptorSet> localDescriptorSets;// pointer to resource: textures
        std::unique_ptr <IHCDescriptorSetLayout> localDescriptorSetLayout; // slots on the pipeline accepting descriptor sets to be bound
        // use map to find texture's correspoind descriptorsets
        // holds the info per scene (as tecture is loaded/ unloaded per scene)
        std::unordered_map<std::string, std::vector<VkDescriptorSet>> textureToDescriptorSetsMap; 
        std::stack<VkDescriptorSet> availableDescriptorSets; // Stack of available descriptor sets.
        
        
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
// 1. Rendering Setup (RenderSystems (Pipelines,DescriptorSetLayouts )
// 2. Resource Setup (UniformBuffers, DescriptorPool, DescriptorSets(ubo, textures))
//
// Update()
// 1. Handle System Events:
// 2. Pre-rendering Updates (Update Uniform Buffers)
// 3. Render
//    BeginSwapChainRenderPass
//    RenderGameObjects
//        Bind Global Pipeline and GlobalDescriptors(ubo)
//        For each GameObject
//            Bind Local Pipeline and LocalDescriptors(texture)
//            Update push constants (per-object)
//            Bind model(mesh)
//            Draw the object
//    EndSwapChainRenderPass
//
