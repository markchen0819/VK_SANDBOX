#pragma once
#include "../VKWraps/VKHelpers.h"
#include "../../Math/VQS.h"

namespace IHCEngine::Graphics
{
	struct SkeletalNodeData;
	class IHCBuffer;
	struct FrameInfo;
	class Model;

	class InverseKinematicsSolver
    {
    public:

        InverseKinematicsSolver();
        ~InverseKinematicsSolver();

        // IK functionality

        void SetModel(Model* m);
        void SetJoints(std::vector<SkeletalNodeData*>& initialJoints);
        void SetEndEffector(std::string name);
        void ConvertVQSLocalToGlobal(SkeletalNodeData* node);
        void ConvertVQSGlobalToLocal(SkeletalNodeData* node);
        void Solve_FABRIK(glm::vec3 target);
        void Solve_CCD(glm::vec3 target);
        void FixEEChildrens(SkeletalNodeData* node);
        void Update();


        // Drawing
        std::vector<glm::mat4>& GetFinalBoneMatrices();
        void calculateBoneTransformVQS(SkeletalNodeData* node, Math::VQS parentVQS);

        // Vulkan
        void SetDescriptorSets(std::vector<VkDescriptorSet> set) { descriptorSets = set; }
        std::vector<VkDescriptorSet>& GetDescriptorSets() { return descriptorSets; }
        void SetBuffers(std::vector<IHCBuffer*> buffers) { skeletalBuffers = buffers; }
        std::vector<IHCBuffer*>& GetBuffers() { return skeletalBuffers; }
        // Debug
        void AllocateDebugBoneBuffer();
        void UpdateDebugBoneBuffer(FrameInfo& frameInfo);
        void DrawDebugBoneBuffer(FrameInfo& frameInfo);

    private:

        // IK
        int MAX_ITERATIONS = 10;
        float EPSILON_DISTANCE_TO_TARGET = 0.1;
        float EPSILON_DISTANCE_FOR_END_EFFECTOR = 0.1;
        std::vector<SkeletalNodeData*> joints;
        std::vector<Math::Quaternion> initialRotations;
        std::vector<glm::vec3> initialDirections;
        std::vector<float> distances;
        float totalDistance = 0;

        Model* model = nullptr;
        std::vector<glm::mat4> finalBoneMatrices;
        void applyIKResultToBoneTransformVQS();


        void vulkanResourceSetup();
        void vulkanResourceShutDown();
        // Vulkan
        std::vector<VkDescriptorSet> descriptorSets;
        std::vector<IHCBuffer*> skeletalBuffers;
        // Debug
        std::vector<Vertex> debugBoneVertices;
        std::vector<std::unique_ptr<IHCBuffer>> debugBoneBuffers;
    };
}
