#include "../pch.h"
#include "IHCSwapChain.h"

IHCEngine::Graphics::IHCSwapChain::IHCSwapChain(IHCDevice& device, IHCEngine::Window::AppWindow& window)
    : device{ device }, window{ window }
{
    init();
}

IHCEngine::Graphics::IHCSwapChain::IHCSwapChain(IHCDevice& device, IHCEngine::Window::AppWindow& window, std::shared_ptr<IHCSwapChain> previous)
    : device{ device }, window{ window }, oldSwapChain{ previous }
{
    init();
    oldSwapChain = nullptr;
}

IHCEngine::Graphics::IHCSwapChain::~IHCSwapChain()
{
    cleanupSwapChain();

    // renderPass
    vkDestroyRenderPass(device.GetDevice(), renderPass, nullptr);

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device.GetDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device.GetDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device.GetDevice(), inFlightFences[i], nullptr);
    }
}

void IHCEngine::Graphics::IHCSwapChain::cleanupSwapChain()
{
    // swapChainImageViews
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device.GetDevice(), imageView, nullptr);
    }
    swapChainImageViews.clear();

    // swapChain
    if (swapChain != nullptr) {
        vkDestroySwapchainKHR(device.GetDevice(), swapChain, nullptr);
        swapChain = nullptr;
    }

    // depth
    for (int i = 0; i < depthImages.size(); i++)
    {
        vkDestroyImageView(device.GetDevice(), depthImageViews[i], nullptr);
        vkDestroyImage(device.GetDevice(), depthImages[i], nullptr);
        vkFreeMemory(device.GetDevice(), depthImageMemorys[i], nullptr);
    }

    // colors

    //vkDestroyImageView(device, colorImageView, nullptr);
    //vkDestroyImage(device, colorImage, nullptr);
    //vkFreeMemory(device, colorImageMemory, nullptr);


    // swapchain buffers
    for (auto framebuffer : swapChainFramebuffers)
    {
        vkDestroyFramebuffer(device.GetDevice(), framebuffer, nullptr);
    }
}

void IHCEngine::Graphics::IHCSwapChain::init()
{
    createSwapChain(); // Creates a swap chain. Collection of images rendered into and display on surface.
    createImageViews(); // Creates image views for the swap chain images. An image (how to access the image)
    createRenderPass(); // Describes the sequence of steps rendering graphics
    // including how many color and depth buffers, how many samples to use for each, and how to handle data throughout the rendering process.
    createColorResources(); // store the color buffer image for each frame
    createDepthResources(); // store the depth buffer image for each frame
    createFramebuffers(); // Creates a framebuffer for each image in the swap chain
    //The framebuffer will contain all the color, depth and stencil buffer images that shaders write to when we render a frame.
    createSyncObjects(); // Coordinate order of operations between multiple command buffers, to ensure that things happen in the right order.
}



#pragma region Create swapchain (Setup a multi-buffering system)
void IHCEngine::Graphics::IHCSwapChain::createSwapChain()
{
    // Query the swap chain support details for the physical device
    SwapChainSupportDetails swapChainSupport = device.GetSwapChainSupport();

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
    createInfo.surface = device.GetSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; // This is almost always 1, unless developing a stereoscopic 3D application
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // We'll be drawing directly to the images

    // Retrieve the queue families for graphics and presentation operations
    QueueFamilyIndices indices = device.FindPhysicalQueueFamilies();
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    // If graphics family and presentation family are different, images should be shared
    if (indices.graphicsFamily != indices.presentFamily) 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // Image can be used across multiple queue families without explicit ownership transfers
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Image is owned by one queue family at a time
        //createInfo.queueFamilyIndexCount = 0;      // Optional
        //createInfo.pQueueFamilyIndices = nullptr;  // Optional
    }
    // We don't want any transformation to be applied to images in the swap chain
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    // We don't want to blend with other windows in the window system
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE; // We don't care about the color of pixels that are obscured
    createInfo.oldSwapchain = (oldSwapChain == nullptr ? VK_NULL_HANDLE : oldSwapChain->swapChain);

    // Try to create the swap chain
    if (vkCreateSwapchainKHR(device.GetDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    // We only specified a minimum number of images in the swap chain, below allows to create a swap chain with more
    vkGetSwapchainImagesKHR(device.GetDevice(), swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device.GetDevice(), swapChain, &imageCount, swapChainImages.data());

    // Store
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}
VkSurfaceFormatKHR IHCEngine::Graphics::IHCSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}
VkPresentModeKHR IHCEngine::Graphics::IHCSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
    }
    std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D IHCEngine::Graphics::IHCSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else
    {

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(window.GetWidth()),
            static_cast<uint32_t>(window.GetHeight())
        };

        actualExtent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        //int width, height;
        //glfwGetFramebufferSize(window.GetWindowHandle(), &width, &height);
        //VkExtent2D actualExtent = {
        //    static_cast<uint32_t>(width),
        //    static_cast<uint32_t>(height)
        //};
        //actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        //actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
#pragma endregion

#pragma region Create ImageViews (Accessing the image)
#pragma endregion
void IHCEngine::Graphics::IHCSwapChain::createImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());
    for (uint32_t i = 0; i < swapChainImages.size(); i++)
    {
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}
VkImageView IHCEngine::Graphics::IHCSwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
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

void IHCEngine::Graphics::IHCSwapChain::recreateSwapChain()
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

