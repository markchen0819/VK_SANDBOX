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

#pragma region Fundamental setup & update
	void InverseKinematicsSolver::SetModel(Model* m)
	{
		model = m;
		CreateBindPoseDebugBoneVertices();
	}
	void InverseKinematicsSolver::SetGameObjectVQS(Math::VQS vqs)
	{
		gameObjectVQS = vqs;
		CalculateGlobalVQS(&model->GetRootNodeOfHierarhcy());
	}

	void InverseKinematicsSolver::Update()
	{
		debugBoneVertices.clear();

		if (joints.size() == 0) return; // No IKChain set

		// Solve
		Solve_FABRIK(target);

		CalculateLocalVQS(&model->GetRootNodeOfHierarhcy());

		// Update finalBoneMatrices
		calculateBoneTransformVQS(&model->GetRootNodeOfHierarhcy(), Math::VQS());
	}
#pragma endregion

#pragma region VQS global and local

	void InverseKinematicsSolver::CalculateGlobalVQS(SkeletalNodeData* node)
	{
		if (node->parent != nullptr)
		{
			const Math::VQS& parentVQS = node->parent->globalVQS;
			node->globalVQS = parentVQS * node->localVQS;
		}
		else
		{
			// root
			node->globalVQS = gameObjectVQS * node->localVQS;
		}

		for (unsigned int i = 0; i < node->children.size(); ++i)
		{
			CalculateGlobalVQS(node->children[i].get());
		}
	}

	void InverseKinematicsSolver::CalculateLocalVQS(SkeletalNodeData* node)
	{
		if (node->parent != nullptr)
		{
			Math::VQS inverseParentVQS = node->parent->globalVQS.Inverse();
			node->localVQS = inverseParentVQS * node->globalVQS;
		}
		else
		{
			// root
			node->localVQS = gameObjectVQS.Inverse() * node->globalVQS;
		}
		for (unsigned int i = 0; i < node->children.size(); ++i)
		{
			CalculateLocalVQS(node->children[i].get());
		}
	}


#pragma endregion

#pragma region IK Setup
	// Make sure global VQS is setup first before these steps
	void InverseKinematicsSolver::SetTarget(glm::vec3 target)
	{
		this->target = target;
	}
	void InverseKinematicsSolver::SetRootAndEE(std::string r, std::string ee)
	{
		root = model->GetNodeByName(r);
		endEffector = model->GetNodeByName(ee);
		auto manuipulator = model->GetPathFromRootToEE(endEffector, root);

		setJoints(manuipulator);
	}



	void InverseKinematicsSolver::setJoints(std::vector<SkeletalNodeData*>& initialJoints)
	{
		// Remember to get globalVQS calculated before this step
		// as IKSolver works in world space

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


#pragma endregion

#pragma region Joint Constraints
	//void InverseKinematicsSolver::SetConstraints(std::vector<JointConstraint>& jointConstraints)
	//{
	//	this->jointConstraints = jointConstraints;
	//}
	
#pragma endregion

#pragma region IK Solver Logic
	void InverseKinematicsSolver::Solve_FABRIK(glm::vec3 target)
	{
        // Reference
        // https://youtu.be/UNoX65PRehA?si=qRPyM7tonfIDzYfX

        // Cannot reach, too far away -> adjust target to be reachable on the tangent
        float distanceFromRootToTarget = glm::length(target - joints[0]->globalVQS.GetTranslate());
        if(distanceFromRootToTarget > totalDistance)
        {
            glm::vec3 dir = glm::normalize(target - joints[0]->globalVQS.GetTranslate());
            target = joints[0]->globalVQS.GetTranslate() + dir * totalDistance;
        }
        // Can reach
        int iteration = 0;
        float distanceFromEEtoTargetPos = glm::length(joints.back()->globalVQS.GetTranslate() - target);
		glm::vec3 rootPos = joints.front()->globalVQS.GetTranslate();
        while (iteration < MAX_ITERATIONS && distanceFromEEtoTargetPos > EPSILON_DISTANCE_TO_TARGET)
        {
			// Backward reaching (E.E. to root)
			joints.back()->globalVQS.SetTranslate(target);
			for (int i = static_cast<int>(joints.size() - 2); i >= 0; --i) 
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
				// Calculate rotation from initialDir to the new direction (in global space)
				Math::Quaternion newRotation = Math::Quaternion::FromToRotation(initialDir, newDir);
				// Combine with initial rotation
				Math::Quaternion finalRotation = newRotation * initialRotations[i];
				// Apply the new rotation
				joints[i]->globalVQS.SetRotation(finalRotation);
			}

			// Propagate hierarchy affected by IKChain root
			propagateIKResultToAffectedHierarchy(joints[0]);

			// Continue looping
			distanceFromEEtoTargetPos = glm::length(joints.back()->globalVQS.GetTranslate() - target);
			iteration++;
        }
		// Propagate hierarchy affected by IKChain E.E.
		propagateEEHierarchy(endEffector);
	}
	void InverseKinematicsSolver::propagateEEHierarchy(SkeletalNodeData* node)
	{
		// If E.E set to forearm, fingers are not moved
        // this moves the finger

		if (node == nullptr)
		{
			return;
		}
		// Assuming each node's localVQS is already set relative to its parent
		// and that the parent's globalVQS is correct, calculate this node's globalVQS
		if (node->parent != nullptr)
		{
			// If there is a parent, we update based on the parent's globalVQS
			node->globalVQS = node->parent->globalVQS * node->localVQS;
		}
		// Recursively update the global transform for all children.
		for (size_t i = 0; i < node->children.size(); ++i)
		{
			propagateEEHierarchy(node->children[i].get());
		}
	}
#pragma endregion

#pragma region Render IK result
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
#pragma endregion

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

	void InverseKinematicsSolver::CreateBindPoseDebugBoneVertices()
	{
		calculateBoneTransformVQS(&model->GetRootNodeOfHierarhcy(), Math::VQS());
	}


	void InverseKinematicsSolver::propagateIKResultToAffectedHierarchy(SkeletalNodeData* node)
	{
		bool isPartOfManipulator = false;
		for (size_t i = 0; i < joints.size(); ++i)
		{
			if (node == joints[i])
			{
				isPartOfManipulator = true;
				break;
			}
		}

		if(isPartOfManipulator)
		{
			// We don't update it as this is our result
			// just applying result to unaffected ones
		}
		else
		{
			node->globalVQS = node->parent->globalVQS * node->localVQS;
		}
		for (size_t i = 0; i < node->children.size(); ++i)
		{
			propagateIKResultToAffectedHierarchy(node->children[i].get());
		}
	}
#pragma endregion

}
