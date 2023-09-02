#pragma once

// Forward declaration
namespace IHCEngine::Window
{
	class AppWindow;
}
namespace IHCEngine::Graphics
{
	class IHCDevice;
}

namespace IHCEngine::Graphics
{
	class IHCSwapChain
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		IHCSwapChain(IHCDevice& device, IHCEngine::Window::AppWindow& window);
		IHCSwapChain(IHCDevice& device, IHCEngine::Window::AppWindow& window, std::shared_ptr<IHCSwapChain> previous);
		~IHCSwapChain();
		// no duplication
		IHCSwapChain(const IHCSwapChain&) = delete;
		IHCSwapChain &operator=(const IHCSwapChain&) = delete;

		VkFramebuffer GetFrameBuffer(int index) { return swapChainFramebuffers[index]; }
		VkRenderPass GetRenderPass() { return renderPass; }
		VkImageView GetImageView(int index) { return swapChainImageViews[index]; }
		size_t GetImageCount() { return swapChainImages.size(); }
		VkFormat GetSwapChainImageFormat() { return swapChainImageFormat; }
		VkExtent2D GetSwapChainExtent() { return swapChainExtent; }
		uint32_t GetWidth() { return swapChainExtent.width; }
		uint32_t GetHeight() { return swapChainExtent.height; }
		float GetExtentAspectRatio() { return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);}

		//  DrawFrame (Submission of Command Buffers to Queue)
		VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);
		VkResult AcquireNextImage(uint32_t* imageIndex);

		// Recreation checking
		bool CompareSwapFormats(const IHCSwapChain& swapChain) const 
		{
			return swapChain.swapChainDepthFormat == swapChainDepthFormat 
				&& swapChain.swapChainImageFormat == swapChainImageFormat;
		}

	private:
		//// functions
		void init();
		void cleanupSwapChain();
		void createSwapChain();
		void createImageViews();
		void createRenderPass();
		void createColorResources();
		void createDepthResources();
		void createFramebuffers();
		void createSyncObjects();

		//// Helper functions
	    // swapchain
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
       
		
		// image & imageview (separated for swapchain & color & depth & texture)
		// createImage allocates the memory for color & depth & texture
		// swapchain doesn't need that as we have vkCreateSwapchainKHR
		// it needs to be directly compatible with the display hardware
		//void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        //VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		
		
		// depth
		VkFormat findDepthFormat();

		//// variables
		IHCEngine::Window::AppWindow& window;
		IHCEngine::Graphics::IHCDevice& device;
		// swapChain
		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		std::shared_ptr<IHCSwapChain> oldSwapChain;
		VkFormat swapChainImageFormat;
		VkFormat swapChainDepthFormat;
		VkExtent2D swapChainExtent;
		// ImageView
		std::vector<VkImageView> swapChainImageViews;
		// Drawing
		std::vector<VkFramebuffer> swapChainFramebuffers;
		// msaa (color)
		std::vector<VkImage>  colorImages;
		std::vector<VkDeviceMemory> colorImageMemorys;
		std::vector<VkImageView> colorImageViews;
		// depth test
		std::vector<VkImage> depthImages;
		std::vector<VkDeviceMemory> depthImageMemorys;
		std::vector<VkImageView> depthImageViews;
		// Render pass
		VkRenderPass renderPass;
		// GPU draw, CPU wait frame (var per frame)
		// Semaphores: order between queue operations (GPU - GPU)
		// Fences: ordering the execution on the CPU  (CPU - GPU)
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		size_t currentFrame = 0;
	};
}