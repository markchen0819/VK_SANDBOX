#pragma once

// Forward declaration
namespace IHCEngine::Window
{
    class AppWindow;
}
struct QueueFamilyIndices; 
struct SwapChainSupportDetails;

namespace IHCEngine::Graphics
{
	class IHCDevice
	{
    public:
        IHCDevice(IHCEngine::Window::AppWindow& window);
        ~IHCDevice();
        // no duplication
        IHCDevice(const IHCDevice&) = delete;
        IHCDevice &operator=(const IHCDevice&) = delete;

        VkInstance GetInstance() { return instance; } // imgui
        VkSurfaceKHR GetSurface() { return surface; }
        VkDevice GetDevice() { return device; }
        VkQueue GetGraphicsQueue() { return graphicsQueue; }
        VkQueue GetPresentQueue() { return presentQueue; };
        QueueFamilyIndices FindPhysicalQueueFamilies(); 
        uint32_t GetGraphicsQueueIndex(); // imgui
        SwapChainSupportDetails GetSwapChainSupport();
        VkCommandPool GetCommandPool() { return commandPool; }
        VkSampleCountFlagBits GetMsaaSamples() { return msaaSamples; }

        VkPhysicalDevice GetPhysicalDevice() { return physicalDevice; }
        VkPhysicalDeviceProperties GetPhysicalDeviceProperties() { return properties; }
        
        // Image, Attachment Helper Functions
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        void CreateImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);


        // Buffer Helper Functions
        void CreateBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory);
        // Mostly use single times for
        // Buffer Copying, Image Layout Transitions, Buffer Initialization
        // it can Reduce Overhead, Simplify Synchronization
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

    private:

        //// functions
        void createInstance(); // connection to the Vulkan library
        void setupDebugMessenger();  // debug messenger for Vulkan.
        void createSurface(); // surface render onto (e.g., a window in an operating system).
        void pickPhysicalDevice(); // Selects a physical device (GPU) to use. depend on what types of features we need, like certain types of textures or shaders.
        void createLogicalDevice(); // Creates a logical device. communicate with the physical device.
        void createCommandPool(); // Creates a command pool. Allocate command buffers, record rendering commands send to the GPU.


        //// Helper functions
        // physical device
        int evaluateDeviceScore(VkPhysicalDevice device);
        // debug & validation
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void hasGflwRequiredInstanceExtensions();
        // device support (rendering/ presenting/ swapchain/ msaa)
        VkSampleCountFlagBits getMaxUsableSampleCount();
        bool isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        //// variables
        IHCEngine::Window::AppWindow& window;
        // window
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        // surface
        VkSurfaceKHR surface;
        // physical
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties properties;
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        // logical
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue computeQueue;
        VkQueue presentQueue;
        // Commandpool
        VkCommandPool commandPool;
	};

}

