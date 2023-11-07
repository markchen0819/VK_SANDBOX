#include "../../pch.h"
#include "InverseKinematicsSolver.h"
#include "../../Core/Locator/GraphicsManagerLocator.h"
#include "../../Graphics/VKWraps/IHCSwapChain.h"
#include "../../Graphics/VKWraps/IHCBuffer.h"
#include "../../Graphics/VKWraps/IHCDevice.h"
#include "AnimationInfo.h"
#include "Model.h"

namespace IHCEngine::Graphics
{
	InverseKinematicsSolver::InverseKinematicsSolver()
	{
		vulkanResourceSetup();

	}
	InverseKinematicsSolver::~InverseKinematicsSolver()
	{
		vulkanResourceShutDown();
	}

	void InverseKinematicsSolver::SetEndEffector(std::string name)
	{
	}

	void InverseKinematicsSolver::ConvertVQSLocalToGlobal(SkeletalNodeData* node)
	{
		if (node->parent!= nullptr)
		{
			const Math::VQS& parentVQS = node->parent->globalVQS;

			node->globalVQS = parentVQS * node->localVQS;
		}
		else
		{
			node->globalVQS = node->localVQS;
		}

		for (unsigned int i = 0; i < node->children.size(); ++i)
		{
			ConvertVQSLocalToGlobal(node->children[i].get());
		}
	}

	void InverseKinematicsSolver::ConvertVQSGlobalToLocal(SkeletalNodeData* node)
	{
		if (node->parent != nullptr) 
		{
			Math::VQS inverseParentVQS = node->parent->globalVQS.Inverse();
			node->localVQS = inverseParentVQS * node->globalVQS;
		}
		else 
		{
			node->localVQS = node->globalVQS;
		}
		for (unsigned int i = 0; i < node->children.size(); ++i) 
		{
			ConvertVQSGlobalToLocal(node->children[i].get());
		}
	}


	void InverseKinematicsSolver::SetJoints(std::vector<SkeletalNodeData*>& initialJoints)
	{
		// From root to E.E.
		joints = initialJoints;
        totalDistance = 0;
		distances.clear();
		initialRotations.clear();
		initialDirections.clear();

		for (size_t i = 1; i < joints.size(); i++) 
		{
			// Direction
			glm::vec3 direction = glm::normalize(joints[i]->globalVQS.GetTranslate() - joints[i - 1]->globalVQS.GetTranslate());
			initialDirections.push_back(direction);

			// Distance
            float distance = glm::length(joints[i]->globalVQS.GetTranslate() - joints[i - 1]->globalVQS.GetTranslate());
			distances.push_back(distance);
            totalDistance += distance;

			// Rotation
			initialRotations.push_back(joints[i]->globalVQS.GetRotation());
		}
	}

	void InverseKinematicsSolver::Solve_FABRIK(glm::vec3 target)
	{
        // Reference
        // https://youtu.be/UNoX65PRehA?si=qRPyM7tonfIDzYfX

        // Cannot reach, too far away
        float distanceFromRootToTarget = glm::length(target - joints[0]->globalVQS.GetTranslate());
        if(distanceFromRootToTarget > totalDistance)
        {
            glm::vec3 dir = glm::normalize(target - joints[0]->globalVQS.GetTranslate());
            target = joints[0]->globalVQS.GetTranslate() + dir * totalDistance;
            // straight line
        }

        // Can reach
        int iteration = 0;
        float distanceFromEEtoTargetPos = glm::length(joints.back()->globalVQS.GetTranslate() - target);
		glm::vec3 rootPos = joints.front()->globalVQS.GetTranslate();

        while (iteration < MAX_ITERATIONS && distanceFromEEtoTargetPos > EPSILON_DISTANCE_TO_TARGET)
        {

			// Backward reaching (E.E. to root)
			joints.back()->globalVQS.SetTranslate(target);
			for (int i = joints.size() - 2; i >= 0; --i) 
			{
				glm::vec3 pointA = joints[i]->globalVQS.GetTranslate();
				glm::vec3 pointB = joints[i + 1]->globalVQS.GetTranslate();
				glm::vec3 dir = glm::normalize(pointA - pointB);
				pointA = pointB + dir * distances[i];
				joints[i]->globalVQS.SetTranslate(pointA);
			}

			// Forward reaching  (root to E.E.)
			joints.front()->globalVQS.SetTranslate(rootPos);
			for (size_t i = 1; i < joints.size(); i++) 
			{
				glm::vec3 pointA = joints[i - 1]->globalVQS.GetTranslate();
				glm::vec3 dir = glm::normalize(joints[i]->globalVQS.GetTranslate() - pointA);
				glm::vec3 pointB = pointA + dir * distances[i - 1];
				joints[i]->globalVQS.SetTranslate(pointB);
			}

			// Fix orientation
			for (int i = 0; i < joints.size() - 1; ++i)
			{
				glm::vec3 newDir;
				if (i != joints.size() - 2)
				{
					// Direction from current joint to the next joint
					newDir = glm::normalize(joints[i + 1]->globalVQS.GetTranslate() - joints[i]->globalVQS.GetTranslate());
				}
				else 
				{
					// Direction from E.E. to target
					newDir = glm::normalize(target - joints[i]->globalVQS.GetTranslate());
				}

				glm::vec3 initialDir = initialDirections[i];

				// Calculate rotation from initialDir to the new direction
				Math::Quaternion newRotation = Math::Quaternion::FromToRotation(initialDir, newDir);

				// Combine with initial local rotation
				Math::Quaternion finalRotation = newRotation * initialRotations[i];

				// Apply the new rotation
				joints[i]->globalVQS.SetRotation(finalRotation);
			}

			// Continue looping
			distanceFromEEtoTargetPos = glm::length(joints.back()->globalVQS.GetTranslate() - target);
			iteration++;
        }

	}

	void InverseKinematicsSolver::Solve_CCD(glm::vec3 target)
	{
		//// Local space to world space
		//skeleton[1]->MoveAllToWorldSpace();

		//// Get the node before the last one
		//// Last one is the end effector
		//SkeletonNode* endEffector = skeleton[skeleton.size() - 1];

		//// End effector position
		//vec3 Pc = endEffector->globalVQS.translate;
		//// Target position
		//vec3 Pd = target->Position();

		//// will be used inside the loop
		//// Vck = vector from k'th joint to Pc
		//// Vdk = vector from k'th joint to Pd
		//vec3 Vck, Vdk;
		//vec3 PV;
		//while (glm::distance(Pc, Pd) > EPSILON_DISTANCE_TO_TARGET)
		//{ 
		//	PV = Pc;
		//	for (unsigned int i = skeleton.size() - 2; i > 0; --i) 
		//	{
		//		SkeletonNode* currentJoint = skeleton[i];
		//		vec3 currentJointPosition = currentJoint->globalVQS.translate;

		//		// Set vectors from k'th joint
		//		Vck = glm::normalize(Pc - currentJointPosition);
		//		Vdk = glm::normalize(Pd - currentJointPosition);

		//		float cosAngle = glm::dot(Vdk, Vck);
		//		if (cosAngle < 0.999999f) 
		//		{    //which mean we rotated a little bit
		//			// Calculate angle and axis
		//			float angle = acosf(cosAngle);
		//			glm::vec3 axis = glm::cross(Vck, Vdk);
		//			// Unit axis
		//			axis = glm::normalize(axis);
		//			// Create the quaternion to rotate the joint
		//			Quaternion rotator(axis, angle);
		//			// Update local rotation of the current joint
		//			Quaternion& originalRotation = currentJoint->localVQS.rotate;
		//			originalRotation = rotator * originalRotation;
		//			// Update skeleton's world coordinates starting from this
		//			currentJoint->MoveAllToWorldSpace();

		//			// Update Pc - end effector is updated when the world space is recalculated
		//			Pc = endEffector->globalVQS.translate;
		//		}
		//	}

		//	if (glm::distance(Pc, PV) < EPSILON_DISTANCE_FOR_END_EFFECTOR)
		//	{
		//		return;
		//	}
		//}
	}

	void InverseKinematicsSolver::FixEEChildrens(SkeletalNodeData* node)
	{
		// If E.E set to forearm, fingers are not moved
		// this moves the finger

		Math::VQS endEffectorGlobalVQS = node->globalVQS;

		for (size_t i = 0; i < node->children.size(); ++i)
		{
			node->children[i]->globalVQS = endEffectorGlobalVQS * node->children[i]->localVQS;
			FixEEChildrens(node->children[i].get());
		}
	}

	// Apply after solving IK transforms
	void InverseKinematicsSolver::Update()
	{
		debugBoneVertices.clear();

		// Solve
		//target = glm::vec3(-100, 100, 100);
		Solve_FABRIK(target);// glm::vec3(-100, 100, 100)); // -84 152 -2.5
		FixEEChildrens(endEffector);
		ConvertVQSGlobalToLocal(&model->GetRootNodeOfHierarhcy());

		// Update finalBoneMatrices
		calculateBoneTransformVQS(&model->GetRootNodeOfHierarhcy(), Math::VQS());
	}

	void InverseKinematicsSolver::SetModel(Model* m)
	{
		model = m;
		AllocateDebugBoneBuffer();
		ConvertVQSLocalToGlobal(&model->GetRootNodeOfHierarhcy());
	}

	void InverseKinematicsSolver::SetRootAndEE(std::string r, std::string ee)
	{
		root = model->GetNodeByName(r);
		endEffector= model->GetNodeByName(ee);
		auto path = model->GetPathFromRootToEE(endEffector, root);

		// Calculate globalVQS as IKSolver works in world space
		SetJoints(path);
	}

	void InverseKinematicsSolver::SetTarget(glm::vec3 target)
	{
		this->target = target;
	}

	std::vector<glm::mat4>& InverseKinematicsSolver::GetFinalBoneMatrices()
	{
		return finalBoneMatrices;
	}

	void InverseKinematicsSolver::calculateBoneTransformVQS(SkeletalNodeData* node, Math::VQS parentVQS)
	{
		// Starting from root node
		const std::string& nodeName = node->name;
		// Use the VQS after IK is applied
		Math::VQS nodeVQS = node->localVQS;
		// Convert bone from local space into global space
		Math::VQS globalVQS = parentVQS * nodeVQS;

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
		auto& boneInfoMap = model->GetSkinningInfoMap();

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

#pragma region Vulkan resources
	void InverseKinematicsSolver::vulkanResourceSetup()
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
	void InverseKinematicsSolver::vulkanResourceShutDown()
	{
		auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();
		graphicsAssetCreator.DestroySkeletalData(this);
	}
	void InverseKinematicsSolver::AllocateDebugBoneBuffer()
	{
		calculateBoneTransformVQS(&model->GetRootNodeOfHierarhcy(), Math::VQS());

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

	void InverseKinematicsSolver::UpdateDebugBoneBuffer(FrameInfo& frameInfo)
	{
		debugBoneBuffers[frameInfo.frameIndex]->WriteToBuffer((void*)debugBoneVertices.data());
		debugBoneBuffers[frameInfo.frameIndex]->Flush();
	}

	void InverseKinematicsSolver::DrawDebugBoneBuffer(FrameInfo& frameInfo)
	{
		VkBuffer buffers[] = { debugBoneBuffers[frameInfo.frameIndex]->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, buffers, offsets);
		vkCmdDraw(frameInfo.commandBuffer, debugBoneVertices.size(), 1, 0, 0);
	}
#pragma endregion


}
