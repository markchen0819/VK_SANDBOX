#include "GraphicsManager.h"


void IHCEngine::Graphics::GraphicsManager::Init(std::unique_ptr<Window::AppWindow>& w)
{
    appWindow = *w;
    loadGameObjects();
    initVulkan();
}

void IHCEngine::Graphics::GraphicsManager::initVulkan()
{
    ihcDevice = std::make_unique<IHCEngine::Graphics::IHCDevice>(appWindow);
    renderer = std::make_unique<IHCEngine::Graphics::Renderer>(appWindow, ihcDevice);

    uboBuffers.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++)
    {
        uboBuffers[i] = std::make_unique<IHCBuffer>(
            ihcDevice,
            sizeof(GlobalUniformBufferObject),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->Map();
    }


    basicRenderSystem = std::make_unique<IHCEngine::Graphics::RenderSystem>
        (ihcDevice, renderer->GetSwapChainRenderPass());
    //     globalSetLayout->getDescriptorSetLayout() };




    // old
    //createDescriptorSetLayout(); // Creates a layout for descriptor sets. The descriptor set layout describes the types of resources that will be accessed by the pipeline stages, and in what format and order.
    //createTextureImage(); // Creates a texture image. This is used to store the pixel data of our texture.
    //createTextureImageView(); // Creates a view into our texture image. This describes how to access the texture image and which part of the image to access.
    //createTextureSampler(); // Creates a texture sampler. This is used to read color data from the texture image in the shaders.

    //createUniformBuffers(); // Creates uniform buffers. These are used to store data that remains constant across all vertices in a draw call, such as transformation matrices.
    //createDescriptorPool(); // Creates a descriptor pool. This is where we allocate descriptor sets, which are used to pass resources like buffers and images to the shaders.
    //createDescriptorSets(); // Creates descriptor sets. Each set contains descriptors that reference the buffers and images the shaders will access.
}
void IHCEngine::Graphics::GraphicsManager::Update()
{
    glfwPollEvents();
    //IHCEngine::Core::Time::Update(); // windowsize change etc might need recheck

    // Render
    if (auto commandBuffer = renderer->BeginFrame())
    {
        int frameIndex = renderer->GetFrameIndex();

        FrameInfo frameInfo{
            frameIndex,
            IHCEngine::Core::Time::GetDeltaTime(),
            commandBuffer,
            camera,
            globalDescriptorSets[frameIndex],
            gameObjects };

        GlobalUniformBufferObject ubo{};
        ubo.projectionMatrix = camera.GetProjection();
        ubo.viewMatrix = camera.GetView();
        ubo.inverseViewMatrix = camera.GetInverseView();

        uboBuffers[frameIndex]->WriteToBuffer(&ubo);
        uboBuffers[frameIndex]->Flush(); // Manual flush

        renderer->BeginSwapChainRenderPass(commandBuffer);
        //// Order here matters ////

        basicRenderSystem->RenderGameObjects(frameInfo);
        //simpleRenderSystem.renderGameObjects(frameInfo);
        //pointLightSystem.render(frameInfo);
        
        ////////////////////////////
        renderer->EndSwapChainRenderPass(commandBuffer);
        renderer->EndFrame();
    }
}
void IHCEngine::Graphics::GraphicsManager::Shutdown()
{
    vkDeviceWaitIdle(ihcDevice->GetDevice()); // sync then allowed to destroy
    //cleanup();
}

void IHCEngine::Graphics::GraphicsManager::cleanup()
{


    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }

    vkDestroySampler(device, textureSampler, nullptr);
    vkDestroyImageView(device, textureImageView, nullptr);
    vkDestroyImage(device, textureImage, nullptr);
    vkFreeMemory(device, textureImageMemory, nullptr);

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

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
