#pragma once
#include "AnimationInfo.h"
#include "../../Math/VQS.h"
struct aiNodeAnim;

namespace IHCEngine::Graphics
{
	// Individual bone animation
	// reads all keyframes data from aiNodeAnim
	// interpolate between its keys (TRS) based on the current animation time.

	class BoneAnimation
	{
	public:
        BoneAnimation(
            const std::string& name, 
            int boneID,
            const aiNodeAnim* channel,
            std::vector<KeyPosition> kP,
            std::vector<KeyRotation> kR,
            std::vector<KeyScale> kS
        );;

        void Update(float animationTime);
        glm::mat4 GetLocalTransform() const { return localTransform; }
        Math::VQS GetLocalTransformVQS() const { return localVQS; }

        std::string GetBoneName() const { return boneName; }
        int GetBoneID() const { return id; }

        // Current Index to interpolate to based on the current animation time
        int GetPositionIndex(float animationTime);
        int GetRotationIndex(float animationTime);
        int GetScaleIndex(float animationTime);
        glm::vec3 GetVQSIndices(float animationTime); // keyframes of pos/rot/scale might not align

	private:

        // Gets normalized value for Lerp & Interpolate used for interpolation
        float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
        glm::mat4 interpolatePosition(float animationTime);
        glm::mat4 interpolateRotation(float animationTime);
        glm::mat4 interpolateScaling(float animationTime);
        Math::VQS interpolateVQS(float animationTime);

        std::string boneName;
        int id;
        const aiNodeAnim* channel;

        glm::mat4 localTransform;
        Math::VQS localVQS;

        int numPositions;
        int numRotations;
        int numScales;
        std::vector<KeyPosition> keyPositions;
        std::vector<KeyRotation> keyRotations;
        std::vector<KeyScale> keyScales;
	};
}
