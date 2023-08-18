#pragma once
#include "../pch.h"
#include "VKHelpers.h"
#include "IHCDevice.h"

namespace IHCEngine::Graphics
{
	class IHCSwapChain
	{

	public:
		const int MAX_FRAMES_IN_FLIGHT = 2;


		IHCSwapChain(IHCDevice& device, IHCEngine::Window::AppWindow& window);
		IHCSwapChain(IHCDevice& device, IHCEngine::Window::AppWindow& window, std::shared_ptr<IHCSwapChain> previous);
		~IHCSwapChain();

		// no duplication
		IHCSwapChain(const IHCSwapChain&) = delete;
		void operator=(const IHCSwapChain&) = delete;

		VkFramebuffer GetFrameBuffer(int index) { return swapChainFramebuffers[index]; }
		VkRenderPass GetRenderPass() { return renderPass; }
		VkImageView GetImageView(int index) { return swapChainImageViews[index]; }
		size_t GetImageCount() { return swapChainImages.size(); }
		VkFormat GetSwapChainImageFormat() { return swapChainImageFormat; }
		VkExtent2D GetSwapChainExtent() { return swapChainExtent; }
		uint32_t GetWidth() { return swapChainExtent.width; }
		uint32_t GetHeight() { return swapChainExtent.height; }

		float GetExtentAspectRatio() {
			return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
		}
		VkFormat FindDepthFormat();
		VkResult AcquireNextImage(uint32_t* imageIndex);
		VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

		bool CompareSwapFormats(const IHCSwapChain& swapChain) const {
			return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
				swapChain.swapChainImageFormat == swapChainImageFormat;
		}

	private:
		

		void init();
		void createSwapChain();
		void createImageViews();
		void createColorResources();
		void createDepthResources();
		void createRenderPass();
		void createFramebuffers();
		void createSyncObjects();

		// window size change requires to recreate swap chain
		void recreateSwapChain();
		void cleanupSwapChain();

		//// Helper functions
	    // debug & validation
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
       



		//// variables
		IHCEngine::Window::AppWindow& window;
		IHCEngine::Graphics::IHCDevice& device;
		// swapChain
		VkSwapchainKHR swapChain;
		std::shared_ptr<IHCSwapChain> oldSwapChain;
		VkFormat swapChainImageFormat;
		VkFormat swapChainDepthFormat;
		VkExtent2D swapChainExtent;
		// Drawing
		std::vector<VkFramebuffer> swapChainFramebuffers;
		// depth test
		std::vector<VkImage> depthImages;
		std::vector<VkDeviceMemory> depthImageMemorys;
		std::vector<VkImageView> depthImageViews;


	
		// Render pass
		VkRenderPass renderPass;
		// ImageView
		std::vector<VkImageView> swapChainImageViews;
		



		std::vector<VkImage> swapChainImages;
	
		// GPU draw, CPU wait frame (var per frame)
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		size_t currentFrame = 0;

	};

}