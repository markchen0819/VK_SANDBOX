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
#include "VKWraps/IHCTexture.h"
#include "VKWraps/IHCDescriptorManager.h"
#include "Renderer.h"
//#include "VKWraps/IHCMesh.h"
#include "RenderSystems/RenderSystem.h"
#include "RenderSystems/ParticleSystem.h" 
// Scene
#include "../Core/Locator/SceneManagerLocator.h"
#include "../Core/Scene/Scene.h"
// Imgui
#include <imgui_impl_vulkan.h>
// Model
#include "GraphicsAssetCreator.h"

#include "VKWraps/DescriptorWraps/GlobalDescriptorWrap.h"


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
    setupRenderSystems();
}
void IHCEngine::Graphics::GraphicsManager::initVulkan()
{
    // Create VK instance 
    ihcDevice = std::make_unique<IHCEngine::Graphics::IHCDevice>(appWindow);

    // Create Swapchain to render to screen
    renderer = std::make_unique<IHCEngine::Graphics::Renderer>(appWindow, *ihcDevice);

    // Create Descriptor Manager
    descriptorManager = std::make_unique<IHCEngine::Graphics::IHCDescriptorManager>(*ihcDevice);

    // Create Graphics Asset Creator
    graphicsAssetCreator = std::make_unique<IHCEngine::Graphics::GraphicsAssetCreator>(*ihcDevice, descriptorManager.get());
}
void IHCEngine::Graphics::GraphicsManager::setupRenderSystems()
{
    // Use above to create pipeline layout, also create the pipeline afterwards
    basicRenderSystem = std::make_unique<IHCEngine::Graphics::RenderSystem>
        (
            *ihcDevice,
            renderer->GetSwapChainRenderPass(),
            descriptorManager.get()
        );
    particleSystem = std::make_unique<IHCEngine::Graphics::ParticleSystem>
        (
            *ihcDevice,
            renderer->GetSwapChainRenderPass(),
            descriptorManager.get()
        );
}
void IHCEngine::Graphics::GraphicsManager::Update()
{
    // Fixes window interrupt
    IHCEngine::Core::Time::Update();

    // Compute
    auto computeCommandBuffer = renderer->BeginComputeCommands();
    int computeFrameIndex = renderer->GetComputeFrameIndex();
    FrameInfo computeFrameInfo
    {
        computeFrameIndex,
        computeCommandBuffer,
    };
    particleSystem->Compute(computeFrameInfo);
    renderer->EndComputeCommands();

    // Render
    if (auto commandBuffer = renderer->BeginFrame())
    {
        int frameIndex = renderer->GetFrameIndex();

        FrameInfo frameInfo
        {
            frameIndex,
            commandBuffer,
        };

        // Vulkan uses a right-handed coordinate system by default.
		// Y points down
		// Z points out of screen
        GlobalUniformBufferObject ubo{};
        auto activeScene = Core::SceneManagerLocator::GetSceneManager()->GetActiveScene();
        auto camera = activeScene->GetCamera();
        camera.SetAspectRatio(renderer->GetAspectRatio());
        ubo.viewMatrix = camera.GetViewMatrix();
        ubo.projectionMatrix = camera.GetProjectionMatrix();
        ubo.inverseViewMatrix = camera.GetInverseViewMatrix();
        ubo.projectionMatrix[1][1] *= -1; // Flip the Y-axis for vulkan <-> opengl
        ubo.cameraPosition = glm::vec4(camera.transform.GetPosition(), 0);

        descriptorManager->GetGlobalDescriptorWrap()->GetGlobalUBOs()[frameIndex]->WriteToBuffer(&ubo);
        descriptorManager->GetGlobalDescriptorWrap()->GetGlobalUBOs()[frameIndex]->Flush(); // Manual flush, can comment out if using VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 

        renderer->BeginSwapChainRenderPass(commandBuffer);

        //// System Render order here matters ////
        basicRenderSystem->RenderGameObjects(frameInfo);
        particleSystem->RenderGameObjects(frameInfo);
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

void IHCEngine::Graphics::GraphicsManager::SetClearColor(const glm::vec3& color) const
{
    renderer->SetClearColor(color);
}