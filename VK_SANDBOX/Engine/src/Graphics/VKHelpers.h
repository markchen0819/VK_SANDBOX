#pragma once
#include "Camera.h"
#include "../Core/GameObject.h"

#pragma region Debugging 
const std::vector<const char*> validationLayers =
{
    "VK_LAYER_KHRONOS_validation"
};
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
#pragma endregion


#pragma region SwapChainSupport
const std::vector<const char*> deviceExtensions = 
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
#pragma endregion


#pragma region QueueFamilies
// QueueFamilies, find queue id of the logical device
// extend other purpose queues here
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily; // rendering
    std::optional<uint32_t> presentFamily;  // flush to screen

    bool isComplete() // finish finding all ids that we want
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};
#pragma endregion


#pragma region Shader
// Shader
// Single binding (all at binding 0) with 3 attributes (different location) for now
// typically we want minimize memory allocations 
// -> allocate one large vertex buffer, each binding have different region
// vertex buffer [ (binding 0 )( binding1) ]
// single binding good for most cases, but still depends on implementaion
struct Vertex
{
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
        // return vector incase we want more bindings in the future
    }
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
        attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
        attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
        attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

        return attributeDescriptions;
    }
    bool operator==(const Vertex& other) const 
    {
        return position == other.position 
            && color == other.color
            && normal == other.normal 
            && uv == other.uv;
    }
};
namespace IHCEngine::Graphics
{
    // from: https://stackoverflow.com/a/57595105
    template <typename T, typename... Rest>
    void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
        seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    };
}
namespace std 
{
    template<> struct hash<Vertex> { 
        size_t operator()(Vertex const& vertex) const {
            size_t seed = 0;
            IHCEngine::Graphics::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}
// small amounts of uniform data to shaders without using uniform buffers.
// frequently changing data(+) , size limit(-), require multiple drawcalls(-)
// common use: Transformation Matrices, Material properties, Light properties, Animation data
struct SimplePushConstantData
{
    alignas(16) glm::mat4 modelMatrix{1.f};
    //alignas(16) glm::mat4 normalMatrix{1.f};
};
// larger amount of data that doesn't change often
// common use: Camera Matrices, Global Lighting, Per-Object Data, Array of bone transformations
struct UniformBufferObject
{
   // alignas(16) glm::mat4 modelMatrix;
   //alignas(16) glm::mat4 viewMatrix;
   // alignas(16) glm::mat4 projMatrix;
};

struct GlobalUniformBufferObject
{
    alignas(16) glm::mat4 viewMatrix;
    alignas(16) glm::mat4 projectionMatrix;
    alignas(16) glm::mat4 inverseViewMatrix{1.f};
};

#pragma endregion


#pragma region FrameInfo (Data passing for VKwraps)

namespace IHCEngine::Core
{
    class GameObject;  // Forward declaration, BUT WHY?
}

namespace IHCEngine::Graphics
{
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera& camera;
        VkDescriptorSet globalDescriptorSet;
        std::unordered_map<unsigned int, IHCEngine::Core::GameObject*> gameObjects;
    };
}
#pragma endregion 