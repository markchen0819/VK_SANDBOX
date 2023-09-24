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
	return animator.GetDebugBoneVertices();
}

void IHCEngine::Component::AnimatorComponent::UpdateDebugBoneBuffer(Graphics::FrameInfo& frameInfo)
{
	animator.UpdateDebugBoneBuffer(frameInfo);
}

IHCEngine::Graphics::IHCBuffer* IHCEngine::Component::AnimatorComponent::GetDebugBoneBuffer(Graphics::FrameInfo& frameInfo)
{
	return animator.GetDebugBoneBuffer(frameInfo);
}

void IHCEngine::Component::AnimatorComponent::Remove()
{

}
