#include "../../../pch.h"
#include "IHCDescriptorWrap.h"
#include "../IHCDevice.h"

namespace IHCEngine::Graphics
{
	IHCDescriptorWrap::IHCDescriptorWrap(IHCDevice& ihcDevice)
		: ihcDevice(ihcDevice)
	{

	}

	IHCDescriptorWrap::~IHCDescriptorWrap()
	{
		vkDeviceWaitIdle(ihcDevice.GetDevice());
		vkDestroyDescriptorSetLayout(ihcDevice.GetDevice(), descriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(ihcDevice.GetDevice(), descriptorPool, nullptr);
	}

#pragma region DescriptorPool
	void IHCDescriptorWrap::AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
	{
		poolSizes.push_back({ descriptorType, count });
	}
	void IHCDescriptorWrap::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
	{
		poolFlags = flags;
	}
	void IHCDescriptorWrap::SetMaxSets(uint32_t count)
	{
		maxSets = count;
	}
	void IHCDescriptorWrap::InitializePool()
	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;

		if (vkCreateDescriptorPool(ihcDevice.GetDevice(), &descriptorPoolInfo, nullptr, &descriptorPool)
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
	void IHCDescriptorWrap::ResetPool()
	{
		vkResetDescriptorPool(ihcDevice.GetDevice(), descriptorPool, 0);
	}
#pragma endregion

#pragma region DescriptorSetLayout
	void IHCDescriptorWrap::AddLayoutBinding(uint32_t binding, VkDescriptorType descriptorType,
		VkShaderStageFlags stageFlags, uint32_t count)
	{
		assert(bindings.count(binding) == 0 && "Binding already in use");

		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		bindings[binding] = layoutBinding;
	}
	void IHCDescriptorWrap::CreateLayout()
	{
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		for (auto b : bindings)
		{
			setLayoutBindings.push_back(b.second);
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

		if (vkCreateDescriptorSetLayout(ihcDevice.GetDevice(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout)
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}
#pragma endregion

#pragma region DescriptorSet (Alloc/ Release) (Binding resources to shader/ Overwrites)
	VkDescriptorSet IHCDescriptorWrap::AllocateDescriptorSet() 
	{
		VkDescriptorSet descriptorSet;
		if (!availableDescriptorSets.empty())
		{
			descriptorSet = availableDescriptorSets.top();
			availableDescriptorSets.pop();
		}
		else
		{
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = descriptorPool;
			allocInfo.pSetLayouts = &descriptorSetLayout; // how many descriptors and what type
			allocInfo.descriptorSetCount = 1;
			if (vkAllocateDescriptorSets(ihcDevice.GetDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS)
			{
				assert("Descriptor sets allocation failed, Check if exceed poolsize");
				// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
			    // a new pool whenever an old pool fills up in the future
			    // https://vkguide.dev/docs/extra-chapter/abstracting_descriptors/
			}
		}
		descriptorSets.push_back(descriptorSet);
		return descriptorSet;
	}
	void IHCDescriptorWrap::FreeDescriptorSet(VkDescriptorSet descriptorSet) 
	{
		vkDeviceWaitIdle(ihcDevice.GetDevice());

		// ACTUALLY REUSING INSTEAD OF FREEING
		auto it = std::remove(descriptorSets.begin(), descriptorSets.end(), descriptorSet);
		descriptorSets.erase(it, descriptorSets.end());
		availableDescriptorSets.push(descriptorSet);
	}

	void IHCDescriptorWrap::BindBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
	{
		assert(bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = bindings[binding];

		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;

		writes.push_back(write);

	}
	void IHCDescriptorWrap::BindImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
	{
		assert(bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = bindings[binding];

		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo; // mostly same, just here 
		write.descriptorCount = 1;

		writes.push_back(write);
	}
	void IHCDescriptorWrap::OverwriteBindings(VkDescriptorSet& set)
	{
		for (auto& write : writes)
		{
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(ihcDevice.GetDevice(), writes.size(), writes.data(), 0, nullptr);
		writes.clear();
	}
#pragma endregion

	VkDescriptorPool IHCDescriptorWrap::GetPool() const
	{
		return descriptorPool;
	}

	VkDescriptorSetLayout IHCDescriptorWrap::GetDescriptorSetLayout() const
	{
		return descriptorSetLayout;
	}
}

