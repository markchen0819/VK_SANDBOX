#include "../../../pch.h"
#include "AnimatorComponent.h"
#include "../../Locator/GraphicsManagerLocator.h"
#include "../../Engine/src/Graphics/VKWraps/IHCBuffer.h"
#include "../../Engine/src/Graphics/VKWraps/IHCDevice.h"

IHCEngine::Component::AnimatorComponent::AnimatorComponent()
	:Component(ComponentType::Animator)
{

}

void IHCEngine::Component::AnimatorComponent::PlayAnimation(Graphics::Animation* animation)
{
	animator.PlayAnimation(animation);
}

bool IHCEngine::Component::AnimatorComponent::HasAnimation()
{
	if(animator.GetCurrentAnimation()!=nullptr)
	{
		return true;
	}
	return false;
}

void IHCEngine::Component::AnimatorComponent::UpdateAnimation(float dt)
{
	animator.UpdateAnimation(dt);
}

std::vector<glm::mat4> IHCEngine::Component::AnimatorComponent::GetFinalBoneMatrices()
{
	return animator.GetFinalBoneMatrices();
}

std::vector<VkDescriptorSet>& IHCEngine::Component::AnimatorComponent::GetDescriptorSets()
{
	return animator.GetDescriptorSets();
}

std::vector<IHCEngine::Graphics::IHCBuffer*>& IHCEngine::Component::AnimatorComponent::GetBuffers()
{
	return animator.GetBuffers();
}

std::vector<Vertex>& IHCEngine::Component::AnimatorComponent::GetDebugBoneVertices()
{
	// test
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	std::vector<Vertex>& bonevertices = animator.GetDebugBoneVertices();
	auto vertexCount = static_cast<uint32_t>(bonevertices.size());
    // temporary buffer accessed by CPU and GPU
	VkDeviceSize bufferSize = sizeof(bonevertices[0]) * vertexCount;
	uint32_t vertexSize = sizeof(bonevertices[0]);
	Graphics::IHCBuffer stagingBuffer
    {
		*graphicsManager->GetIHCDevice(),
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };
    // without HOST_VISIBLE, HOST_COHERENT we'll need vkflush

    // map temporary buffer memory to CPU address space
    // (able to write/read on CPU )
    // Copy vertex data to staging bufferF
    stagingBuffer.Map();
    stagingBuffer.WriteToBuffer((void*)bonevertices.data());
    stagingBuffer.Unmap(); //also handled automatically in destructor

    // actual vertex buffer, staging buffer to the vertex buffer by the GPU
	debugBoneBuffer = std::make_unique<Graphics::IHCBuffer>(
		*graphicsManager->GetIHCDevice(),
		vertexSize,
		vertexCount,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		| VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	graphicsManager->GetIHCDevice()->CopyBuffer(stagingBuffer.GetBuffer(), debugBoneBuffer->GetBuffer(), bufferSize);

	return animator.GetDebugBoneVertices();
}

IHCEngine::Graphics::IHCBuffer* IHCEngine::Component::AnimatorComponent::GetDebugBoneBuffer()
{
	return debugBoneBuffer.get();
}

void IHCEngine::Component::AnimatorComponent::Remove()
{

}
