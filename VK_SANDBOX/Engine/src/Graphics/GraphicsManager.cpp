#include "GraphicsManager.h"

IHCEngine::Graphics::GraphicsManager::GraphicsManager(std::unique_ptr<Window::AppWindow>& w)
    : appWindow(*w)
{}

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

    loadGameObjects();

    ////// Create a basic shading pipeline //////
    //// Set up shader interface (DescriptorSetLayout (ubo, sampler), PipelineLayout)
    auto globalDescriptorSetLayout =
        IHCDescriptorSetLayout::Builder(*ihcDevice)
        .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)  // Assuming this is for vertex shaders
        .Build();
    auto localDescriptorSetLayout =
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
    //// Allocate memory (ubo, sampler)
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
    // Allocate textures (TO DO: local textures can be allocated other places to support runtime)
    int N = textures.size();
    globalDescriptorPool =
        IHCDescriptorPool::Builder(*ihcDevice)
        .SetMaxSets(IHCSwapChain::MAX_FRAMES_IN_FLIGHT)
        .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, IHCSwapChain::MAX_FRAMES_IN_FLIGHT)
        .Build();
    localDescriptorPool =
        IHCDescriptorPool::Builder(*ihcDevice)
        .SetMaxSets(N * IHCSwapChain::MAX_FRAMES_IN_FLIGHT)
        .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, N * IHCSwapChain::MAX_FRAMES_IN_FLIGHT) // sampler
        .Build();
    globalDescriptorSets.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
    localDescriptorSets.resize(N * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
    // Allocate global descriptor sets
    for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto bufferInfo = uboBuffers[i]->GetDescriptorInfo();
        IHCDescriptorWriter(*globalDescriptorSetLayout, *globalDescriptorPool)
            .WriteBuffer(0, &bufferInfo)  // UBO
            .Build(globalDescriptorSets[i]);
    }
    // Allocate local descriptor sets
    int descriptorIndex = 0;
    for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
    {
        for (const auto& pair : textures)
        {
            std::cout << "Key: " << pair.first << " Value: " << pair.second << '\n';

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = pair.second->GetTextureImageView();
            imageInfo.sampler = pair.second->GetTextureSampler();


            IHCDescriptorWriter(*localDescriptorSetLayout, *localDescriptorPool)
                .WriteImage(0, &imageInfo)// Sampler
                .Build(localDescriptorSets[descriptorIndex]);

            // Associate each game object using this texture to its descriptor set
            for (auto& g : gameObjects)
            {
                if (g.second->texture == pair.second)
                {
                    gameObjectToDescriptorSet[g.second] = localDescriptorSets[descriptorIndex];
                }
            }
            descriptorIndex++;
        }
    }

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
        //ubo.modelMatrix = glm::rotate(glm::mat4(1.0f), IHCEngine::Core::Time::GetInstance().GetElapsedTime() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.viewMatrix = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.projectionMatrix = glm::perspective(
            glm::radians(45.0f),
            renderer->GetAspectRatio(), //swapChainExtent.width / (float)swapChainExtent.height,
            0.1f, 
            10.0f);
        ubo.projectionMatrix[1][1] *= -1;
        //ubo.projectionMatrix = camera.GetProjection();
        //ubo.viewMatrix = camera.GetView();
        ubo.inverseViewMatrix = camera.GetInverseView();

        uboBuffers[frameIndex]->WriteToBuffer(&ubo);
        uboBuffers[frameIndex]->Flush(); // Manual flush, can comment out if using VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 

        renderer->BeginSwapChainRenderPass(commandBuffer);
        //// System Render order here matters ////

        basicRenderSystem->RenderGameObjects(frameInfo, gameObjectToDescriptorSet);
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



void IHCEngine::Graphics::GraphicsManager::loadGameObjects()
{
    std::shared_ptr<IHCModel> testModel =
        IHCModel::CreateModelFromFile
        (
            *ihcDevice,
            "Engine/assets/models/viking_room/viking_room.obj"
        );
    std::shared_ptr<IHCTexture> testTexture1 =
        std::make_shared < IHCTexture>
        (
            *ihcDevice,
            "Engine/assets/models/viking_room/viking_room.png"
        );
    std::shared_ptr<IHCTexture> testTexture2 =
        std::make_shared < IHCTexture>
        (
            *ihcDevice,
            "Engine/assets/models/viking_room/viking_room_2.png"
        );


    textures.emplace(1, testTexture1);
    textures.emplace(2, testTexture2);

    testGobj1 = std::make_unique<IHCEngine::Core::GameObject>();
    testGobj1->model = testModel;
    testGobj1->texture = testTexture1;

    testGobj2 = std::make_unique<IHCEngine::Core::GameObject>();
    testGobj2->model = testModel;
    testGobj2->texture = testTexture2;

    gameObjects.emplace(testGobj1->GetUID(), testGobj1.get());
    gameObjects.emplace(testGobj2->GetUID(), testGobj2.get());

}