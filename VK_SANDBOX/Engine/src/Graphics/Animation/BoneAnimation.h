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

        std::string GetBoneName() const { return boneName; }
        int GetBoneID() const { return id; }

        void Update(float animationTime);

        // Matrix 
        glm::mat4 GetLocalTransform() const { return localTransform; }

        // VQS
        Math::VQS GetLocalTransformVQS() const { return localVQS; }

	private:

        std::string boneName;
        int id;
        const aiNodeAnim* channel;
        int numPositions;
        int numRotations;
        int numScales;
        std::vector<KeyPosition> keyPositions;
        std::vector<KeyRotation> keyRotations;
        std::vector<KeyScale> keyScales;

        // Gets normalized value for Lerp & Interpolate used for interpolation
        float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

        // Matrix 
        glm::mat4 localTransform;
        int GetPositionIndex(float animationTime); // Current Index to interpolate to based on the current animation time
        int GetRotationIndex(float animationTime);
        int GetScaleIndex(float animationTime);
        glm::mat4 interpolatePosition(float animationTime); // Interpolate
        glm::mat4 interpolateRotation(float animationTime);
        glm::mat4 interpolateScaling(float animationTime);

        // VQS
        Math::VQS localVQS;
        glm::vec3 GetVQSIndices(float animationTime); // keyframes of pos/rot/scale might not align
        Math::VQS interpolateVQS(float animationTime);
	};
}
