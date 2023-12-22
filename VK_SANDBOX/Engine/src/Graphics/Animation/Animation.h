#pragma once
#include <assimp/anim.h>
#include "Model.h"
#include "BoneAnimation.h"
struct aiNode;

namespace IHCEngine::Graphics
{
	// Animation
	//
	// Contain multiple BoneAnimations (each bone has its own keyframe data)
	// Requires model to match Bone <-> BoneAnimation

	class Animation
	{
	public:
		Animation(const std::string animationPath, Model* model);
		~Animation() = default;

		float GetTicksPerSecond() { return static_cast<float>(ticksPerSecond); }
		float GetDuration() { return duration; }

		// For animator, to calculate boneAnimations
		SkeletalNodeData& GetRootNodeOfHierarhcy() { return model->GetRootNodeOfHierarhcy(); }
		BoneAnimation* FindBone(const std::string& name);
		std::map<std::string, SkinningInfo>& GetSkinningInfoMap() { return model->GetSkinningInfoMap(); }

	private:

		Model* model;
		std::string animationPath;
		float duration;
		int ticksPerSecond;

		std::vector<BoneAnimation> boneAnimations;
		void extractAnimationData(const aiAnimation* aiAnim);
	};
}
