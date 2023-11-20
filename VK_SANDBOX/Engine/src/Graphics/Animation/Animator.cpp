#include "../../pch.h"
#include "Animator.h"

#include <glm/gtx/matrix_decompose.hpp>

#include "../../Core/Time/Time.h"
#include "../../Core/Locator/GraphicsManagerLocator.h"
#include "../../Graphics/VKWraps/IHCSwapChain.h"
#include "../../Graphics/VKWraps/IHCBuffer.h"
#include "../../Graphics/VKWraps/IHCDevice.h"
#include "../../Math/VQS.h"

#include "AnimationConfig.h"
#include "Animation.h"
#include "BoneAnimation.h"
#include "BlendTree.h"

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
			// When we set animation
			// we assume they are processed with same model
			// -> same node hierarchy -> choose 1 of the 2 animation
			// also assume animationA and animationB motions are synced using same time

			auto animationA = blendTree->GetAnimationA();
			currentTime += animationA->GetTicksPerSecond() * speed * dt;
			currentTime = fmod(currentTime, animationA->GetDuration());

			if (AnimationConfig::calculateBonesWithVQS)
			{
				calculateBoneTransformVQS(blendTree, &animationA->GetRootNodeOfHierarhcy(), Math::VQS());
			}
			else
			{
				calculateBoneTransformMatrix(blendTree,&animationA->GetRootNodeOfHierarhcy(), glm::mat4(1.0f));
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
		glm::mat4 nodeTransform = node->transformation_Matrix; // ex: arm bone bent 45 degrees

		// Check if there is a bone in the root node related to the animation
		BoneAnimation* boneAnimation = currentAnimation->FindBone(nodeName);
		if (boneAnimation)
		{
			// interpolates bone transformation
			// and return local bone transform matrix 
			boneAnimation->Update(currentTime);
			nodeTransform = boneAnimation->GetLocalTransformMatrix(); // ex: arm bone change to 65 degrees
		}
		// Convert bone from local space into global space
		glm::mat4 globalTransformation = parentTransform * nodeTransform; // ex: arm bone in the world (consider shoulder)

		// If the bone has a parent, its position
		// would be the end of the parent bone segment
		if (node->parent)
		{
			Vertex debugVertex;
			debugVertex.color = glm::vec3(0.0, 1.0, 0.0);
			debugVertex.position = glm::vec3(globalTransformation[3]);
			debugBoneVertices.push_back(debugVertex);
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
			calculateBoneTransformMatrix(node->children[i].get(), globalTransformation);
		}
	}
	void Animator::calculateBoneTransformVQS(const SkeletalNodeData* node, Math::VQS parentVQS)
	{
		// Starting from root node of current animation
		const std::string& nodeName = node->name;
		Math::VQS nodeVQS = node->localVQS; 

		// Check if there is a bone in the root node related to the animation
		BoneAnimation* boneAnimation = currentAnimation->FindBone(nodeName);
		if (boneAnimation)
		{
			// interpolates bone transformation
			// and return local bone transform matrix 
			boneAnimation->Update(currentTime);
			nodeVQS = boneAnimation->GetLocalTransformVQS(); // ex: arm bone change to 65 degrees
		}
		// Convert bone from local space into global space
		Math::VQS globalVQS = parentVQS * nodeVQS; // ex: arm bone in the world (consider shoulder)

		// If the bone has a parent, its position
		// would be the end of the parent bone segment
		if (node->parent)
		{
			Vertex debugVertex;
			debugVertex.color = glm::vec3(0.0, 1.0, 0.0);
			debugVertex.position = globalVQS.GetTranslate();
			debugBoneVertices.push_back(debugVertex);
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
			calculateBoneTransformVQS(node->children[i].get(), globalVQS);
		}
	}


	void Animator::calculateBoneTransformMatrix(BlendTree* blendTree, const SkeletalNodeData* node,
		glm::mat4 parentTransform)
	{
		Animation* animationA = blendTree->GetAnimationA();
		Animation* animationB = blendTree->GetAnimationB();

		// Starting from root node of model
		const std::string& nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation_Matrix;

		// Check if there is a bone in the root node related to the animation
		BoneAnimation* boneAnimationA = animationA->FindBone(nodeName);
		BoneAnimation* boneAnimationB = animationB->FindBone(nodeName);
		if (boneAnimationA && boneAnimationB)
		{
			// Tri-linear interpolation 
			boneAnimationA->Update(currentTime);
			boneAnimationB->Update(currentTime);

			glm::mat4 mat1 = boneAnimationA->GetLocalTransformMatrix();
			glm::mat4 mat2 = boneAnimationB->GetLocalTransformMatrix();
			float blendFactor = blendTree->GetBlendFactor();

			// Not a good way here, VQS seems better
			glm::vec3 skew;
			glm::vec4 perspective;
			// Decompose the first matrix into its components
			glm::vec3 translation1;
			glm::vec3 scale1;
			glm::quat rotation1;
			glm::decompose(mat1, scale1, rotation1, translation1, skew, perspective);
			// Decompose the second matrix into its components
			glm::vec3 translation2;
			glm::vec3 scale2;
			glm::quat rotation2;
			glm::decompose(mat2, scale2, rotation2, translation2, skew, perspective);
			// Interpolate
			glm::vec3 finalTranslation = glm::mix(translation1, translation2, blendFactor);
			glm::quat finalRotation = glm::slerp(rotation1, rotation2, blendFactor);
			glm::vec3 finalScale = glm::mix(scale1, scale2, blendFactor);

			// Reconstruct the matrix (TRS)
			glm::mat4 finalMatrix =
				glm::translate(glm::mat4(1.0f), finalTranslation) *
				glm::mat4_cast(finalRotation) *
				glm::scale(glm::mat4(1.0f), finalScale);

			nodeTransform = finalMatrix;
		}

		// Convert bone from local space into global space
		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		// If the bone has a parent, its position
		// would be the end of the parent bone segment
		if (node->parent)
		{
			Vertex debugVertex;
			debugVertex.color = glm::vec3(0.0, 1.0, 0.0);
			debugVertex.position = glm::vec3(globalTransformation[3]);
			debugBoneVertices.push_back(debugVertex);
			debugVertex.position = glm::vec3(parentTransform[3]);
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
			finalBoneMatrices[index] = globalTransformation * offsetMatrix;
		}
		for (int i = 0; i < node->childrenCount; i++)
		{
			calculateBoneTransformMatrix(blendTree, node->children[i].get(), globalTransformation);
		}

	}

	void Animator::calculateBoneTransformVQS(BlendTree* blendTree, const SkeletalNodeData* node, Math::VQS parentVQS)
	{
		Animation* animationA = blendTree->GetAnimationA();
		Animation* animationB = blendTree->GetAnimationB();

		// Starting from root node of model
		const std::string& nodeName = node->name;
		Math::VQS nodeVQS = node->localVQS;

		// Check if there is a bone in the root node related to the animation
		BoneAnimation* boneAnimationA = animationA->FindBone(nodeName);
		BoneAnimation* boneAnimationB = animationB->FindBone(nodeName);
		if (boneAnimationA && boneAnimationB)
		{
			// Tri-linear interpolation 
			boneAnimationA->Update(currentTime);
			boneAnimationB->Update(currentTime);
			auto vqs1 = boneAnimationA->GetLocalTransformVQS();
			auto vqs2 = boneAnimationB->GetLocalTransformVQS();
			float blendFactor = blendTree->GetBlendFactor();
			glm::vec3 vec = glm::vec3(blendFactor, blendFactor, blendFactor);
			auto resultVQS = Math::VQS::Interpolate(vqs1, vqs2, vec);
			nodeVQS = resultVQS;
		}

		// Convert bone from local space into global space
		Math::VQS globalVQS = parentVQS * nodeVQS; // ex: arm bone in the world (consider shoulder)

		// If the bone has a parent, its position
		// would be the end of the parent bone segment
		if (node->parent)
		{
			Vertex debugVertex;
			debugVertex.color = glm::vec3(0.0, 1.0, 0.0);
			debugVertex.position = globalVQS.GetTranslate();
			debugBoneVertices.push_back(debugVertex);
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
			calculateBoneTransformVQS(blendTree , node->children[i].get(), globalVQS);
		}
	}

#pragma endregion

	void Animator::OverwriteAnimationLocalVQStoModelLocalVQS(SkeletalNodeData* node)
	{
		const std::string& nodeName = node->name;
		BoneAnimation* boneAnimation = currentAnimation->FindBone(nodeName);
		if (boneAnimation)
		{
			node->localVQS = boneAnimation->GetLocalTransformVQS();
		}
		for (int i = 0; i < node->childrenCount; i++)
		{
			OverwriteAnimationLocalVQStoModelLocalVQS(node->children[i].get());
		}
	}
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
