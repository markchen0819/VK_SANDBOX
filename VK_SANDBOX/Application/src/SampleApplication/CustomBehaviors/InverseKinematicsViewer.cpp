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
#include "../../../../Engine/src/Graphics/Camera.h"

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

        // AnimationViewerInput
        auto cameraGobj = sceneManager->GetActiveScene()->GetGameObjectByName("camera");
        lineRenderer = cameraGobj->GetComponent<IHCEngine::Component::LineRendererComponent>();

        camera = &(this->gameObject->GetScene()->GetCamera());
        camera->transform.Rotate(glm::vec3(-20, 0, 0));
        camera->transform.SetPosition(glm::vec3(0.0f, 15.0f, 20.0f));
        glm::vec3 cameraPosWithoutY = camera->transform.GetPosition();
        angleRespectToCenterPoint = 90;
        cameraPosWithoutY.y = 0;
        distanceToCenterPoint = length(centerPoint - cameraPosWithoutY);
        camera->LookAt(glm::vec3(0, 5, 0));
       

        // Target Gobj
		movingGobj = sceneManager->GetActiveScene()->GetGameObjectByName("targetGobj");
        movingGobj->transform.SetScale(glm::vec3(0.5, 0.5, 0.5));
        movingGobj->transform.SetPosition(glm::vec3(-1, 10, 3));

        // IK Gobj
        IKGobj = sceneManager->GetActiveScene()->GetGameObjectByName("IKGobj");

        animatorComponent = IKGobj->GetComponent<IHCEngine::Component::AnimatorComponent>();
        animatorComponent->SetActive(false);

        ikComponent = IKGobj->GetComponent<IHCEngine::Component::IKComponent>();
        auto pos = IKGobj->transform.GetPosition();
        auto rot = IHCEngine::Math::Quaternion::CreateFromGLMQuat(IKGobj->transform.GetRotationInQuaternion());
        auto scale = IKGobj->transform.GetScale();
        auto gobjVQS = IHCEngine::Math::VQS(pos, rot, scale.x);
        ikComponent->SetGameObjectVQS(gobjVQS);
        //ikComponent->SetRootAndEE("mixamorig:RightShoulder", "mixamorig:RightHand");
        ikComponent->SetRootAndEE("mixamorig:RightShoulder", "mixamorig:RightHandIndex4_end");
        ikComponent->SetActive(false);
	}

	void InverseKinematicsViewer::Start()
	{

	}

	void InverseKinematicsViewer::Update()
	{
        HandleAnimationViewerInput();

        if(mode=="IK_Animation")
        {
            IK_Animation();
        }
        else if(mode=="IK_Realtime")
        {
            IK_RealTime();
        }
	}

	void InverseKinematicsViewer::FixedUpdate() {}

	void InverseKinematicsViewer::OnEnable() {}

	void InverseKinematicsViewer::OnDisable() {}

	void InverseKinematicsViewer::SetMode(std::string m)
    {
        mode = m;
    }

	void InverseKinematicsViewer::TargetObjectInputs()
    {
        float dt = IHCEngine::Core::Time::GetDeltaTime();
        auto up = glm::vec3(0, 1, 0);
        auto right = glm::vec3(1, 0, 0);
        auto forward = glm::vec3(0, 0, 1);

        // Translate
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_W)) //GLFW_KEY_UP))
        {
            auto p = -1.0f * forward * movementSpeed * dt;
            movingGobj->transform.Translate(p);
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_S)) //GLFW_KEY_DOWN))//
        {
            auto p = forward * movementSpeed * dt;
            movingGobj->transform.Translate(p);
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_A)) //GLFW_KEY_LEFT))//
        {
            auto p = -1.0f * right * movementSpeed * dt;
            movingGobj->transform.Translate(p);
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_D)) //GLFW_KEY_RIGHT))//
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

    void InverseKinematicsViewer::IK_Animation()
    {
        float dt = IHCEngine::Core::Time::GetDeltaTime();

        if (trigger == false)
        {
            TargetObjectInputs();
        }
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
        float distanceToDoIK = ikComponent->GetTotalDistance() - 0.05;

        // Walk Along Path
        if (glm::distance(ikPosXZ, targetPosXZ) > distanceToDoIK)
        {
            if (animatorComponent->IsActive() == false)
            {
                animatorComponent->SetActive(true);
                ikComponent->SetActive(false);
                animatorComponent->PlayAnimation();

                // Orientation
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

                //Path
                std::vector<glm::vec3> points;
                glm::vec3 destOffset = distanceToDoIK * -lookAtPos;
                points.push_back(ikPosXZ);
                points.push_back(targetPosXZ + destOffset);
                lineRenderer->SetPoints(points);
                for (auto pt : debugControlPoints)
                {
                    pt->Destroy();
                }
                debugControlPoints.clear();
                auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
                auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
                for (int i = 0; i < points.size(); ++i)
                {
                    std::string id = "debugControlPoint_" + std::to_string(i);
                    IHCEngine::Core::GameObject& point = sceneManager->GetActiveScene()->AddGameObject(id);
                    point.AddComponent<IHCEngine::Component::PipelineComponent>();
                    auto meshcomponent = point.AddComponent<IHCEngine::Component::MeshComponent>();
                    meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("controlPointModel"));
                    auto texturecomponent = point.AddComponent<IHCEngine::Component::TextureComponent>();
                    texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));
                    point.transform.SetPosition(points[i]);
                    point.transform.SetScale(glm::vec3(0.2, 0.2, 0.2));
                    debugControlPoints.push_back(&point);
                }
            }
            glm::vec3 direction = glm::normalize(targetPosXZ - ikPosXZ);
            ikPos += direction * movementSpeed * dt;
            IKGobj->transform.SetPosition(ikPos);

        }
        else // IK Animation
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

            if (currentTime == 0)
            {
                auto ee = ikComponent->GetEndEffector();
                eePositionAtKeyframeStart = ee->globalVQS.GetTranslate();
                currentTime += dt;
            }
            if (currentTime < totalTime)
            {
                glm::vec3 interpolatedTarget = glm::mix(eePositionAtKeyframeStart, targetPos, currentTime / totalTime);
                ikComponent->SetTarget(interpolatedTarget);
                currentTime += dt;
            }
            else if (currentTime >= totalTime)
            {
                currentTime = 0;
                ikComponent->SetTarget(targetPos);
                trigger = false;
            }
        }
    }

    void InverseKinematicsViewer::IK_RealTime()
    {
        TargetObjectInputs();

        auto targetPos = movingGobj->transform.GetPosition();
        auto pos = IKGobj->transform.GetPosition();
        auto rot = IHCEngine::Math::Quaternion::CreateFromGLMQuat(IKGobj->transform.GetRotationInQuaternion());
        auto scale = IKGobj->transform.GetScale();
        auto gobjVQS = IHCEngine::Math::VQS(pos, rot, scale.x);
        ikComponent->SetGameObjectVQS(gobjVQS);

        ikComponent->SetTarget(targetPos);
    }

    void InverseKinematicsViewer::HandleAnimationViewerInput()
    {
        float dt = IHCEngine::Core::Time::GetDeltaTime();
        auto cameraUp = camera->transform.GetUp(); //camera->GetUp();
        auto cameraRight = camera->transform.GetRight(); //camera->GetRight();
        auto cameraForward = camera->transform.GetForward();// camera->GetForward();

        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_0)) // rotate right around target
        {
            angleRespectToCenterPoint += angleSpeed * dt;
            glm::vec3 cameraPos = camera->transform.GetPosition();
            glm::vec3 newPos;
            newPos.x = centerPoint.x + distanceToCenterPoint * cos(glm::radians(angleRespectToCenterPoint));
            newPos.y = cameraPos.y;  // Y remains the same as we're not moving vertically
            newPos.z = centerPoint.z + distanceToCenterPoint * sin(glm::radians(angleRespectToCenterPoint));
            camera->transform.SetWorldPosition(newPos);
            camera->LookAt(glm::vec3(0, 5, 0));
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_DECIMAL))// rotate left around target
        {
            angleRespectToCenterPoint -= angleSpeed * dt;
            glm::vec3 cameraPos = camera->transform.GetPosition();
            glm::vec3 newPos;
            newPos.x = centerPoint.x + distanceToCenterPoint * cos(glm::radians(angleRespectToCenterPoint));
            newPos.y = cameraPos.y;  // Y remains the same as we're not moving vertically
            newPos.z = centerPoint.z + distanceToCenterPoint * sin(glm::radians(angleRespectToCenterPoint));
            camera->transform.SetWorldPosition(newPos);
            camera->LookAt(glm::vec3(0, 5, 0));
        }
        if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_KP_5)) // reset
        {
            angleRespectToCenterPoint = 90;
            glm::vec3 cameraPos = camera->transform.GetPosition();
            glm::vec3 newPos;
            newPos.x = centerPoint.x + distanceToCenterPoint * cos(glm::radians(angleRespectToCenterPoint));
            newPos.y = cameraPos.y;  // Y remains the same as we're not moving vertically
            newPos.z = centerPoint.z + distanceToCenterPoint * sin(glm::radians(angleRespectToCenterPoint));
            camera->transform.SetWorldPosition(newPos);
            camera->LookAt(glm::vec3(0, 5, 0));
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_4)) // left
        {
            auto p = -cameraRight * movementSpeed * dt;
            camera->transform.Translate(p);
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_6)) // right
        {
            auto p = cameraRight * movementSpeed * dt;
            camera->transform.Translate(p);
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_8)) // up
        {
            auto p = cameraUp * movementSpeed * dt;
            camera->transform.Translate(p);
            auto cameraX = camera->transform.GetPosition().x;
            camera->LookAt(glm::vec3(cameraX, 5, 0));
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_2)) // down
        {
            auto p = -cameraUp * movementSpeed * dt;
            camera->transform.Translate(p);
            auto cameraX = camera->transform.GetPosition().x;
            camera->LookAt(glm::vec3(cameraX, 5, 0));
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_ADD)) // zoom in
        {
            float newFov = camera->GetFOV() - 1 * zoomSpeed;
            camera->SetFOV(newFov);
        }
        if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_SUBTRACT)) // zoom in
        {
            float newFov = camera->GetFOV() + 1 * zoomSpeed;
            camera->SetFOV(newFov);
        }
        if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_B))
        {
            // bone drawing on/off.
            IHCEngine::Graphics::RenderSystem::debugBonesEnabled = !IHCEngine::Graphics::RenderSystem::debugBonesEnabled;
        }
        if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_M))
        {
            // mesh drawing on/off.
            IHCEngine::Graphics::RenderSystem::animationMeshEnabled = !IHCEngine::Graphics::RenderSystem::animationMeshEnabled;
        }
    }

#pragma region Imgui
#pragma endregion 
}
