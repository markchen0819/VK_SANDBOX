#include "../../pch.h"
#include "Animator.h"

#include "../../Core/Time/Time.h"
#include "../../Core/Locator/GraphicsManagerLocator.h"
#include "Animation.h"
#include "BoneAnimation.h"


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

		auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();
		graphicsAssetCreator.CreateSkeletalData(this);
	}

	Animator::~Animator()
	{
		auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();
		graphicsAssetCreator.DestroySkeletalData(this);
	}

	void Animator::UpdateAnimation(float dt)
	{
		if(currentAnimation==nullptr)
		{
			assert("No animation assigned to animator");
		}

		currentTime += currentAnimation->GetTicksPerSecond() * dt;
		currentTime = fmod(currentTime, currentAnimation->GetDuration());
		debugBoneVertices.clear();
		calculateBoneTransform(&currentAnimation->GetRootNodeOfHierarhcy(), glm::mat4(1.0f));
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
		BoneAnimation* boneAnimation = currentAnimation->FindBone(nodeName);
		if (boneAnimation)
		{
			// interpolates bone transformation
			// and return local bone transform matrix 
			boneAnimation->Update(currentTime);
			nodeTransform = boneAnimation->GetLocalTransform(); // ex: arm bone change to 65 degrees
		}
		// Convert bone from local space into global space
		glm::mat4 globalTransformation = parentTransform * nodeTransform; // ex: arm bone in the world (consider shoulder)

		Vertex debugVertex;
		debugVertex.color = glm::vec3(0.0, 1.0, 0.0);
		debugVertex.position = glm::vec3(globalTransformation[3]);
		debugBoneVertices.push_back(debugVertex);
		// If the bone has a parent, its position
		// would be the end of the parent bone segment
		if (node->parent)
		{
			debugVertex.position = glm::vec3(parentTransform[3]);
			debugBoneVertices.push_back(debugVertex);
		}

		// find offset matrix in boneInfoMap (how each bone relates to original T-pose)
		// transforms vertices from the original position of the mesh vertices
		// to match how the bone has moved
		auto boneInfoMap = currentAnimation->GetBoneInfoMap();
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
