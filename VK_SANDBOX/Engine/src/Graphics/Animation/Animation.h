#pragma once
#include <assimp/anim.h>
#include "Model.h"
#include "BoneAnimation.h"
struct aiNode;

namespace IHCEngine::Graphics
{
	class Animation
	{
	public:
		Animation(const std::string animationPath, Model* model);
		~Animation() = default;

		float GetTicksPerSecond() { return ticksPerSecond; }
		float GetDuration() { return duration; }

		// For animator, to calculate bones of animation
		AssimpNodeData& GetRootNodeOfHierarhcy() { return model->GetRootNodeOfHierarhcy(); }
		BoneAnimation* FindBone(const std::string& name);
		std::map<std::string, BoneInfo>& GetBoneInfoMap() { return model->GetBoneInfoMap(); }

	private:

		std::string animationPath;
		float duration;
		int ticksPerSecond;
		std::vector<BoneAnimation> boneAnimations;
		Model* model; // model is needed for boneInfo

		void extractAnimationData(const aiAnimation* aiAnim);

	};
}
