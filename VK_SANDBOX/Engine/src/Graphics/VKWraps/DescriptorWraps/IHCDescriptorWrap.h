#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <stack>
#include <unordered_map>

namespace IHCEngine::Graphics
{
	class IHCDevice;

	class IHCDescriptorWrap
    {
    public:

        IHCDescriptorWrap(IHCDevice& ihcDevice);
        virtual ~IHCDescriptorWrap();

        // Init pool and create layout
        virtual void Setup() = 0;
        // Create resources in derived class or elsewhere
        virtual void CreateResources() = 0;
        // Bind resources to shaders in derived class or elsewhere
		virtual void AllocAndBindDescriptorSets() = 0;

        // Pool
        void AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
        void SetPoolFlags(VkDescriptorPoolCreateFlags flags);
        void SetMaxSets(uint32_t count);
    	void InitializePool();
        void ResetPool();

        // Layout
        void AddLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count=1);
    	void CreateLayout();

    	// Sets
        VkDescriptorSet AllocateDescriptorSet();
        void FreeDescriptorSet(VkDescriptorSet descriptorSet);

        // Binds the resource to descriptor
        void BindBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        void BindImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);
		void OverwriteBindings(VkDescriptorSet& set);


        VkDescriptorPool GetPool() const;
        VkDescriptorSetLayout GetDescriptorSetLayout() const;

    protected:

        IHCDevice& ihcDevice;
        // Pool
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorPoolSize> poolSizes{};
        uint32_t maxSets = 1000;
        VkDescriptorPoolCreateFlags poolFlags = 0;

        // Layout
        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};

		// Sets
        std::vector<VkDescriptorSet> descriptorSets;
        std::stack<VkDescriptorSet> availableDescriptorSets;
        // WriteBuffer & WriteImage are stored here
        // Overwrite to update and clear writes
        std::vector<VkWriteDescriptorSet> writes;
    
        // Expected variables in derived class
        // Assume resources stored in respect classes
        // uses these as reference to do WriteBuffer and WriteImage
        // std::vector<IHCBuffer*> buffers; // contains buffer info
        // std::vector<IHCTexture*> textures;  // contains image view and sampler
    };
}