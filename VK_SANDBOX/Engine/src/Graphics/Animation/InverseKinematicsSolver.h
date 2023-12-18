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

        void Update();

        float GetTotalDistance() const { return totalDistance; }
        const SkeletalNodeData* GetRoot() const { return root; }
        const SkeletalNodeData* GetEndEffector() const{ return endEffector; }


        //// IK functionality
        void SetModel(Model* m);

        // Call in order
		// Sets correct globalVQS
        void SetGameObjectVQS(Math::VQS vqs);
        // Set IKChain and store base globalVQS, FABRIK rotations will base on this
        void SetRootAndEE(std::string root, std::string EE);  
        void SetTarget(glm::vec3 target);

        // Drawing
        std::vector<glm::mat4>& GetFinalBoneMatrices();

        // Vulkan
        void SetDescriptorSets(std::vector<VkDescriptorSet> set) { descriptorSets = set; }
        std::vector<VkDescriptorSet>& GetDescriptorSets() { return descriptorSets; }
        void SetBuffers(std::vector<IHCBuffer*> buffers) { skeletalBuffers = buffers; }
        std::vector<IHCBuffer*>& GetBuffers() { return skeletalBuffers; }

        // Debug
        std::vector<Vertex>& GetDebugBoneVertices() { return debugBoneVertices; }
        void CreateBindPoseDebugBoneVertices();

    private:

		//// IK functionality
        glm::vec3 target = glm::vec3();

        Model* model = nullptr;
        SkeletalNodeData* root;
        SkeletalNodeData* endEffector;

        void setJoints(std::vector<SkeletalNodeData*>& initialJoints);
        std::vector<SkeletalNodeData*> joints;
        std::vector<Math::Quaternion> initialRotations;
        std::vector<glm::vec3> initialDirections;

        // FABRIK calculation
        int MAX_ITERATIONS = 10;
        float EPSILON_DISTANCE_TO_TARGET = 0.1;
        float EPSILON_DISTANCE_FOR_END_EFFECTOR = 0.2;
        std::vector<float> distances;
        float totalDistance = 0;

        void Solve_FABRIK(glm::vec3 target);
        void propagateIKResultToAffectedHierarchy(SkeletalNodeData* node); // this skips updating IKChain
        void propagateEEHierarchy(SkeletalNodeData* node);

        // Example:
        // [ shoulder -> arm -> forearm -> hand  -> finger1_0 -> finger 1_1 -> finger 1_3 -> finger 1_4 ]
        //                                       -> finger2_0 -> finger 2_1 -> finger 2_3 -> finger 2_4
        //                                       -> ....
        // propagateIKResultToAffectedHierarchy: Propagates branches (finger 2, 3, 4, 5)
        //
        //
        // [ shoulder -> arm -> forearm -> hand ] -> finger1_0 -> finger 1_1 -> finger 1_3 -> finger 1_4
        //                                        -> finger2_0 -> finger 2_1 -> finger 2_3 -> finger 2_4
        //                                           ....
        // propagateEEHierarchy: Propagates E.E (hand -> finger 1, 2, 3, 4, 5)
        //


        // Global/local space
        Math::VQS gameObjectVQS;
        void CalculateGlobalVQS(SkeletalNodeData* node);
        void CalculateLocalVQS(SkeletalNodeData* node);

        // Drawing
        std::vector<glm::mat4> finalBoneMatrices;
        void calculateBoneTransformVQS(SkeletalNodeData* node, Math::VQS parentVQS);

		// Vulkan
        void vulkanResourceSetup();
        void vulkanResourceShutDown();
        std::vector<VkDescriptorSet> descriptorSets;
        std::vector<IHCBuffer*> skeletalBuffers;

        // Debug
        std::vector<Vertex> debugBoneVertices;
        std::vector<std::unique_ptr<IHCBuffer>> debugBoneBuffers;
    };
}
