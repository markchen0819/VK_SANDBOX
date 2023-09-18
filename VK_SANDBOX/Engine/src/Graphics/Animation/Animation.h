#pragma once
#include <map>
#include <assimp/anim.h>
#include "Model.h"
#include "Bone.h"

struct aiNode;

namespace IHCEngine::Graphics
{
	struct AssimpNodeData
	{
		glm::mat4 transformation;
		std::string name;
		int childrenCount;
		std::vector<AssimpNodeData> children;
	};

	class Animation
	{
	public:
		Animation(const std::string animationPath, Model* model);
		~Animation() = default;

		Bone* FindBone(const std::string& name);

		inline float GetTicksPerSecond() { return ticksPerSecond; }
		inline float GetDuration() { return duration; }
		inline const AssimpNodeData& GetRootNode() { return rootNode; }
		inline const std::map<std::string, BoneInfo>& GetBoneIDMap(){return *boneInfoMap;}
	private:
		// Create a hiearchy of AssimpNodeData from the animation file
		void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
		// Bones might be missing in the FBX model.
		// Update missing bones using this animation file
		void ReadMissingBones(const aiAnimation* animation, Model& model);

		float duration;
		int ticksPerSecond;
		std::vector<Bone> bones;
		AssimpNodeData rootNode;
		std::map<std::string, BoneInfo>* boneInfoMap = nullptr;

		Model* model;
		std::string animationPath;

	};

}
