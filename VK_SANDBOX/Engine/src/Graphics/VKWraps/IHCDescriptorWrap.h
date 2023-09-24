#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace IHCEngine::Graphics
{
    class IHCDescriptorWrap
    {
    public:

        ~IHCDescriptorWrap()
    	{
            vkDestroyDescriptorSetLayout(device, descriptorLayout, nullptr);
            vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        }

        // Pool
        void AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
        SetPoolFlags(VkDescriptorPoolCreateFlags flags);
        SetMaxSets(uint32_t count);
    	bool InitializePool();
        void ResetPool();

        // Layout
        void AddLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count);
    	bool CreateLayout();

    	// Sets
        VkDescriptorSet AllocateDescriptorSet() const;
        void FreeDescriptorSet(VkDescriptorSet descriptorSet) const;

        // Binds the resource to descriptor
        void BindBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        void BindImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);
		void Overwrite(VkDescriptorSet& set);


        VkDescriptorPool GetPool() const = 0;
        VkDescriptorSetLayout GetLayout() const = 0;
        const std::vector<VkDescriptorSet>& GetSets() const = 0;

    private:

        VkDevice device = VK_NULL_HANDLE;
        // Pool
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorPoolSize> poolSizes{};
        uint32_t maxSets = 1000;
        VkDescriptorPoolCreateFlags poolFlags = 0;

        // Layout
        VkDescriptorSetLayout descriptorLayout = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};

        // Sets
        std::vector<VkDescriptorSet> descriptorSets;
        std::stack<VkDescriptorSet> availableDescriptorSets;
        // WriteBuffer & WriteImage are stored here
        // Overwrite to update and clear writes
        std::vector<VkWriteDescriptorSet> writes;
    
        // Expected varaibles in derived class
        // Assume resources stored in respect classes
        // uses these as reference to do WriteBuffer and WriteImage
        // std::vector<IHCBuffer*> buffers; // contains buffer info
        // std::vector<IHCTexture*> textures;  // contains image view and sampler
    };
}