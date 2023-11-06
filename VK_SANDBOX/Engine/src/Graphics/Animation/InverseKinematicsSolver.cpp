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
		// from root to E.E.
		joints = initialJoints;
        totalDistance = 0;

		for (size_t i = 1; i < joints.size(); i++) 
		{
            float distance = glm::length(joints[i]->globalVQS.GetTranslate() - joints[i - 1]->globalVQS.GetTranslate());
			distances.push_back(distance);
            totalDistance += distance;
		}

		// E.E to root
		//std::reverse(distances.begin(),distances.end());
		//std::reverse(joints.begin(), joints.end());
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
		std::vector<glm::vec3> pointsForCalculation;
		glm::vec3 rootPos = joints.front()->globalVQS.GetTranslate();
        while (iteration < MAX_ITERATIONS && distanceFromEEtoTargetPos > EPSILON_DISTANCE_TO_TARGET)
        {

			// Backward
			joints.back()->globalVQS.SetTranslate(target);
			for (int i = joints.size() - 2; i >= 0; --i) 
			{
				glm::vec3 pointA = joints[i]->globalVQS.GetTranslate();
				glm::vec3 pointB = joints[i + 1]->globalVQS.GetTranslate();
				glm::vec3 dir = glm::normalize(pointA - pointB);
				pointA = pointB + dir * distances[i];
				joints[i]->globalVQS.SetTranslate(pointA);
			}

			// Keep root in place
			joints.front()->globalVQS.SetTranslate(rootPos);

			// Forward
			for (size_t i = 1; i < joints.size(); i++)
			{
				glm::vec3 pointA = joints[i - 1]->globalVQS.GetTranslate();
				glm::vec3 dir = glm::normalize(joints[i]->globalVQS.GetTranslate() - pointA);
				glm::vec3 pointB = pointA + dir * distances[i - 1];
				joints[i]->globalVQS.SetTranslate(pointB);
			}



   //         // Backward reaching (E.E. to root)
			//pointsForCalculation.push_back(target);
   //         for (int i = joints.size() - 2; i >= 0; --i)
   //         {
   //             glm::vec3 pointA = joints[i]->globalVQS.GetTranslate();
   //             glm::vec3 pointB = joints[i+1]->globalVQS.GetTranslate();
			//	glm::vec3 pointC = pointsForCalculation[joints.size() - 2 - i];
   //             const float distanceAB = distances[i];
   //             glm::vec3 dirAC = glm::normalize(pointA - pointC);
			//	glm::vec3 target = pointB + dirAC * distanceAB;
			//	pointsForCalculation.push_back(target);
   //         }

			//joints.front()->globalVQS.SetTranslate(rootPos);
			//std::reverse(pointsForCalculation.begin(), pointsForCalculation.end());
   //         // Forward reaching  (root to E.E.)
   //         for (size_t i = 1; i < joints.size(); i++) 
   //         {
   //             glm::vec3 pointA = joints[i-1]->globalVQS.GetTranslate();
   //             glm::vec3 pointB = pointsForCalculation[i];
			//	glm::vec3 pointC = joints[i]->globalVQS.GetTranslate();
   //             const float distanceAC = distances[i-1];
   //             glm::vec3 dirBA = glm::normalize(pointB - pointA);
			//	glm::vec3 target = pointA + dirBA * distanceAC;
			//	joints[i]->globalVQS.SetTranslate(target); // .SetTranslate(glm::vec3(-100, 100, 50));
   //         }
			////test
			////joints.back()->globalVQS.SetTranslate(glm::vec3(100, 100, 2));
   //         // Keep getting closer target
			//auto test2 = joints.back()->globalVQS.GetTranslate();
			//auto test = glm::length(joints.back()->globalVQS.GetTranslate() - target);
            distanceFromEEtoTargetPos = glm::length(joints.back()->globalVQS.GetTranslate() - target);
			pointsForCalculation.clear();
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

	// Apply after solving IK transforms
	void InverseKinematicsSolver::Update()
	{
		debugBoneVertices.clear();

		//Solve;
		//Testing

		calculateBoneTransformVQS(&model->GetRootNodeOfHierarhcy(), Math::VQS());
	}

	void InverseKinematicsSolver::SetModel(Model* m)
	{
		model = m;
		AllocateDebugBoneBuffer();

		auto testEE = model->GetNodeByName("mixamorig:RightHandIndex4_end");
		auto testRoot = model->GetNodeByName("mixamorig:RightForeArm");
		//auto testRoot = model->GetNodeByName("mixamorig:RightShoulder");
		auto testPath = model->GetPathFromRootToEE(testEE, testRoot);
		
		ConvertVQSLocalToGlobal(&model->GetRootNodeOfHierarhcy());
		SetJoints(testPath);
		float scale=0.05;// welp localVQS fucked by global scale
		// -84 152 -2.5
		Solve_FABRIK(glm::vec3(-100, 100, 100));
		ConvertVQSGlobalToLocal(&model->GetRootNodeOfHierarhcy());
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
