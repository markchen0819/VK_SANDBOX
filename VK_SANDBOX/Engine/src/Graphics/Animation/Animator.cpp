#include "../../pch.h"
#include "Animator.h"

#include "../../Core/Time/Time.h"
#include "../../Core/Locator/GraphicsManagerLocator.h"
#include "../../Graphics/VKWraps/IHCSwapChain.h"
#include "../../Graphics/VKWraps/IHCBuffer.h"
#include "../../Graphics/VKWraps/IHCDevice.h"

#include "Animation.h"
#include "AnimationConfig.h"
#include "BlendTree.h"
#include "BoneAnimation.h"
#include "../../Math/VQS.h"

namespace IHCEngine::Graphics
{
	Animator::Animator()
		: currentAnimation(nullptr),
		  currentTime(0.0f),
	      speed(1.0)
	{
		finalBoneMatrices.reserve(100);
		for (int i = 0; i < 100; i++)
		{
			finalBoneMatrices.push_back(glm::mat4(1.0f));
		}

		// vulkan
		auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();
		graphicsAssetCreator.CreateSkeletalData(this);

		// debug
		debugBoneBuffers.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
	}

	Animator::~Animator()
	{
		auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();
		graphicsAssetCreator.DestroySkeletalData(this);
	}

	void Animator::UpdateAnimation(float dt)
	{
		if (!isPlaying) return;

		debugBoneVertices.clear();

		if(animationType == AnimationType::SINGLE_ANIMATION)
		{
			if (currentAnimation == nullptr)
			{
				std::cerr << "No animation assigned to animator" << std::endl;
				assert(false);
			}

			currentTime += currentAnimation->GetTicksPerSecond() * speed * dt;
			currentTime = fmod(currentTime, currentAnimation->GetDuration());
			debugBoneVertices.clear();

			if (AnimationConfig::calculateBonesWithVQS)
			{
				calculateBoneTransformVQS(&currentAnimation->GetRootNodeOfHierarhcy(), Math::VQS());
			}
			else
			{
				calculateBoneTransformMatrix(&currentAnimation->GetRootNodeOfHierarhcy(), glm::mat4(1.0f));
			}

		}
		else if(animationType == AnimationType::BLEND_TREE)
		{
			// Assume animationA and animationB are synced using same time
			auto animationA = blendTree->GetAnimationA();
			currentTime += animationA->GetTicksPerSecond() * speed * dt;
			currentTime = fmod(currentTime, animationA->GetDuration());

			if (AnimationConfig::calculateBonesWithVQS)
			{
				// When we set animation
				// we assume they are processed with same model -> same node hierachy
				Animation* animationA = blendTree->GetAnimationA();
				calculateBoneTransformVQS(blendTree, &animationA->GetRootNodeOfHierarhcy(), Math::VQS());
			}
			else
			{
				//calculateBoneTransformMatrix(&currentAnimation->GetRootNodeOfHierarhcy(), glm::mat4(1.0f));
			}
		}

	}

#pragma region Animation calculations
	std::vector<glm::mat4>& Animator::GetFinalBoneMatrices()
	{
		return finalBoneMatrices;
	}
	void Animator::calculateBoneTransformMatrix(const SkeletalNodeData* node, glm::mat4 parentTransform)
	{
		// Starting from root node of current animation
		const std::string& nodeName = node->name;
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

		// find offset matrix in skinningInfoMap (how each bone relates to original T-pose)
		// transforms vertices from the original position of the mesh vertices
		// to match how the bone has moved
		auto& boneInfoMap = currentAnimation->GetSkinningInfoMap();

		auto iter = boneInfoMap.find(nodeName);
		if (iter != boneInfoMap.end())
		{
			int index = iter->second.id;
			glm::mat4 offsetMatrix = iter->second.offsetMatrix;
			finalBoneMatrices[index] = globalTransformation * offsetMatrix;
		}
		for (int i = 0; i < node->childrenCount; i++)
		{
			calculateBoneTransformMatrix(&node->children[i], globalTransformation);
		}
	}
	void Animator::calculateBoneTransformVQS(const SkeletalNodeData* node, Math::VQS parentVQS)
	{
		// Starting from root node of current animation
		const std::string& nodeName = node->name;
		Math::VQS nodeTransformVQS = Math::VQS::GLMMat4ToVQS(node->transformation); // ex: arm bone bent 45 degrees

		// Check if there is a bone in the root node related to the animation
		BoneAnimation* boneAnimation = currentAnimation->FindBone(nodeName);
		if (boneAnimation)
		{
			// interpolates bone transformation
			// and return local bone transform matrix 
			boneAnimation->Update(currentTime);
			nodeTransformVQS = boneAnimation->GetLocalTransformVQS(); // ex: arm bone change to 65 degrees
		}
		// Convert bone from local space into global space
		Math::VQS globalVQS = parentVQS * nodeTransformVQS; // ex: arm bone in the world (consider shoulder)

		Vertex debugVertex;
		debugVertex.color = glm::vec3(0.0, 1.0, 0.0);
		debugVertex.position = globalVQS.GetTranslate();
		debugBoneVertices.push_back(debugVertex);
		// If the bone has a parent, its position
		// would be the end of the parent bone segment
		if (node->parent)
		{
			debugVertex.position = parentVQS.GetTranslate();
			debugBoneVertices.push_back(debugVertex);
		}

		// find offset matrix in skinningInfoMap (how each bone relates to original T-pose)
		// transforms vertices from the original position of the mesh vertices
		// to match how the bone has moved
		auto& boneInfoMap = currentAnimation->GetSkinningInfoMap();

		auto iter = boneInfoMap.find(nodeName);
		if (iter != boneInfoMap.end())
		{
			int index = iter->second.id;
			glm::mat4 offsetMatrix = iter->second.offsetMatrix;
			finalBoneMatrices[index] = Math::VQS::VQSToGLMMat4(globalVQS) * offsetMatrix;
		}
		for (int i = 0; i < node->childrenCount; i++)
		{
			calculateBoneTransformVQS(&node->children[i], globalVQS);
		}
	}


	void Animator::calculateBoneTransformVQS(BlendTree* blendTree, const SkeletalNodeData* node, Math::VQS parentVQS)
	{
		Animation* animationA = blendTree->GetAnimationA();
		Animation* animationB = blendTree->GetAnimationB();

		// Starting from root node of model
		const std::string& nodeName = node->name;
		Math::VQS nodeTransformVQS = Math::VQS::GLMMat4ToVQS(node->transformation); // ex: arm bone bent 45 degrees

		// Check if there is a bone in the root node related to the animation
		BoneAnimation* boneAnimationA = animationA->FindBone(nodeName);
		BoneAnimation* boneAnimationB = animationB->FindBone(nodeName);
		if (boneAnimationA && boneAnimationB)
		{
			// interpolates bone transformation
			// and return local bone transform matrix 
			boneAnimationA->Update(currentTime);
			boneAnimationB->Update(currentTime);

			auto vqs1 = boneAnimationA->GetLocalTransformVQS();
			auto vqs2 = boneAnimationB->GetLocalTransformVQS();
			float blendFactor = blendTree->GetBlendFactor();
			glm::vec3 vec = glm::vec3(blendFactor, blendFactor, blendFactor);
			auto resultVQS = Math::VQS::Interpolate(vqs1, vqs2, vec);

			nodeTransformVQS = resultVQS;
		}

		// Convert bone from local space into global space
		Math::VQS globalVQS = parentVQS * nodeTransformVQS; // ex: arm bone in the world (consider shoulder)

		Vertex debugVertex;
		debugVertex.color = glm::vec3(0.0, 1.0, 0.0);
		debugVertex.position = globalVQS.GetTranslate();
		debugBoneVertices.push_back(debugVertex);
		// If the bone has a parent, its position
		// would be the end of the parent bone segment
		if (node->parent)
		{
			debugVertex.position = parentVQS.GetTranslate();
			debugBoneVertices.push_back(debugVertex);
		}

		// find offset matrix in skinningInfoMap (how each bone relates to original T-pose)
		// transforms vertices from the original position of the mesh vertices
		// to match how the bone has moved
		auto& boneInfoMap = animationA->GetSkinningInfoMap();

		auto iter = boneInfoMap.find(nodeName);
		if (iter != boneInfoMap.end())
		{
			int index = iter->second.id;
			glm::mat4 offsetMatrix = iter->second.offsetMatrix;
			finalBoneMatrices[index] = Math::VQS::VQSToGLMMat4(globalVQS) * offsetMatrix;
		}
		for (int i = 0; i < node->childrenCount; i++)
		{
			calculateBoneTransformVQS(blendTree , &node->children[i], globalVQS);
		}
	}
#pragma endregion

#pragma region Getters & Setters
	void Animator::SetAnimation(Animation* animation)
	{
		currentAnimation = animation;
		currentTime = 0.0f;
		AllocateDebugBoneBuffer();
	}

	void Animator::SetBlendTree(BlendTree* blendtree)
	{
		blendTree = blendtree;
		currentTime = 0.0f;
		AllocateDebugBoneBuffer();
	}

	void Animator::PlayAnimation()
	{
		if (currentAnimation == nullptr)
		{
			std::cerr << "No animation assigned to animator" << std::endl;
			assert(false);
		}
		currentTime = 0.0f;
		isPlaying = true;
	}

	void Animator::StopAnimation()
	{
		currentTime = 0.0f;
		isPlaying = false;
	}

	void Animator::SetSpeed(float speed)
	{
		this->speed = speed;
	}
#pragma endregion

#pragma region Debug
	void Animator::AllocateDebugBoneBuffer()
	{
		if(animationType==AnimationType::SINGLE_ANIMATION)
		{
			if (currentAnimation == nullptr)
			{
				std::cerr << "No animation assigned to animator" << std::endl;
				assert(false);
			}
			// Calculate first animation frame to get all bone vertices
			currentTime = 0;
			debugBoneVertices.clear();
			if (AnimationConfig::calculateBonesWithVQS)
			{
				calculateBoneTransformVQS(&currentAnimation->GetRootNodeOfHierarhcy(), Math::VQS());
			}
			else
			{
				calculateBoneTransformMatrix(&currentAnimation->GetRootNodeOfHierarhcy(), glm::mat4(1.0f));
			}
		}
		else if (animationType == AnimationType::BLEND_TREE)
		{
			if (blendTree == nullptr)
			{
				std::cerr << "No blendtree assigned to animator" << std::endl;
				assert(false);
			}
			// Calculate first animation frame to get all bone vertices
			currentTime = 0;
			debugBoneVertices.clear();
			auto animation = blendTree->GetAnimationA();
			if (AnimationConfig::calculateBonesWithVQS)
			{
				calculateBoneTransformVQS(&animation->GetRootNodeOfHierarhcy(), Math::VQS());
			}
			else
			{
				calculateBoneTransformMatrix(&animation->GetRootNodeOfHierarhcy(), glm::mat4(1.0f));
			}
		}

		// Allocate Buffer
		auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
		std::vector<Vertex>& bonevertices = debugBoneVertices;
		auto vertexCount = static_cast<uint32_t>(bonevertices.size());
		uint32_t vertexSize = sizeof(bonevertices[0]);

		vkDeviceWaitIdle(graphicsManager->GetIHCDevice()->GetDevice());

		for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
		{
			debugBoneBuffers[i] = std::make_unique<Graphics::IHCBuffer>(
				*graphicsManager->GetIHCDevice(),
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			debugBoneBuffers[i]->Map();
		}
	}
	void Animator::UpdateDebugBoneBuffer(FrameInfo& frameInfo)
	{
		debugBoneBuffers[frameInfo.frameIndex]->WriteToBuffer((void*)debugBoneVertices.data());
		debugBoneBuffers[frameInfo.frameIndex]->Flush();
	}
	void Animator::DrawDebugBoneBuffer(FrameInfo& frameInfo)
	{
		VkBuffer buffers[] = { debugBoneBuffers[frameInfo.frameIndex]->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, buffers, offsets);
		vkCmdDraw(frameInfo.commandBuffer, debugBoneVertices.size(), 1, 0, 0);
	}
#pragma endregion


}
