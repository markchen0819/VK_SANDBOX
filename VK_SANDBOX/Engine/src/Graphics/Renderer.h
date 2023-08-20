#pragma once
#include "../pch.h"
#include "IHCSwapChain.h"
#include "IHCModel.h"

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


        VkCommandBuffer GetCurrentCommandBuffer() const 
        {
            assert(isFrameInProgress && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int GetFrameIndex() const 
        {
            assert(isFrameInProgress && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }


        // Draw Frame functions
        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);


    private:

        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        Window::AppWindow& appWindow;
        IHCDevice& ihcDevice;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<IHCSwapChain> ihcSwapChain;
        uint32_t currentImageIndex;
        int currentFrameIndex{ 0 };
        bool isFrameInProgress{ false };
	};
}

