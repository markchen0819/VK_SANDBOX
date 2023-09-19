#include "../pch.h"
#include "GraphicsManager.h"

// Window
#include "../Window/AppWindow.h"
// Time
#include "../Core/Time/Time.h"
// Camera
#include "Camera.h"
// Vulkan
#include "VKWraps/VKHelpers.h"
#include "VKWraps/IHCDevice.h"
#include "VKWraps/IHCBuffer.h"
#include "Renderer.h"
#include "VKWraps/IHCDescriptors.h"
#include "VKWraps/IHCTexture.h"
//#include "VKWraps/IHCMesh.h"
#include "RenderSystems/RenderSystem.h" 
// Scene
#include "../Core/Scene/Scene.h"
// Imgui
#include <imgui_impl_vulkan.h>
// Model
#include "Animation/Model.h"
#include "VKWraps/IHCDescriptorManager.h"

IHCEngine::Graphics::GraphicsManager::GraphicsManager(std::unique_ptr<Window::AppWindow>& w)
    : appWindow(*w)
{}

IHCEngine::Graphics::GraphicsManager::~GraphicsManager()
{
    vkDeviceWaitIdle(ihcDevice->GetDevice()); // sync then allowed to destroy
}

void IHCEngine::Graphics::GraphicsManager::Init()
{
    initVulkan();
    setupBasicRenderSystem();
    // Create Other RenderSystems 
    ////// Create ParticleSystem //////
    ////// Create RayTracingSystem //////
    ////// Create SimulationSystem //////
}
void IHCEngine::Graphics::GraphicsManager::initVulkan()
{
    // Create VK instance 
    ihcDevice = std::make_unique<IHCEngine::Graphics::IHCDevice>(appWindow);

    // Create Swapchain to render to screen
    renderer = std::make_unique<IHCEngine::Graphics::Renderer>(appWindow, *ihcDevice);

    // Create Descriptor Manager
    descriptorManager = std::make_unique<IHCEngine::Graphics::IHCDescriptorManager>(*ihcDevice);

}
void IHCEngine::Graphics::GraphicsManager::setupBasicRenderSystem()
{
    auto layouts = descriptorManager->GetDefaultDescriptorSetLayoutsForBasicRenderSystem();
    // Use above to create pipeline layout, also create the pipeline afterwards
    basicRenderSystem = std::make_unique<IHCEngine::Graphics::RenderSystem>
        (
            *ihcDevice,
            renderer->GetSwapChainRenderPass(),
            layouts 
        );
}
void IHCEngine::Graphics::GraphicsManager::Update(IHCEngine::Core::Scene* scene)
{
    //glfwPollEvents();
    //IHCEngine::Core::Time::GetInstance().Update(); // windowsize change need recheck
    
    // Render
    std::map<unsigned int, IHCEngine::Core::GameObject*> gameObjects = scene->GetGameObjectsMap();
    if (auto commandBuffer = renderer->BeginFrame())
    {
        int frameIndex = renderer->GetFrameIndex();
        
        FrameInfo frameInfo
        {
            frameIndex,
            IHCEngine::Core::Time::GetInstance().GetDeltaTime(),
            commandBuffer,
            descriptorManager->GetGlobalDescriptorSets()[frameIndex],
            descriptorManager->GetTextureToDescriptorSetsMap(),
            gameObjects
        };

        // Vulkan uses a right-handed coordinate system by default.
		// Y points down
		// Z points out of screen
        GlobalUniformBufferObject ubo{};
        auto camera = scene->GetCamera();
        camera.SetAspectRatio(renderer->GetAspectRatio());
        ubo.viewMatrix = camera.GetViewMatrix();
        ubo.projectionMatrix = camera.GetProjectionMatrix();
        ubo.inverseViewMatrix = camera.GetInverseViewMatrix();
        ubo.projectionMatrix[1][1] *= -1; // Flip the Y-axis for vulkan <-> opengl

        descriptorManager->GetGlobalUBOs()[frameIndex]->WriteToBuffer(&ubo);
        descriptorManager->GetGlobalUBOs()[frameIndex]->Flush(); // Manual flush, can comment out if using VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 

        renderer->BeginSwapChainRenderPass(commandBuffer);
        //// System Render order here matters ////

        basicRenderSystem->RenderGameObjects(frameInfo);
        //pointLightSystem.render(frameInfo);
        
        //////////////////////////////////////////
        if (usingIMGUI)
        {
            ImGui::Render();  // Rendering UI
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
        }
        renderer->EndSwapChainRenderPass(commandBuffer);
        renderer->EndFrame();
    }
}
void IHCEngine::Graphics::GraphicsManager::Shutdown()
{
    vkDeviceWaitIdle(ihcDevice->GetDevice()); // sync then allowed to destroy
}

#pragma region Helpers for assetManagement (texture, mesh)
std::unique_ptr<IHCEngine::Graphics::IHCTexture> IHCEngine::Graphics::GraphicsManager::CreateTexture(std::string assetName, std::string path)
{
    auto texture = std::make_unique<IHCEngine::Graphics::IHCTexture>(*ihcDevice, assetName, path);
    descriptorManager->AllocateLocalDescriptorSetForTexture(texture.get());
    return texture;
}
void IHCEngine::Graphics::GraphicsManager::DestroyTexture(std::string assetName)
{
    descriptorManager->DeallocateLocalDescriptorSetForTexture(assetName);
}
std::unique_ptr<IHCEngine::Graphics::IHCMesh> IHCEngine::Graphics::GraphicsManager::CreateMesh(std::string assetName, std::string path)
{
    // Don't need to keep track for models, they self-deallocate
    // Textures need keeping track due to descriptors
    return IHCMesh::CreateMeshFromFile(*ihcDevice, path);
}
std::unique_ptr<IHCEngine::Graphics::IHCMesh> IHCEngine::Graphics::GraphicsManager::CreateMesh(std::string assetName, IHCEngine::Graphics::IHCMesh::Builder& builder)
{
    // Don't need to keep track for models, they self-deallocate
    // Textures need keeping track due to descriptors
    return std::make_unique<IHCEngine::Graphics::IHCMesh>(*ihcDevice, builder);
}
void IHCEngine::Graphics::GraphicsManager::DestroyMesh(std::string assetName)
{
    // Don't need to keep track for models
    // just created for same format
}

std::unique_ptr<IHCEngine::Graphics::Model> IHCEngine::Graphics::GraphicsManager::CreateModel(std::string assetName, std::string path)
{
    return std::move(std::make_unique<IHCEngine::Graphics::Model>(path));
}

void IHCEngine::Graphics::GraphicsManager::DestroyModel(std::string assetName)
{
}

#pragma endregion

#pragma region Imgui
VkRenderPass IHCEngine::Graphics::GraphicsManager::GetRenderPass()
{
    return renderer->GetSwapChainRenderPass();
}
IHCEngine::Graphics::IHCDevice* IHCEngine::Graphics::GraphicsManager::GetIHCDevice()
{
    return ihcDevice.get();
};
ImGui_ImplVulkan_InitInfo IHCEngine::Graphics::GraphicsManager::GetImGui_ImplVulkan_InitInfo()
{
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = ihcDevice->GetInstance();
    init_info.PhysicalDevice = ihcDevice->GetPhysicalDevice();;
    init_info.Device = ihcDevice->GetDevice();
    init_info.QueueFamily = ihcDevice->GetGraphicsQueueIndex();
    init_info.Queue = ihcDevice->GetGraphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    //init_info.DescriptorPool = ; // use imguiPool
    init_info.MinImageCount = 2;
    init_info.ImageCount = renderer->GetSwapChainImageCount();
    init_info.MSAASamples = ihcDevice->GetMsaaSamples();
    return init_info;
}
VkCommandBuffer IHCEngine::Graphics::GraphicsManager::BeginSingleTimeImGuiCommandBuffer()
{
    return ihcDevice->BeginSingleTimeCommands();
}
void IHCEngine::Graphics::GraphicsManager::EndSingleTimeImGuiCommandBuffer(VkCommandBuffer cmdBuffer)
{
    ihcDevice->EndSingleTimeCommands(cmdBuffer);
}
#pragma endregion
