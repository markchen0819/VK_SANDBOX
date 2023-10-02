#include "../pch.h"
#include "Renderer.h"
#include "../Window/AppWindow.h"
#include "VKWraps/IHCDevice.h"

IHCEngine::Graphics::Renderer::Renderer(Window::AppWindow& window, IHCDevice& device)
	: appWindow{ window }, ihcDevice{ device }
{
	recreateSwapChain();
	createCommandBuffers();
}

IHCEngine::Graphics::Renderer::~Renderer()
{
	freeCommandBuffers();
}

#pragma region Recreate SwapChain (window size change, also affects pipeline)
void IHCEngine::Graphics::Renderer::recreateSwapChain()
{
    GLFWwindow* window = appWindow.GetWindowHandle();
    int width = appWindow.GetWidth();
    int height = appWindow.GetHeight();
    VkExtent2D extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    // program wait when window minimization
    while (extent.width == 0 || extent.height == 0)
    {
        // glfwGetFramebufferSize(window, &width, &height);
        extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
        glfwWaitEvents();
    }

    // wait for everything on the device to be finish
    vkDeviceWaitIdle(ihcDevice.GetDevice());

    // recreate
    if (ihcSwapChain == nullptr)
    {
        ihcSwapChain = std::make_unique<IHCEngine::Graphics::IHCSwapChain>(ihcDevice, appWindow);
    }
    else // reuse with oldSwapChain
    {
        std::shared_ptr<IHCSwapChain> oldSwapChain = std::move(ihcSwapChain); // transferring ownership
        ihcSwapChain = std::make_unique<IHCEngine::Graphics::IHCSwapChain>(ihcDevice, appWindow, oldSwapChain);
        if (!oldSwapChain->CompareSwapFormats(*ihcSwapChain.get()))
        {
            throw std::runtime_error("Swap chain image(or depth) format has changed!");
        }
    }
}
#pragma endregion

#pragma region Create Command buffers (used for submission to swapchain execute on GPU) 
void IHCEngine::Graphics::Renderer::createCommandBuffers()
{
    commandBuffers.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;// can submit but cannot be called by other cmdbuffers
    allocInfo.commandPool = ihcDevice.GetCommandPool();
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(ihcDevice.GetDevice(), &allocInfo,
        commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}
void IHCEngine::Graphics::Renderer::freeCommandBuffers()
{
    vkFreeCommandBuffers(ihcDevice.GetDevice(), ihcDevice.GetCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    commandBuffers.clear();
}
#pragma endregion

#pragma region Draw Frame, Record command buffers (check Steps for execution order)
VkCommandBuffer IHCEngine::Graphics::Renderer::BeginFrame() 
{
    assert(!isFrameInProgress && "Can't call beginFrame while already in progress");

    auto result = ihcSwapChain->AcquireNextImage(&currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return nullptr;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    isFrameInProgress = true;

    //Step 1: Start recording
    auto commandBuffer = GetCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    return commandBuffer;
}
void IHCEngine::Graphics::Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(isFrameInProgress && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

    // Step 2: Start Render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = ihcSwapChain->GetRenderPass();
    renderPassInfo.framebuffer = ihcSwapChain->GetFrameBuffer(currentImageIndex);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = ihcSwapChain->GetSwapChainExtent();

    // clear screen values
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { { clearColorValues.x, clearColorValues.y, clearColorValues.z, 1.0f} };
    //clearValues[0].depthStencil = X //depth attachment is 1 not 0
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Optional: dynamic viewport and scissor state
    // for this pipeline to be dynamic.
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)ihcSwapChain->GetWidth();
    viewport.height = (float)ihcSwapChain->GetHeight();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = ihcSwapChain->GetSwapChainExtent();
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // The following are placed at the rendersystem
    // Step 3: Bind model, PushConstants, UniformBufferObjects
    // Step 4: Draw
}
void IHCEngine::Graphics::Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer) 
{
    // Step 5: End Render pass
    assert(isFrameInProgress && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
    vkCmdEndRenderPass(commandBuffer);
}
void IHCEngine::Graphics::Renderer::EndFrame() 
{
    assert(isFrameInProgress && "Can't call endFrame while frame is not in progress");
    
    //Step 6: End recording
    auto commandBuffer = GetCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
    //Step 7: Submit Commands
    auto result = ihcSwapChain->SubmitCommandBuffers(&commandBuffer, &currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || appWindow.IsWindowResized())
    {
        appWindow.SetWindowResized(false);
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    isFrameInProgress = false;
    currentFrameIndex = (currentFrameIndex + 1) % IHCSwapChain::MAX_FRAMES_IN_FLIGHT;
}
#pragma endregion

