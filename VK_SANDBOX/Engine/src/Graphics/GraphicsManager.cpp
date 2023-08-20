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
    //ihcModel = std::make_unique<IHCEngine::Graphics::IHCModel>(ihcDevice, );

    basicRenderSystem = std::make_unique<IHCEngine::Graphics::RenderSystem>
        (ihcDevice, renderer->GetSwapChainRenderPass());
    //     globalSetLayout->getDescriptorSetLayout() };


    // old
    //createDescriptorSetLayout(); // Creates a layout for descriptor sets. The descriptor set layout describes the types of resources that will be accessed by the pipeline stages, and in what format and order.
    //createTextureImage(); // Creates a texture image. This is used to store the pixel data of our texture.
    //createTextureImageView(); // Creates a view into our texture image. This describes how to access the texture image and which part of the image to access.
    //createTextureSampler(); // Creates a texture sampler. This is used to read color data from the texture image in the shaders.
    //createIndexBuffer(); // Creates an index buffer. This is used to store the index data of our model, which makes rendering more efficient by reusing vertices that appear multiple times.
    //createUniformBuffers(); // Creates uniform buffers. These are used to store data that remains constant across all vertices in a draw call, such as transformation matrices.
    //createDescriptorPool(); // Creates a descriptor pool. This is where we allocate descriptor sets, which are used to pass resources like buffers and images to the shaders.
    //createDescriptorSets(); // Creates descriptor sets. Each set contains descriptors that reference the buffers and images the shaders will access.
}
void IHCEngine::Graphics::GraphicsManager::Update()
{
    glfwPollEvents();

    // Render
    if (auto commandBuffer = renderer->BeginFrame())
    {
        int frameIndex = renderer->GetFrameIndex();
        //FrameInfo frameInfo{
        //    frameIndex,
        //    frameTime,
        //    commandBuffer,
        //    camera,
        //    globalDescriptorSets[frameIndex],
        //    gameObjects };

        //// update
        //GlobalUbo ubo{};
        //ubo.projection = camera.getProjection();
        //ubo.view = camera.getView();
        //ubo.inverseView = camera.getInverseView();
        //pointLightSystem.update(frameInfo, ubo);
        //uboBuffers[frameIndex]->writeToBuffer(&ubo);
        //uboBuffers[frameIndex]->flush();

        
        renderer->BeginSwapChainRenderPass(commandBuffer);
        //// Order here matters ////

        basicRenderSystem->RenderGameObjects();
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



