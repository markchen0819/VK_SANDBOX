#include "RenderSystem.h"

//// create & destroy vkDebug obj (extenstion not auto-loaded, need vkGetInstanceProcAddr)
//VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
//{
//    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
//    if (func != nullptr)
//    {
//        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
//    }
//    else
//    {
//        return VK_ERROR_EXTENSION_NOT_PRESENT;
//    }
//}
//void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
//{
//    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
//    if (func != nullptr)
//    {
//        func(instance, debugMessenger, pAllocator);
//    }
//}

void IHCEngine::Graphics::RenderSystem::Init(std::unique_ptr<Window::AppWindow>& w) 
{
    appWindow = *w;
    initVulkan();
}


void IHCEngine::Graphics::RenderSystem::Update()
{
    glfwPollEvents();
    DrawFrame();
    //drawFrame();
}

void IHCEngine::Graphics::RenderSystem::Shutdown()
{
    vkDeviceWaitIdle(ihcDevice->GetDevice()); // sync then allowed to destroy
    //cleanup();
}

void IHCEngine::Graphics::RenderSystem::initVulkan()
{

    ihcDevice = std::make_unique<IHCEngine::Graphics::IHCDevice>(appWindow);
    ihcSwapChain = std::make_unique<IHCEngine::Graphics::IHCSwapChain>(ihcDevice, appWindow);
    createPipelineLayout(); // (CPU - shader stages, uniform buffers, samplers, and push constants)
    createPipeline(); // ihcPipeline 
    createCommandBuffers(); // record all the commands we want to execute


    //createInstance(); // connection to the Vulkan library
    //setupDebugMessenger(); // debug messenger for Vulkan.


    //createSurface(); // surface render onto (e.g., a window in an operating system).
    //pickPhysicalDevice(); // Selects a physical device (GPU) to use. depend on what types of features we need, like certain types of textures or shaders.
    //createLogicalDevice(); // Creates a logical device. communicate with the physical device.


    //createSwapChain(); // Creates a swap chain. Collection of images rendered into and display on surface.
    //createImageViews(); // Creates image views for the swap chain images. An image view describes how to access the image and which part of the image to access.
   // createRenderPass(); // Creates a render pass. A render pass describes the sequence of steps that will occur when rendering graphics, including how many color and depth buffers there will be, how many samples to use for each, and how to handle data throughout the rendering process.
    createDescriptorSetLayout(); // Creates a layout for descriptor sets. The descriptor set layout describes the types of resources that will be accessed by the pipeline stages, and in what format and order.
    createGraphicsPipeline(); // Creates a graphics pipeline. This object encapsulates all the state related to rendering, including all the shader stages, fixed-function pipeline configurations, and the render pass description.
    //createColorResources(); // Creates color image resources. These resources are used to store the color buffer image for each frame.
    //createDepthResources(); // Creates depth image resources. These resources are used to store the depth buffer image for each frame.
    createFramebuffers(); // Creates a framebuffer for each image in the swap chain. The framebuffer will contain all the color, depth and stencil buffer images that shaders write to when we render a frame.
    //createCommandPool(); // Creates a command pool. This is where we allocate command buffers, which are used to record rendering commands that we'll send to the GPU.
    createTextureImage(); // Creates a texture image. This is used to store the pixel data of our texture.
    createTextureImageView(); // Creates a view into our texture image. This describes how to access the texture image and which part of the image to access.
    createTextureSampler(); // Creates a texture sampler. This is used to read color data from the texture image in the shaders.
    loadModel(); // Loads the 3D model we want to render. This involves loading the model from disk, processing it into a format suitable for rendering, and storing it in memory.
    createVertexBuffer(); // Creates a vertex buffer. This is used to store the vertex data of our model, and it's uploaded to GPU memory so that the GPU can access it when rendering.
    createIndexBuffer(); // Creates an index buffer. This is used to store the index data of our model, which makes rendering more efficient by reusing vertices that appear multiple times.
    createUniformBuffers(); // Creates uniform buffers. These are used to store data that remains constant across all vertices in a draw call, such as transformation matrices.
    createDescriptorPool(); // Creates a descriptor pool. This is where we allocate descriptor sets, which are used to pass resources like buffers and images to the shaders.
    createDescriptorSets(); // Creates descriptor sets. Each set contains descriptors that reference the buffers and images the shaders will access.
    //createCommandBuffers(); // Creates command buffers. We record all the commands we want to execute, including rendering commands, into these buffers, and then submit them to the GPU.
    createSyncObjects(); // Creates synchronization objects. These are used to coordinate the order of operations between multiple command buffers, to ensure that things happen in the right order.
}
void IHCEngine::Graphics::RenderSystem::cleanup()
{
    // NEW CODE HERE /////////////////////////////
    vkDestroyPipelineLayout(ihcDevice->GetDevice(), pipelineLayout, nullptr);




    //clean VK
    //cleanupSwapChain();

    //vkDestroyPipeline(device, graphicsPipeline, nullptr);
    //vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    //vkDestroyRenderPass(device, renderPass, nullptr);

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

    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);


    //for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    //{
    //    vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
    //    vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
    //    vkDestroyFence(device, inFlightFences[i], nullptr);
    //}


    //vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    //vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    //vkDestroyFence(device, inFlightFence, nullptr);


    //vkDestroyCommandPool(device, commandPool, nullptr);

    //vkDestroyDevice(device, nullptr);
    //if (enableValidationLayers)
    //{
    //    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    //}
    //vkDestroySurfaceKHR(instance, surface, nullptr);
    //vkDestroyInstance(instance, nullptr);

}




// NEW CODE HERE///////////////////////////////////////////////
#pragma region Create PipelineLayout (for shaders interface) & Pipeline
void IHCEngine::Graphics::RenderSystem::createPipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    //pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(ihcDevice->GetDevice(), 
        &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}
void IHCEngine::Graphics::RenderSystem::createPipeline()
{
    auto pipelineConfig = IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo
    (ihcSwapChain->GetWidth(), appWindow.GetHeight(), *ihcDevice);
    // appWindow.GetWidth() appWindow.GetHeight() might not match

    pipelineConfig.renderPass = ihcSwapChain->GetRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;

    ihcPipeline = std::make_unique<IHCEngine::Graphics::IHCPipeline>
        (ihcDevice,
            "Engine/assets/shaders/vert.spv",
            "Engine/assets/shaders/frag.spv",
            pipelineConfig
        );
}
#pragma endregion

#pragma region Create Command buffers (submit to swapchain execute on GPU) & record 
void IHCEngine::Graphics::RenderSystem::createCommandBuffers()
{
    commandBuffers.resize(ihcSwapChain->GetImageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;// can submit but cannot be called by other cmdbuffers
    allocInfo.commandPool = ihcDevice->GetCommandPool();
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(ihcDevice->GetDevice(), &allocInfo,
        commandBuffers.data()) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (int i = 0; i < commandBuffers.size(); ++i)
    {
        recordCommandBuffer(commandBuffers[i], i);
    }

}
void IHCEngine::Graphics::RenderSystem::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    // Start recording
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = ihcSwapChain->GetRenderPass();
    renderPassInfo.framebuffer = ihcSwapChain->GetFrameBuffer(imageIndex);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = ihcSwapChain->GetSwapChainExtent(); 

    // clear screen values
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    //clearValues[0].depthStencil = X //depth attachment is 1 not 0
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    ///////////////////////////////
    ////// Render pass start //////
    ///////////////////////////////
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    // Step 1: Bind pipeline
    ihcPipeline->Bind(commandBuffer);

    // Optional: dynamic viewport and scissor state
    // for this pipeline to be dynamic.
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)ihcSwapChain->GetWidth();
    viewport.height = (float)ihcSwapChain->GetHeight();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = ihcSwapChain->GetSwapChainExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // Step 2: setup vertices
    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
    
    // Step 3: Draw
    //vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    /////////////////////////////
    ////// Render pass end //////
    /////////////////////////////
    // End recording
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}
#pragma endregion

#pragma region DrawFrame
void IHCEngine::Graphics::RenderSystem::DrawFrame()
{
    uint32_t imageIndex;
    VkResult result = ihcSwapChain->AcquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    result = ihcSwapChain->SubmitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        
    bool framebufferResized = appWindow.IsWindowResized();
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    {
        framebufferResized = false;
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
}
#pragma endregion


#pragma region VK instance creation with debugging, extensions
//void IHCEngine::Graphics::RenderSystem::createInstance()
//{
//    if (enableValidationLayers && !checkValidationLayerSupport())
//    {
//        throw std::runtime_error("validation layers requested, but not available!");
//    }
//
//    // Info to create VK instance
//    VkApplicationInfo appInfo{};
//    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//    appInfo.pApplicationName = appWindow->GetName().c_str();
//    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//    appInfo.pEngineName = appWindow->GetName().c_str();
//    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//    appInfo.apiVersion = VK_API_VERSION_1_0;
//
//    VkInstanceCreateInfo createInfo{};
//    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//    createInfo.pApplicationInfo = &appInfo;
//
//    // Extensions
//    auto extensions = getRequiredExtensions();
//    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
//    createInfo.ppEnabledExtensionNames = extensions.data();
//
//    // Validation layer
//    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
//    if (enableValidationLayers)
//    {
//        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//        createInfo.ppEnabledLayerNames = validationLayers.data();
//
//        populateDebugMessengerCreateInfo(debugCreateInfo);
//        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
//    }
//    else
//    {
//        createInfo.enabledLayerCount = 0;
//
//        createInfo.pNext = nullptr;
//    }
//
//    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
//    {
//        throw std::runtime_error("failed to create instance!");
//    }
//}
//bool IHCEngine::Graphics::RenderSystem::checkValidationLayerSupport()
//{
//    uint32_t layerCount;
//    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
//
//    std::vector<VkLayerProperties> availableLayers(layerCount);
//    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
//
//    for (const char* layerName : validationLayers)
//    {
//        bool layerFound = false;
//        for (const auto& layerProperties : availableLayers)
//        {
//            if (strcmp(layerName, layerProperties.layerName) == 0) {
//                layerFound = true;
//                break;
//            }
//        }
//        if (!layerFound) {
//            return false;
//        }
//    }
//    return true;
//}
//std::vector<const char*> IHCEngine::Graphics::RenderSystem::getRequiredExtensions()
//{
//    uint32_t glfwExtensionCount = 0;
//    const char** glfwExtensions;
//    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
//
//    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
//    if (enableValidationLayers)
//    {
//        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//    }
//
//    return extensions;
//}
//VKAPI_ATTR VkBool32 VKAPI_CALL IHCEngine::Graphics::RenderSystem::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
//{
//    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
//    return VK_FALSE;
//}
//void IHCEngine::Graphics::RenderSystem::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
//{
//    createInfo = {};
//    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
//    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
//    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
//    createInfo.pfnUserCallback = debugCallback;
//}
//void IHCEngine::Graphics::RenderSystem::setupDebugMessenger()
//{
//    if (!enableValidationLayers) return;
//    VkDebugUtilsMessengerCreateInfoEXT createInfo;
//    populateDebugMessengerCreateInfo(createInfo);
//
//    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
//        throw std::runtime_error("failed to set up debug messenger!");
//    }
//}
#pragma endregion

#pragma region Create surface to render, pick physical and create logical devices 
//void IHCEngine::Graphics::RenderSystem::createSurface()
//{
//    if (glfwCreateWindowSurface(instance, appWindow->GetWindowHandle(), nullptr,
//        &surface) != VK_SUCCESS)
//    {
//        throw std::runtime_error("failed to create window surface!");
//    }
//}
//// pick one that supports rendering & presenting onto the screen (queues)
//// & supports swapchain
//void IHCEngine::Graphics::RenderSystem::pickPhysicalDevice()
//{
//    uint32_t deviceCount = 0;
//    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
//    if (deviceCount == 0) 
//    {
//        throw std::runtime_error("failed to find GPUs with Vulkan support!");
//    }
//    std::vector<VkPhysicalDevice> devices(deviceCount);
//    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
//
//    for (const auto& device : devices)
//    {
//        if (isDeviceSuitable(device)) 
//        {
//            physicalDevice = device;
//            msaaSamples = getMaxUsableSampleCount();
//            break;
//        }
//    }
//
//    if (physicalDevice == VK_NULL_HANDLE) 
//    {
//        throw std::runtime_error("failed to find a suitable GPU!");
//    }
//}
//bool IHCEngine::Graphics::RenderSystem::isDeviceSuitable(VkPhysicalDevice device) // check if physical device supports swapchain
//{
//    QueueFamilyIndices indices = findQueueFamilies(device);
//    bool extensionsSupported = checkDeviceExtensionSupport(device);
//
//    bool swapChainAdequate = false;
//    if (extensionsSupported)
//    {
//        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
//        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
//    }
//
//    VkPhysicalDeviceFeatures supportedFeatures;
//    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
//
//    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
//}
//QueueFamilyIndices IHCEngine::Graphics::RenderSystem::findQueueFamilies(VkPhysicalDevice device)
//{
//    QueueFamilyIndices indices;
//
//    uint32_t queueFamilyCount = 0;
//    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
//
//    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
//
//    int i = 0;
//    for (const auto& queueFamily : queueFamilies)
//    {
//        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
//        {
//            indices.graphicsFamily = i;
//        }
//
//        VkBool32 presentSupport = false;
//        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
//        if (presentSupport)
//        {
//            indices.presentFamily = i;
//        }
//
//
//        if (indices.isComplete()) {
//            break;
//        }
//
//        i++;
//    }
//
//    return indices;
//}
//bool IHCEngine::Graphics::RenderSystem::checkDeviceExtensionSupport(VkPhysicalDevice device)
//{
//    uint32_t extensionCount;
//    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
//    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
//    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
//
//    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
//    for (const auto& extension : availableExtensions)
//    {
//        requiredExtensions.erase(extension.extensionName);
//    }
//
//    return requiredExtensions.empty();
//}
//SwapChainSupportDetails IHCEngine::Graphics::RenderSystem::querySwapChainSupport(VkPhysicalDevice device)
//{
//    SwapChainSupportDetails details;
//
//    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
//
//    uint32_t formatCount;
//    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
//    if (formatCount != 0)
//    {
//        details.formats.resize(formatCount);
//        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
//    }
//
//    uint32_t presentModeCount;
//    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
//    if (presentModeCount != 0)
//    {
//        details.presentModes.resize(presentModeCount);
//        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
//    }
//
//    return details;
//}
//VkSampleCountFlagBits IHCEngine::Graphics::RenderSystem::getMaxUsableSampleCount()
//{
//    VkPhysicalDeviceProperties physicalDeviceProperties;
//    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
//
//    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
//    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
//    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
//    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
//    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
//    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
//    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
//
//    return VK_SAMPLE_COUNT_1_BIT;
//}
//
//void IHCEngine::Graphics::RenderSystem::createLogicalDevice()
//{
//    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
//
//    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
//    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
//
//
//    float queuePriority = 1.0f;
//
//    // Create queue create info for each unique queue family
//    for (uint32_t queueFamily : uniqueQueueFamilies)
//    {
//        VkDeviceQueueCreateInfo queueCreateInfo{};
//        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//        queueCreateInfo.queueFamilyIndex = queueFamily;
//        queueCreateInfo.queueCount = 1;
//        queueCreateInfo.pQueuePriorities = &queuePriority;
//        queueCreateInfos.push_back(queueCreateInfo);
//    }
//    // Specify device features
//    VkPhysicalDeviceFeatures deviceFeatures{};
//    deviceFeatures.samplerAnisotropy = VK_TRUE; //  optional device feature
//    // Create the logical device
//    deviceFeatures.sampleRateShading = VK_TRUE; // enable sample shading feature for the device
//    VkDeviceCreateInfo createInfo{};
//    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
//    createInfo.pQueueCreateInfos = queueCreateInfos.data();
//    createInfo.pEnabledFeatures = &deviceFeatures;
//    //createInfo.enabledExtensionCount = 0;
//
//    // IMPORTANT TO PUT THESE!
//    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
//    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
//    // Add any required device extensions
//    if (enableValidationLayers)
//    {
//        //older implementations
//        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//        createInfo.ppEnabledLayerNames = validationLayers.data();
//    }
//    else {
//        createInfo.enabledLayerCount = 0;
//    }
//
//    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create logical device!");
//    }
//
//    // Get handles to the queues
//    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
//    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
//}
#pragma endregion

#pragma region Create swapchain and imageviews for accessing
void IHCEngine::Graphics::RenderSystem::createSwapChain()
{
    // Query the swap chain support details for the physical device
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    // Choose the surface format, present mode, and extent from the available options
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    // Request at least one more image than the minimum, to prevent waiting on driver to complete internal operations before we can acquire another image to render to.
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    // If maxImageCount is 0, there's no maximum. Otherwise, ensure we don't exceed the maximum
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // Fill in the structure for swap chain creation info
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; // This is almost always 1, unless developing a stereoscopic 3D application
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // We'll be drawing directly to the images

    // Retrieve the queue families for graphics and presentation operations
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    // If graphics family and presentation family are different, images should be shared
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // Image can be used across multiple queue families without explicit ownership transfers
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Image is owned by one queue family at a time
    }

    // We don't want any transformation to be applied to images in the swap chain
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    // We don't want to blend with other windows in the window system
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE; // We don't care about the color of pixels that are obscured

    createInfo.oldSwapchain = VK_NULL_HANDLE; // We're not recreating the swap chain right now

    // Try to create the swap chain
    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    // Retrieve the images in the swap chain
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    // Store
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
};
VkSurfaceFormatKHR IHCEngine::Graphics::RenderSystem::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}
VkPresentModeKHR IHCEngine::Graphics::RenderSystem::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D IHCEngine::Graphics::RenderSystem::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;

        GLFWwindow* window = appWindow->GetWindowHandle();
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
void IHCEngine::Graphics::RenderSystem::createImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());
    for (uint32_t i = 0; i < swapChainImages.size(); i++)
    {
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}
VkImageView IHCEngine::Graphics::RenderSystem::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    //viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}
// window size change requires to recreate swap chain
void IHCEngine::Graphics::RenderSystem::recreateSwapChain()
{
    // window minimize
    int width = 0, height = 0;

    GLFWwindow* window = appWindow->GetWindowHandle();

    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }
    // window screen size change
    vkDeviceWaitIdle(device);
    cleanupSwapChain();
    createSwapChain();
    createImageViews();
    createColorResources();
    createDepthResources();
    createFramebuffers();
}
void IHCEngine::Graphics::RenderSystem::cleanupSwapChain()
{
    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);

    vkDestroyImageView(device, colorImageView, nullptr);
    vkDestroyImage(device, colorImage, nullptr);
    vkFreeMemory(device, colorImageMemory, nullptr);

    for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
        vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
    }

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        vkDestroyImageView(device, swapChainImageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}
#pragma endregion



#pragma region GraphicsPipeline
#pragma endregion