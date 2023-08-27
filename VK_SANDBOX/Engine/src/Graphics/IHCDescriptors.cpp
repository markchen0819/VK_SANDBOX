#include "../pch.h"
#include "IHCDescriptors.h"


#pragma region IHCDescriptorSetLayout
IHCEngine::Graphics::IHCDescriptorSetLayout::Builder& IHCEngine::Graphics::IHCDescriptorSetLayout::Builder::AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count)
{
	assert(bindings.count(binding) == 0 && "Binding already in use");

	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = binding;  
	layoutBinding.descriptorType = descriptorType;
	layoutBinding.descriptorCount = count;
	layoutBinding.stageFlags = stageFlags;
	bindings[binding] = layoutBinding;

	return *this;
}
std::unique_ptr<IHCEngine::Graphics::IHCDescriptorSetLayout> 
IHCEngine::Graphics::IHCDescriptorSetLayout::Builder::Build() const
{
	return std::make_unique<IHCDescriptorSetLayout>(ihcDevice, bindings);
}
IHCEngine::Graphics::IHCDescriptorSetLayout::IHCDescriptorSetLayout(IHCDevice& ihcDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
	: ihcDevice{ ihcDevice }, bindings{ bindings }
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

	if (vkCreateDescriptorSetLayout (ihcDevice.GetDevice(), &descriptorSetLayoutInfo,nullptr,&descriptorSetLayout)
		!= VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}
IHCEngine::Graphics::IHCDescriptorSetLayout::~IHCDescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(ihcDevice.GetDevice(), descriptorSetLayout, nullptr);
}
#pragma endregion


#pragma region IHCDescriptorPool
IHCEngine::Graphics::IHCDescriptorPool::Builder& IHCEngine::Graphics::IHCDescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
{
	poolSizes.push_back({ descriptorType, count });
	return *this;
}
IHCEngine::Graphics::IHCDescriptorPool::Builder& IHCEngine::Graphics::IHCDescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
{
	poolFlags = flags;
	return *this;
}
IHCEngine::Graphics::IHCDescriptorPool::Builder& IHCEngine::Graphics::IHCDescriptorPool::Builder::SetMaxSets(uint32_t count)
{
	maxSets = count;
	return *this;
}
std::unique_ptr<IHCEngine::Graphics::IHCDescriptorPool>
IHCEngine::Graphics::IHCDescriptorPool::Builder::Build() const
{
	return std::make_unique<IHCDescriptorPool>(ihcDevice, maxSets, poolFlags, poolSizes);
}
IHCEngine::Graphics::IHCDescriptorPool::IHCDescriptorPool(IHCDevice& ihcDevice, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
	: ihcDevice{ ihcDevice } 
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
IHCEngine::Graphics::IHCDescriptorPool::~IHCDescriptorPool()
{
	vkDestroyDescriptorPool(ihcDevice.GetDevice(), descriptorPool, nullptr);
}
bool IHCEngine::Graphics::IHCDescriptorPool::AllocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.pSetLayouts = &descriptorSetLayout; // how many descriptors and what type
	allocInfo.descriptorSetCount = 1;

	// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
	// a new pool whenever an old pool fills up in the future
	// https://vkguide.dev/docs/extra-chapter/abstracting_descriptors/
	if (vkAllocateDescriptorSets(ihcDevice.GetDevice(), &allocInfo, &descriptor) != VK_SUCCESS) 
	{
		return false;
	}
	return true;
}
void IHCEngine::Graphics::IHCDescriptorPool::FreeDescriptorSet(std::vector<VkDescriptorSet>& descriptors) const
{
	vkFreeDescriptorSets
	(
		ihcDevice.GetDevice(),
		descriptorPool,
		static_cast<uint32_t>(descriptors.size()),
		descriptors.data()
	);
}
void IHCEngine::Graphics::IHCDescriptorPool::ResetPool()
{
	vkResetDescriptorPool(ihcDevice.GetDevice(), descriptorPool, 0);
}
#pragma endregion


#pragma region IHCDescriptorWriter
IHCEngine::Graphics::IHCDescriptorWriter::IHCDescriptorWriter(IHCDescriptorSetLayout& setLayout, IHCDescriptorPool& pool)
	: setLayout{ setLayout }, pool{ pool } 
{

} 
IHCEngine::Graphics::IHCDescriptorWriter& IHCEngine::Graphics::IHCDescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
{
	assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = setLayout.bindings[binding];

	assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = binding;
	write.pBufferInfo = bufferInfo;
	write.descriptorCount = 1;

	writes.push_back(write);

	return *this;
}
IHCEngine::Graphics::IHCDescriptorWriter& IHCEngine::Graphics::IHCDescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
{
	assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = setLayout.bindings[binding];

	assert(bindingDescription.descriptorCount == 1 &&"Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = binding;
	write.pImageInfo = imageInfo; // mostly same, just here 
	write.descriptorCount = 1;

	writes.push_back(write);

	return *this;
}
bool IHCEngine::Graphics::IHCDescriptorWriter::Build(VkDescriptorSet& set)
{
	bool success = pool.AllocateDescriptorSet(setLayout.GetDescriptorSetLayout(), set);
	if (!success) 
	{
		std::cout << "pool.AllocateDescriptorSet not successful" << std::endl;
		return false;
	}
	Overwrite(set);
	return true;
}
void IHCEngine::Graphics::IHCDescriptorWriter::Overwrite(VkDescriptorSet& set)
{
	for (auto& write : writes) 
	{
		write.dstSet = set;
	}
	vkUpdateDescriptorSets(pool.ihcDevice.GetDevice(), writes.size(), writes.data(), 0, nullptr);
}
#pragma endregion