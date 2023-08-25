#include "GraphicsManager.h"

IHCEngine::Graphics::GraphicsManager::GraphicsManager(std::unique_ptr<Window::AppWindow>& w)
    : appWindow(*w)
{
}

void IHCEngine::Graphics::GraphicsManager::Init()
{
    initVulkan();
}
void IHCEngine::Graphics::GraphicsManager::initVulkan()
{
    // Create VK instance 
    ihcDevice = std::make_unique<IHCEngine::Graphics::IHCDevice>(appWindow);

    // Create Swapchain to render to screen
    renderer = std::make_unique<IHCEngine::Graphics::Renderer>(appWindow, *ihcDevice);

    // Create UniformBuffers for updating shaders
    // These are used to store data that remains constant 
    // across all vertices in a draw call
    // such as transformation matrices, light positions, 
    // or other data that's consistent across a single draw call or frame
    uboBuffers.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++)
    {
        uboBuffers[i] = std::make_unique<IHCBuffer>(
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

    ////////// Create textures () (NEED REDESIGN HERE) ////////////////////
    loadGameObjects();

    // To referring resources (UniformBuffers we just created) in the shaders
    // , we need to Create Descriptors 
    globalDescriptorPool =
        IHCDescriptorPool::Builder(*ihcDevice)
        .SetMaxSets(IHCSwapChain::MAX_FRAMES_IN_FLIGHT) // one ubo for each frame
        .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, IHCSwapChain::MAX_FRAMES_IN_FLIGHT)  // ubo
        .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IHCSwapChain::MAX_FRAMES_IN_FLIGHT) // sampler
        .Build();
    auto globalDescriptorSetLayout =
        IHCDescriptorSetLayout::Builder(*ihcDevice)
        .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)  // ubo //VK_SHADER_STAGE_VERTEX_BIT
        .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // sampler
        .Build();
    globalDescriptorSets.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++)
    {
        auto bufferInfo = uboBuffers[i]->GetDescriptorInfo();

        ////////// (NEED REDESIGN HERE) ////////////////////
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = testGobj->texture->GetTextureImageView();
        imageInfo.sampler = testGobj->texture->GetTextureSampler();


        IHCDescriptorWriter(*globalDescriptorSetLayout, *globalDescriptorPool)
            .WriteBuffer(0, &bufferInfo) // ubo
            .WriteImage(1, &imageInfo)// sampler
            .Build(globalDescriptorSets[i]);
    }

    // Create RenderSystems (Customizable),
    // each system can have one to many pipelines
    ////// Create basicRenderSystem //////
    // this system currently has
    // one pipeline for basic shading
    basicRenderSystem = std::make_unique<IHCEngine::Graphics::RenderSystem>
        (
            *ihcDevice, 
            renderer->GetSwapChainRenderPass(),
            globalDescriptorSetLayout->GetDescriptorSetLayout()
        );
    //Pipeline standardPipeline; //Standard Mode : Regular Phong shading with textures.
    //Pipeline nightVisionPipeline;  //Night Vision : Everything is rendered in green shades.
    //Pipeline postProcessBloomPipeline;   
    //wireframe Mode : For debugging, where all the objects are drawn
    ////// Create ParticleSystem //////
    ////// Create RasterSystem //////
    ////// Create RayTracingSystem //////
    ////// Create SimulationSystem //////

}
void IHCEngine::Graphics::GraphicsManager::Update()
{
    glfwPollEvents();
    //IHCEngine::Core::Time::GetInstance().Update(); // windowsize change etc might need recheck

    // Render
    if (auto commandBuffer = renderer->BeginFrame())
    {
        int frameIndex = renderer->GetFrameIndex();

        FrameInfo frameInfo
        {
            frameIndex,
            IHCEngine::Core::Time::GetInstance().GetDeltaTime(),
            commandBuffer,
            camera,
            globalDescriptorSets[frameIndex],
            gameObjects 
        };

        GlobalUniformBufferObject ubo{};

        ubo.projectionMatrix = camera.GetProjection();
        ubo.viewMatrix = camera.GetView();
        ubo.inverseViewMatrix = camera.GetInverseView();
        uboBuffers[frameIndex]->WriteToBuffer(&ubo);
        uboBuffers[frameIndex]->Flush(); // Manual flush, can comment out if using VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 

        renderer->BeginSwapChainRenderPass(commandBuffer);
        //// System Render order here matters ////

        basicRenderSystem->RenderGameObjects(frameInfo);
        //pointLightSystem.render(frameInfo);
        
        //////////////////////////////////////////
        renderer->EndSwapChainRenderPass(commandBuffer);
        renderer->EndFrame();
    }
}
void IHCEngine::Graphics::GraphicsManager::Shutdown()
{
    vkDeviceWaitIdle(ihcDevice->GetDevice()); // sync then allowed to destroy
}


//void updateUniformBuffer(uint32_t currentImage)
//{
//    static auto startTime = std::chrono::high_resolution_clock::now();
//    auto currentTime = std::chrono::high_resolution_clock::now();
//    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
//
//    UniformBufferObject ubo{};
//    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//    ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
//    ubo.proj[1][1] *= -1;
//
//    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
//
//}

void IHCEngine::Graphics::GraphicsManager::loadGameObjects()
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
    testGobj = std::make_unique<IHCEngine::Core::GameObject>();
    testGobj->model = testModel;
    testGobj->texture = testTexture;

    gameObjects.emplace(testGobj->GetUID(), testGobj.get());

    //testGobj.transform.GetLocalModelMatrix();
}