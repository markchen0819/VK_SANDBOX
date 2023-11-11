#include "InverseKinematicsViewer.h"

#include "../../../../Engine/src/Core/Scene/GameObject.h"
#include "../../../../Engine/src/Core/Scene/Components/LineRendererComponent.h"
#include "../../../../Engine/src/Core/Scene/Scene.h"
#include "../../../../Engine/src/Core/Scene/Components/IKComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/MeshComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/TextureComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/PipelineComponent.h"
#include "../../../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Core/Scene/Components/AnimatorComponent.h"
#include "../../../../Engine/src/Input/Input.h"
#include "../../../../Engine/src/Core/Time/Time.h"
#include "../../../../Engine/src/Core/Locator/AppWindowLocator.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"

namespace IHCEngine::Component
{
	class PipelineComponent;
}

namespace SampleApplication
{
	InverseKinematicsViewer::InverseKinematicsViewer()
	{}

	InverseKinematicsViewer::~InverseKinematicsViewer()
	{
	}

	void InverseKinematicsViewer::Awake()
	{
		auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
		movingGobj = sceneManager->GetActiveScene()->GetGameObjectByName("targetGobj");
        movingGobj->transform.SetScale(glm::vec3(0.5, 0.5, 0.5));

        IKGobj = sceneManager->GetActiveScene()->GetGameObjectByName("IKGobj");
        ikComponent = IKGobj->GetComponent<IHCEngine::Component::IKComponent>();
        animatorComponent = IKGobj->GetComponent<IHCEngine::Component::AnimatorComponent>();

        // Set this first before Setting RootAndEE for correct globalVQS
        auto pos = IKGobj->transform.GetPosition();
        auto rot = IHCEngine::Math::Quaternion::CreateFromGLMQuat(IKGobj->transform.GetRotationInQuaternion());
        auto scale = IKGobj->transform.GetScale();
        auto gobjVQS = IHCEngine::Math::VQS(pos, rot, scale.x);
        ikComponent->SetGameObjectVQS(gobjVQS);

        //ikComponent->SetRootAndEE("mixamorig:RightShoulder", "mixamorig:RightHand");
        ikComponent->SetRootAndEE("mixamorig:RightShoulder", "mixamorig:RightHandIndex4_end");

        ikComponent->SetActive(false);
        animatorComponent-> SetActive(false);
	}

	void InverseKinematicsViewer::Start() {}

	void InverseKinematicsViewer::Update()
	{
        float dt = IHCEngine::Core::Time::GetDeltaTime();

        TargetObjectInputs();

        if (IHCEngine::Core::Input::IsKeyUp(GLFW_KEY_SPACE))
        {
            trigger = true;
        }
        if (trigger == false) return;

        auto ikPos = IKGobj->transform.GetPosition();
        auto ikRot = IHCEngine::Math::Quaternion::CreateFromGLMQuat(IKGobj->transform.GetRotationInQuaternion());
        auto ikScale = IKGobj->transform.GetScale();
        auto targetPos = movingGobj->transform.GetPosition();
        auto ikPosXZ = glm::vec3(ikPos.x, 0, ikPos.z);
        auto targetPosXZ = glm::vec3(targetPos.x, 0, targetPos.z);
        float distanceToDoIK = ikComponent->GetTotalDistance() - 0.2;

        if(glm::distance(ikPosXZ, targetPosXZ) > distanceToDoIK)
        {
            if(animatorComponent->IsActive()==false)
            {
                animatorComponent->SetActive(true);
                ikComponent->SetActive(false);
                animatorComponent->PlayAnimation();

                // Body Rotation
                glm::vec3 lookAtPos = glm::normalize(targetPosXZ - ikPosXZ);
                glm::vec3 globalUpVector = glm::vec3(0.0, 1.0, 0.0);
                glm::vec3 W = lookAtPos;
                glm::vec3 U = glm::cross(globalUpVector, W);
                glm::vec3 V = glm::cross(W, U);
                // Create a rotation matrix using the Frenet frame vectors (W, U, V)
                glm::mat4 rotationMatrix = glm::mat4(
                    U.x, U.y, U.z, 0.0f,
                    V.x, V.y, V.z, 0.0f,
                    W.x, W.y, W.z, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f
                );
                // Extract to angles (in degrees) to feed into Transform class
                glm::vec3 rotation = glm::degrees(glm::eulerAngles(glm::quat_cast(rotationMatrix)));
                IKGobj->transform.SetRotation(rotation);
            }
            glm::vec3 direction = glm::normalize(targetPosXZ - ikPosXZ);
            ikPos += direction * movementSpeed * dt;
            IKGobj->transform.SetPosition(ikPos);

        }
        else
        {
            if (ikComponent->IsActive() == false)
            {
                animatorComponent->StopAnimation();
                animatorComponent->OverwriteAnimationLocalVQStoModelLocalVQS();
                animatorComponent->SetActive(false);
                ikComponent->SetActive(true);
            }
            auto gobjVQS = IHCEngine::Math::VQS(ikPos, ikRot, ikScale.x);
            ikComponent->SetGameObjectVQS(gobjVQS);
            ikComponent->SetTarget(targetPos);
            trigger = false;
        }
	}

	void InverseKinematicsViewer::FixedUpdate() {}

	void InverseKinematicsViewer::OnEnable() {}

	void InverseKinematicsViewer::OnDisable() {}

	void InverseKinematicsViewer::TargetObjectInputs()
	{
        float dt = IHCEngine::Core::Time::GetDeltaTime();
        auto up = glm::vec3(0,1,0);
        auto right = glm::vec3(1, 0, 0);
        auto forward = glm::vec3(0, 0, 1);

        // Translate
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_UP))//GLFW_KEY_W))
        {
            auto p = -1.0f * forward * movementSpeed * dt;
            movingGobj->transform.Translate(p);
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_DOWN))//GLFW_KEY_S))
        {
            auto p = forward * movementSpeed * dt;
            movingGobj->transform.Translate(p);
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_LEFT))//GLFW_KEY_A))
        {
            auto p = -1.0f * right * movementSpeed * dt;
            movingGobj->transform.Translate(p);
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_RIGHT))//GLFW_KEY_D))
        {
            auto p = right * movementSpeed * dt;
            movingGobj->transform.Translate(p);
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_E))
        {
            auto p = up * movementSpeed * dt;
            movingGobj->transform.Translate(p);
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_Q))
        {
            auto p = -1.0f * up * movementSpeed * dt;
            movingGobj->transform.Translate(p);
        }
	}
}
