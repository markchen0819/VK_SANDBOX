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
    for (int i = 0; i < depthImages.size(); i++)
    {
        vkDestroyImageView(device.GetDevice(), colorImageViews[i], nullptr);
        vkDestroyImage(device.GetDevice(), colorImages[i], nullptr);
        vkFreeMemory(device.GetDevice(), colorImageMemorys[i], nullptr);
    }

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

#pragma region Create ImageViews (Accessing the image) & color, depth images
// create memory for image (swapchain not using)
void IHCEngine::Graphics::IHCSwapChain::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // imageInfo.flags = 0;

    if (vkCreateImage(device.GetDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device.GetDevice(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device.GetDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }
    if (vkBindImageMemory(device.GetDevice(), image, imageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }
}
// create single imageview (separated for swapchain & color & depth & texture)
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
    if (vkCreateImageView(device.GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}
// swapchain images
void IHCEngine::Graphics::IHCSwapChain::createImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());
    for (uint32_t i = 0; i < swapChainImages.size(); i++)
    {
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}
// msaa (color)
void IHCEngine::Graphics::IHCSwapChain::createColorResources()
{
    VkFormat colorFormat = swapChainImageFormat;
    VkExtent2D swapChainExtent = GetSwapChainExtent();

    colorImages.resize(GetImageCount());
    colorImageMemorys.resize(GetImageCount());
    colorImageViews.resize(GetImageCount());

    for (int i = 0; i < colorImages.size(); i++)
    {
        createImage(
            swapChainExtent.width,
            swapChainExtent.height,
            1,
            device.GetMsaaSamples(),
            colorFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
            colorImages[i],
            colorImageMemorys[i]);

        depthImageViews[i] = createImageView(
            colorImages[i],
            colorFormat,
            VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}
// depth images
void IHCEngine::Graphics::IHCSwapChain::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat();
    swapChainDepthFormat = depthFormat;
    VkExtent2D swapChainExtent = GetSwapChainExtent();

    depthImages.resize(GetImageCount());
    depthImageMemorys.resize(GetImageCount());
    depthImageViews.resize(GetImageCount());

    for (int i = 0; i < depthImages.size(); i++)
    {
        createImage(
            swapChainExtent.width, 
            swapChainExtent.height, 
            1, 
            device.GetMsaaSamples(),
            depthFormat, 
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            depthImages[i],
            depthImageMemorys[i]);

        depthImageViews[i] = createImageView(
            depthImages[i],
            depthFormat, 
            VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    }
}
VkFormat IHCEngine::Graphics::IHCSwapChain::findDepthFormat()
{
    return device.FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}
#pragma endregion

#pragma region Create RenderPass (sequence of steps rendering graphics)
// Steps breakdown:
// 1. Attachments (format & properties for Color, Depth, Resolve(multisampled images into single-sample images, anti-aliasing))
// 2. Attachment References (define how the attachment will be used)
// 3. Subpass
// 4. Dependencies (correct order of operations)
// 5. Creating the Render Pass
// RenderPass (rules ensuring that stages are completed), Pipeline (stages in order)
void IHCEngine::Graphics::IHCSwapChain::createRenderPass()
{
    //   Describe the properties of the color attachment.
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat; // Format to match the swap chain images
    colorAttachment.samples = device.GetMsaaSamples(); //VK_SAMPLE_COUNT_1_BIT; // No multisampling
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear the values to a constant at the start
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Rendered content will be stored in memory and can be read later
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // We don't care about stencil
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // We don't care about stencil
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // We don't care about initial layout
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Images to be presented in the swap chain

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = device.GetMsaaSamples(); //= VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Resolve for MSAA
    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = swapChainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Describe how the color attachment will be used in the subpass.
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0; // We have one attachment referenced by index 0
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Images will be used as color attachment

    //
    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    // Description of a subpass. A render pass can consist of multiple subpasses.
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // This is a graphics subpass
    subpass.colorAttachmentCount = 1; // We have one color attachment
    subpass.pColorAttachments = &colorAttachmentRef; // Pointer to the color attachment reference
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    // Declare a structure to hold information about subpass dependencies
    VkSubpassDependency dependency{};
    // The source subpass, which is the producer of the dependency. 
    // VK_SUBPASS_EXTERNAL is a special value that refers to operations happening before or after the render pass.
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    // The destination subpass, which is the consumer of the dependency. 
    // In this case, it's the first (and possibly only) subpass in the render pass.
    dependency.dstSubpass = 0;
    // The pipeline stage at which the source (producer) subpass generates the dependency. 
    // In this case, it's the stage where color data is written to an attachment (color buffer).
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    // A bitmask that specifies what types of memory operations at the source must complete before the dependency is satisfied.
    // Here it's set to 0, indicating no specific memory operations that need to complete.
    dependency.srcAccessMask = 0;
    // The pipeline stage at which the destination (consumer) subpass waits for the dependency. 
    // Here, it's also the stage where color data is written to an attachment.
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    // A bitmask that specifies what types of memory operations at the destination will wait for the dependency. 
    // Here, it's set to allow write operations to color attachments.
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    // Create the render pass with the given properties and subpass.
    std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO; // Structure type is for a render pass creation
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1; // We have one subpass
    renderPassInfo.pSubpasses = &subpass; // Pointer to the subpass
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device.GetDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}
#pragma endregion

#pragma region Create Framebuffers (collection of memory attachments organized and managed for reference)
void IHCEngine::Graphics::IHCSwapChain::createFramebuffers()
{
    swapChainFramebuffers.resize(GetImageCount());

    for (size_t i = 0; i < GetImageCount(); i++)
    {
        std::array<VkImageView, 3> attachments = {
         colorImageViews[i],
         depthImageViews[i],
         swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device.GetDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}
#pragma endregion

#pragma region Create SyncObjects (for Submission of Command Buffers to a Queue) 
void IHCEngine::Graphics::IHCSwapChain::createSyncObjects()
{
    // Resizing the synchronization containers to the maximum number of concurrent frames our application will support.
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    // Resizing the tracking list of which frame is currently using which swap chain image.
    imagesInFlight.resize(GetImageCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    // Fences will start in the signaled state, meaning that the CPU can wait for a fence 
   // even if the GPU hasn't signaled it yet, preventing potential deadlock scenarios.


    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(device.GetDevice(), &semaphoreInfo,
            nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS // Creating a semaphore for when an image is available to be rendered to.
            || vkCreateSemaphore(device.GetDevice(), &semaphoreInfo,
                nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS // Creating a semaphore for when rendering to an image is finished 
            || vkCreateFence(device.GetDevice(), &fenceInfo,
                nullptr, &inFlightFences[i]) != VK_SUCCESS) // Creating a fence for GPU to signal and CPU to wait.
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}
#pragma endregion

#pragma region DrawFrame, submitting CommandBuffers (using syncObjects)
VkResult IHCEngine::Graphics::IHCSwapChain::SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex)
{
    // Check if an Image is Still in Flight, Wait for the Image to be Free, Mark the Image as in Use by the Current Frame:
    if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE)
    {
        vkWaitForFences(device.GetDevice(), 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
    }
    imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

    //VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    //if (result == VK_ERROR_OUT_OF_DATE_KHR)
    //{
    //    recreateSwapChain();
    //    return;
    //}
    //else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    //{
    //    throw std::runtime_error("failed to acquire swap chain image!");
    //}
    //updateUniformBuffer(currentFrame);

    // Only reset the fence if we are submitting work
    vkResetFences(device.GetDevice(), 1, &inFlightFences[currentFrame]);

    //vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    //recordCommandBuffer(commandBuffers[currentFrame], imageIndex);


    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffers; // &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo,
        inFlightFences[currentFrame]) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndex;
    presentInfo.pResults = nullptr; // Optional

    auto result = vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);

    //bool framebufferResized = appWindow->IsWindowResized();
    //if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    //{
    //    framebufferResized = false;
    //    recreateSwapChain();
    //}
    //else if (result != VK_SUCCESS)
    //{
    //    throw std::runtime_error("failed to present swap chain image!");
    //}

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}
VkResult IHCEngine::Graphics::IHCSwapChain::AcquireNextImage(uint32_t* imageIndex)
{
    // CPU wait
    vkWaitForFences(
        device.GetDevice(),
        1,
        &inFlightFences[currentFrame],
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(
        device.GetDevice(),
        swapChain,
        std::numeric_limits<uint64_t>::max(),
        imageAvailableSemaphores[currentFrame],  // must be a not signaled semaphore
        VK_NULL_HANDLE,
        imageIndex);
    return result;
}
#pragma endregion

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

//void IHCEngine::Graphics::IHCSwapChain::recreateSwapChain()
//{
//    // window minimize
//    int width = 0, height = 0;
//
//    GLFWwindow* window = appWindow->GetWindowHandle();
//
//    glfwGetFramebufferSize(window, &width, &height);
//    while (width == 0 || height == 0) {
//        glfwGetFramebufferSize(window, &width, &height);
//        glfwWaitEvents();
//    }
//    // window screen size change
//    vkDeviceWaitIdle(device);
//    cleanupSwapChain();
//    createSwapChain();
//    createImageViews();
//    createColorResources();
//    createDepthResources();
//    createFramebuffers();
//}

