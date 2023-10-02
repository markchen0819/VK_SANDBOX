#pragma once
#include "VKWraps/IHCSwapChain.h"

// Forward declaration
namespace IHCEngine::Graphics
{
    class IHCDevice;
}

namespace IHCEngine::Graphics
{
	class Renderer
	{
    public:

        Renderer(Window::AppWindow& window, IHCDevice& device);
        ~Renderer();
        // no duplication
        Renderer(const Renderer&) = delete;
        Renderer&operator=(const Renderer&) = delete;

        // Provide to pipeline
        VkRenderPass GetSwapChainRenderPass() const { return ihcSwapChain->GetRenderPass(); }
        float GetAspectRatio() const { return ihcSwapChain->GetExtentAspectRatio(); }
        bool IsFrameInProgress() const { return isFrameInProgress; }

        // Provide to Renderer
        // Get command buffer and swapchainImage for draw
        int GetFrameIndex() const 
        {
            assert(isFrameInProgress && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }
        VkCommandBuffer GetCurrentCommandBuffer() const
        {
            assert(isFrameInProgress && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        // Record Commands functions (ex: Draw Frame )
        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

        //Imgui
        uint32_t GetSwapChainImageCount() { return ihcSwapChain->GetImageCount(); }

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        Window::AppWindow& appWindow;
        IHCDevice& ihcDevice;

        // Command buffers 
        std::vector<VkCommandBuffer> commandBuffers;
        int currentFrameIndex{ 0 }; // cycle through MAX_FRAMES_IN_FLIGHT for commandbuffer resource
        bool isFrameInProgress{ false };

        // SwapChainImages
        std::unique_ptr<IHCSwapChain> ihcSwapChain;
        uint32_t currentImageIndex; // image from swapchain that is available to render to
       
        // command buffers and swapchainImages are no longer 1 to 1 relation
        // render pass compatiblity
        // https://registry.khronos.org/vulkan/specs/1.1-extensions/html/chap8.html#renderpass-compatibility

        glm::vec3 clearColorValues{0.0, 0.7, 1.0};

        // Count breakdown
        // 
        // Assume we have ...
        // MAX_FRAMES_IN_FLIGHT: 2
        // Swap Chain Image Count: swapChainSupport.capabilities.minImageCount + 1
        // Command Buffers Count: resize(MAX_FRAMES_IN_FLIGHT)
        // Sync Objects Count: resize(MAX_FRAMES_IN_FLIGHT)
        // 
        // Double Buffering
        // Swap Chain Images: 2 (1 display, 1 render)
        // Command Buffers: 2 (cycle between frames, 1 GPU execute, 1 CPU command prep)
        // Sync Objects: 2 (both ensure no conflict between frames)
        //
        // Triple Buffering
        // Swap Chain Images: 3 (1 display, 1 render, 1 prep for display or render)
        // Command Buffers: 2 (cycle between frames, 1 GPU execute, 1 CPU command prep)
        //                   even though there are 3 images
        // Sync Objects: 2 (both ensure no conflict between frames)
        //
        // currentFrameIndex: cycle between 0 and 1
        // determining which set of command buffers and sync objects 
        // you're working with for a given frame.
        //
        // currentImageIndex: Determined by AcquireNextImage.
        // which swap chain image you should render to next.
        // range between 0 to(Swap Chain Image Count - 1).
        //
        // Both limited to processing two frames (MAX_FRAMES_IN_FLIGHT)
        // triple buffering scenario potentially reducing display lag and screen tearing
        //
        // 1 to 1 : intuitive and common
        // Not 1 to 1 : Memory Efficiency, Simpler CPU Scheduling,
        // Flexibility with Rendering Strategies(deferred rendering), more predictable performance.
    };
}

