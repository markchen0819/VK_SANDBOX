#pragma once
#include <vector>
#include <map>
#include <optional>
#include <vulkan/vulkan_core.h>

// Forward declaration
namespace IHCEngine::Core
{
    class Camera;
    class GameObject;  
}


#pragma region Debugging 
const std::vector<const char*> validationLayers =
{
    "VK_LAYER_KHRONOS_validation",
    //"VK_LAYER_LUNARG_api_dump"
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
    std::optional<uint32_t> graphicsFamily; // rendering // + compute
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
static constexpr int MAX_BONE_WEIGHTS = 4;
struct Vertex
{
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};
    glm::vec3 tangent{};
    glm::vec3 bitangent{};

    int boneIDs[MAX_BONE_WEIGHTS];     //bone indexes which will influence this vertex
    float boneWeights[MAX_BONE_WEIGHTS];   //weights from each bone

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
        attributeDescriptions.push_back({ 4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent) });
        attributeDescriptions.push_back({ 5, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, bitangent) });
        attributeDescriptions.push_back({ 6, 0, VK_FORMAT_R32G32B32A32_SINT, offsetof(Vertex, boneIDs) }); 
        attributeDescriptions.push_back({ 7, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, boneWeights) }); 

    	return attributeDescriptions;
    }
    bool operator==(const Vertex& other) const 
    {
        return position == other.position 
            && color == other.color
            && normal == other.normal 
            && uv == other.uv
    	    && tangent == other.tangent
            && bitangent == other.bitangent;
    }
};
namespace IHCEngine::Graphics
{
	// from: https://stackoverflow.com/a/57595105
    template <typename T>
    void hashCombine(std::size_t& seed, const T& v) {
        seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template <typename T, std::size_t N>
    void hashArrayCombine(std::size_t& seed, const T(&arr)[N]) {
        for (std::size_t i = 0; i < N; ++i) {
            hashCombine(seed, arr[i]);
        }
    }
}
namespace std 
{
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            size_t seed = 0;
            IHCEngine::Graphics::hashCombine(seed, vertex.position);
            IHCEngine::Graphics::hashCombine(seed, vertex.color);
            IHCEngine::Graphics::hashCombine(seed, vertex.normal);
            IHCEngine::Graphics::hashCombine(seed, vertex.uv);
            IHCEngine::Graphics::hashCombine(seed, vertex.tangent);
            IHCEngine::Graphics::hashCombine(seed, vertex.bitangent);
            IHCEngine::Graphics::hashArrayCombine(seed, vertex.boneIDs);
            IHCEngine::Graphics::hashArrayCombine(seed, vertex.boneWeights);
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
    alignas(16) glm::mat4 normalMatrix{1.f};
};
// larger amount of data that doesn't change often
// common use: Camera Matrices, Global Lighting, Per-Object Data, Array of bone transformations

struct GlobalUniformBufferObject
{
    alignas(16) glm::mat4 viewMatrix;
    alignas(16) glm::mat4 projectionMatrix;
    alignas(16) glm::mat4 inverseViewMatrix{1.f};
};

struct SkeletalUniformBufferObject
{
    alignas(16) glm::mat4 finalBonesMatrices[100];
    uint32_t hasAnimation;
    //alignas(4) bool hasAnimation;
};

#pragma endregion


#pragma region FrameInfo (Data passing for VKwraps)
namespace IHCEngine::Graphics
{
    class IHCDescriptorManager;

    struct FrameInfo
    {
        int frameIndex;
        VkCommandBuffer commandBuffer;
    };
}
#pragma endregion 