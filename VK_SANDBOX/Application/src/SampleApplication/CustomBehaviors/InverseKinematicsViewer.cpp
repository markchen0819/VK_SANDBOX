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
        IKGobj = sceneManager->GetActiveScene()->GetGameObjectByName("IKGobj");
        ikComponent = IKGobj->GetComponent<IHCEngine::Component::IKComponent>();

        //auto testEE = model->GetNodeByName("mixamorig:RightHandIndex4_end");
		//auto testRoot = model->GetNodeByName("mixamorig:RightForeArm");
        //ikComponent->SetRootAndEE("mixamorig:RightShoulder", "mixamorig:RightHandIndex4_end");
        ikComponent->SetRootAndEE("mixamorig:RightShoulder", "mixamorig:RightHand");

        //auto testRoot = model->GetNodeByName("mixamorig:RightShoulder");
		//auto testPath = model->GetPathFromRootToEE(testEE, testRoot);
		//SetJoints(testPath);
		//float scale = 0.05;// welp localVQS fucked by global scale
	}

	void InverseKinematicsViewer::Start() {}

	void InverseKinematicsViewer::Update()
	{
        float scale = IKGobj->transform.GetScale().x;
        glm::vec3 scaledPosition = movingGobj->transform.GetPosition() / glm::vec3(scale, scale, scale);
        ikComponent->SetTarget(scaledPosition);

		TargetObjectInputs();
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
