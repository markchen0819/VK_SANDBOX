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
//#include "VKWraps/IHCModel.h"
#include "RenderSystems/RenderSystem.h" 
// Scene
#include "../Core/Scene/Scene.h"
// Imgui
#include <imgui_impl_vulkan.h>

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

}
void IHCEngine::Graphics::GraphicsManager::setupBasicRenderSystem()
{
    ////// Create a basic shading pipeline //////
    //// Set up shader interface (DescriptorSetLayout (ubo, sampler), PipelineLayout)
    globalDescriptorSetLayout =
        IHCDescriptorSetLayout::Builder(*ihcDevice)
        .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)  // Assuming this is for vertex shaders
        .Build();
    localDescriptorSetLayout =
        IHCDescriptorSetLayout::Builder(*ihcDevice)
        .AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)  // Assuming this is for fragment shaders
        .Build();

    // In GLSL, our case
    // globalDescriptorSetLayout set 0, Binding0
    // localDescriptorSetLayout set 1, Binding0
    std::vector<VkDescriptorSetLayout> layouts { globalDescriptorSetLayout->GetDescriptorSetLayout(), localDescriptorSetLayout->GetDescriptorSetLayout()};
    //// Use the above to create pipeline layout, also create the pipeline afterwards
    basicRenderSystem = std::make_unique<IHCEngine::Graphics::RenderSystem>
        (
            *ihcDevice,
            renderer->GetSwapChainRenderPass(),
            layouts //globalDescriptorSetLayout->GetDescriptorSetLayout()
        );

    //// Allocate memory (ubo)
    // Allocate UniformBuffers for updating shaders (camera matrices, global light info)
    // other data that's consistent across a single draw call or frame
    uboBuffers.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++)
    {
        uboBuffers[i] = std::make_unique<IHCBuffer>
            (
                *ihcDevice,
                sizeof(GlobalUniformBufferObject),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                //| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
                // we can uncomment this as we're not using minOffsetAlignment for now
            );
        uboBuffers[i]->Map(); // persistent mapping
    }
    globalDescriptorPool =
        IHCDescriptorPool::Builder(*ihcDevice)
        .SetMaxSets(IHCSwapChain::MAX_FRAMES_IN_FLIGHT)
        .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, IHCSwapChain::MAX_FRAMES_IN_FLIGHT)
        .Build();
    globalDescriptorSets.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
    // Allocate global descriptor sets
    for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto bufferInfo = uboBuffers[i]->GetDescriptorInfo();
        IHCDescriptorWriter(*globalDescriptorSetLayout, *globalDescriptorPool)
            .WriteBuffer(0, &bufferInfo)  // UBO
            .Build(globalDescriptorSets[i]);
    }
    // Analogy
    // uboBuffers: (ingredients)
    // DescriptorSetLayout: (recipe, how to assemble the ingredients)
    // DescriptorPool: Preparation table with limited space (memory). Dishes (DescriptorSets) occupy this space
    // DescriptorSets: Dishes on the preparation table
    // 
    //// Allocate memory (sampler)
    // Allocate a pool with size of TEXTURE_COUNT_LIMIT, sets are created after textures are loaded in
    localDescriptorPool =
        IHCDescriptorPool::Builder(*ihcDevice)
        .SetMaxSets(TEXTURE_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT)
        .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, TEXTURE_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT) // sampler
        .Build();
    localDescriptorSets.resize(TEXTURE_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
    // Pre-populate the availableDescriptorSets stack:
    for (auto& descSet : localDescriptorSets) 
    {
        availableDescriptorSets.push(descSet);
    }
}
void IHCEngine::Graphics::GraphicsManager::Update(IHCEngine::Core::Scene* scene)
{
    glfwPollEvents();
    //IHCEngine::Core::Time::GetInstance().Update(); // windowsize change need recheck
    
    // Render
    std::map<unsigned int, IHCEngine::Core::GameObject*> gameObjects
        = scene->GetGameObjectsMap();
    if (auto commandBuffer = renderer->BeginFrame())
    {
        int frameIndex = renderer->GetFrameIndex();

        FrameInfo frameInfo
        {
            frameIndex,
            IHCEngine::Core::Time::GetInstance().GetDeltaTime(),
            commandBuffer,
            globalDescriptorSets[frameIndex],
            textureToDescriptorSetsMap,
            gameObjects
        };

        GlobalUniformBufferObject ubo{};

        // Vulkan uses a right-handed coordinate system by default.
        // Y points down
        // Z points out of screen

        //ubo.viewMatrix = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //ubo.projectionMatrix = glm::perspective(
        //    glm::radians(45.0f),
        //    renderer->GetAspectRatio(), //swapChainExtent.width / (float)swapChainExtent.height,
        //    0.1f,
        //    10.0f);

        //Camera camera{CameraType::PERSPECTIVE,
        //    glm::radians(45.0f),
        //    renderer->GetAspectRatio(),
        //    0.1f,
        //    10.0f,
        //    800,
        //    600
        //};
        auto camera = scene->GetCamera();
        camera.SetAspectRatio(renderer->GetAspectRatio());

        glm::vec3 eyePosition = glm::vec3(5.0f, 5.0f, 5.0f);
        glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 0.0f, 1.0f);
        //camera.transform.SetWorldPosition(eyePosition);
        glm::vec3 direction = glm::normalize(targetPosition - eyePosition);
        glm::quat rotation = glm::quatLookAt(direction, upVector);  // Generate the quaternion rotation from the look direction and the up vector
        //camera.transform.SetWorldRotation(rotation);  // Set the camera's rotation

        ubo.viewMatrix = camera.GetViewMatrix();
        ubo.projectionMatrix = camera.GetProjectionMatrix();
        ubo.inverseViewMatrix = camera.GetInverseViewMatrix();
        ubo.projectionMatrix[1][1] *= -1; // Flip the Y-axis for vulkan <-> opengl

        uboBuffers[frameIndex]->WriteToBuffer(&ubo);
        uboBuffers[frameIndex]->Flush(); // Manual flush, can comment out if using VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 

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

#pragma region Helpers for assetManagement (texture, model)
std::unique_ptr<IHCEngine::Graphics::IHCTexture> IHCEngine::Graphics::GraphicsManager::CreateTexture(std::string assetName, std::string path)
{
    auto texture = std::make_unique<IHCEngine::Graphics::IHCTexture>(*ihcDevice, assetName, path);

    // check if already allocate  descriptorsets for the texture;
    if (textureToDescriptorSetsMap.find(assetName) != textureToDescriptorSetsMap.end())
    {
        assert("Loading Duplicated Texture into memory, abort");
        return nullptr;
    }
    // allocate MAX_FRAMES_IN_FLIGHT descriptorsets for  1 textrue
    std::vector<VkDescriptorSet> descriptorSetsForTexture;
    for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (availableDescriptorSets.empty())
        {
            assert("No available descriptor sets for allocation. Check if exceed poolsize");
        }

        VkDescriptorSet descriptor = availableDescriptorSets.top();
        availableDescriptorSets.pop();

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture->GetTextureImageView();
        imageInfo.sampler = texture->GetTextureSampler();

        IHCDescriptorWriter(*localDescriptorSetLayout, *localDescriptorPool)
            .WriteImage(0, &imageInfo)// Sampler
            .Build(descriptor);

        descriptorSetsForTexture.push_back(descriptor);
    }
    // Add the collection of descriptor sets to the map.
    textureToDescriptorSetsMap[assetName] = descriptorSetsForTexture;
    return texture;
}
void IHCEngine::Graphics::GraphicsManager::DestroyTexture(std::string assetName)
{
    // All submitted commands that refer to sampler must have completed execution
    vkDeviceWaitIdle(ihcDevice->GetDevice());
    // Check if the textureID exists in the map.
    auto it = textureToDescriptorSetsMap.find(assetName);
    if (it == textureToDescriptorSetsMap.end())
    {
        std::cerr << "Texture not found in descriptor map." << std::endl;
        return;
    }

    // Push all its descriptor sets as available.
    for (VkDescriptorSet descSet : it->second)
    {
        availableDescriptorSets.push(descSet);
    }
    // Remove the textureID from the map.
    textureToDescriptorSetsMap.erase(it);
}
std::unique_ptr<IHCEngine::Graphics::IHCModel> IHCEngine::Graphics::GraphicsManager::CreateModel(std::string assetName, std::string path)
{
    // Don't need to keep track for models, they self-deallocate
    // Textures need keeping track due to descriptors
    return IHCModel::CreateModelFromFile(*ihcDevice, path);
}
std::unique_ptr<IHCEngine::Graphics::IHCModel> IHCEngine::Graphics::GraphicsManager::CreateModel(std::string assetName, IHCEngine::Graphics::IHCModel::Builder& builder)
{
    // Don't need to keep track for models, they self-deallocate
    // Textures need keeping track due to descriptors
    return std::make_unique<IHCEngine::Graphics::IHCModel>(*ihcDevice, builder);
}
void IHCEngine::Graphics::GraphicsManager::DestroyModel(std::string assetName)
{
    // Don't need to keep track for models
    // just created for same format
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