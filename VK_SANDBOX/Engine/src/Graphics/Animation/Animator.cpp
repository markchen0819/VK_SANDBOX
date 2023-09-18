#include "../../pch.h"
#include "Animator.h"

#include "../../Core/Time/Time.h"
#include "Animation.h"
#include "Bone.h"

namespace IHCEngine::Graphics
{
	Animator::Animator()
		: currentAnimation(nullptr),
		  currentTime(0.0f)
	{
		finalBoneMatrices.reserve(100);
		for (int i = 0; i < 100; i++)
		{
			finalBoneMatrices.push_back(glm::mat4(1.0f));
		}
	}

	void Animator::UpdateAnimation()
	{
		if(currentAnimation==nullptr)
		{
			assert("No animation assigned to animator");
		}

		float dt = IHCEngine::Core::Time::GetInstance().GetDeltaTime();
		currentTime += currentAnimation->GetTicksPerSecond() * dt;
		currentTime = fmod(currentTime, currentAnimation->GetDuration());
		calculateBoneTransform(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
	}

	void Animator::PlayAnimation(Animation* animation)
	{
		currentAnimation = animation;
		currentTime = 0.0f;
	}

	void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		// Starting from root node of current animation
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation; // ex: arm bone bent 45 degrees

		// Check if there is a bone in the root node related to the animation
		Bone* Bone = currentAnimation->FindBone(nodeName);
		if (Bone)
		{
			// interpolates bone transformation
			// and return local bone transform matrix 
			Bone->Update(currentTime);
			nodeTransform = Bone->GetLocalTransform(); // ex: arm bone change to 65 degrees
		}
		// Convert bone from local space into global space
		glm::mat4 globalTransformation = parentTransform * nodeTransform; // ex: arm bone in the world (consider shoulder)

		// find offset matrix in boneInfoMap (how each bone relates to original T-pose)
		// transforms vertices from the original position of the mesh vertices
		// to match how the bone has moved
		auto boneInfoMap = currentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offsetMatrix = boneInfoMap[nodeName].offsetMatrix;
			finalBoneMatrices[index] = globalTransformation * offsetMatrix;
		}

		for (int i = 0; i < node->childrenCount; i++)
		{
			calculateBoneTransform(&node->children[i], globalTransformation);
		}
	}

	std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
	{
		return finalBoneMatrices;
	}
}
