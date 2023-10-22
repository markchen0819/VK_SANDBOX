#include "../../../pch.h"
#include "AnimatorComponent.h"
#include "../../Locator/GraphicsManagerLocator.h"
#include "../../Engine/src/Graphics/VKWraps/IHCBuffer.h"
#include "../../Engine/src/Graphics/VKWraps/IHCDevice.h"

IHCEngine::Component::AnimatorComponent::AnimatorComponent()
	:Component(ComponentType::Animator)
{

}

void IHCEngine::Component::AnimatorComponent::SetAnimationType(Graphics::AnimationType type)
{
	animator.SetAnimationType(type);
}

void IHCEngine::Component::AnimatorComponent::SetAnimation(Graphics::Animation* animation)
{
	animator.SetAnimation(animation);
}

void IHCEngine::Component::AnimatorComponent::SetBlendTree(Graphics::BlendTree* blendtree)
{
	animator.SetBlendTree(blendtree);
}

void IHCEngine::Component::AnimatorComponent::PlayAnimation()
{
	animator.PlayAnimation();
}

void IHCEngine::Component::AnimatorComponent::StopAnimation()
{
	animator.StopAnimation();
}

bool IHCEngine::Component::AnimatorComponent::HasAnimation()
{
	if(animator.GetCurrentAnimation()!=nullptr || animator.GetCurrentBlendTree() != nullptr)
	{
		return true;
	}
	return false;
}

void IHCEngine::Component::AnimatorComponent::SetSpeed(float speed)
{
	animator.SetSpeed(speed);
}

void IHCEngine::Component::AnimatorComponent::UpdateAnimation(float dt)
{
	animator.UpdateAnimation(dt);
}

std::vector<glm::mat4>& IHCEngine::Component::AnimatorComponent::GetFinalBoneMatrices()
{
	return animator.GetFinalBoneMatrices();
}

float IHCEngine::Component::AnimatorComponent::GetSpeed()
{
	return animator.GetSpeed();
}

std::vector<VkDescriptorSet>& IHCEngine::Component::AnimatorComponent::GetDescriptorSets()
{
	return animator.GetDescriptorSets();
}

std::vector<IHCEngine::Graphics::IHCBuffer*>& IHCEngine::Component::AnimatorComponent::GetBuffers()
{
	return animator.GetBuffers();
}

void IHCEngine::Component::AnimatorComponent::AllocateDebugBoneBuffer()
{
	animator.AllocateDebugBoneBuffer();
}


void IHCEngine::Component::AnimatorComponent::UpdateDebugBoneBuffer(Graphics::FrameInfo& frameInfo)
{
	animator.UpdateDebugBoneBuffer(frameInfo);
}

void IHCEngine::Component::AnimatorComponent::DrawDebugBoneBuffer(Graphics::FrameInfo& frameInfo)
{
	animator.DrawDebugBoneBuffer(frameInfo);
}

void IHCEngine::Component::AnimatorComponent::Remove()
{

}
