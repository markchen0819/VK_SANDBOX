#include <glm/gtx/string_cast.hpp> 
#include "AnimationTester.h"

#include "imgui.h"
#include "../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../../../../Engine/src/Core/Scene/GameObject.h"
#include "../../Engine/src/Core/Locator/AppWindowLocator.h"
#include "../../Engine/src/Core/Time/Time.h"
#include "../../Engine/src/Core/Scene/Components/AnimatorComponent.h"
#include "../../Engine/src/Graphics/RenderSystems/RenderSystem.h"

namespace SampleApplication
{
	AnimationTester::AnimationTester()
	{
	}

	void AnimationTester::Awake()
	{
		window = IHCEngine::Core::AppWindowLocator::GetAppWindow()->GetWindowHandle();
		for(int i=0; i< animationGobjs.size(); ++i)
		{
			animationGobjs[i]->SetActive(false);
		}
		animationGobjs[0]->SetActive(true);
		animationGobjs[0]->GetComponent<IHCEngine::Component::AnimatorComponent>()
			->SetAnimation(animationSlots1[0]);
	}

	void AnimationTester::Start()
	{
	}

	void AnimationTester::Update()
	{
		bool isRightKeyPressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
		if (isRightKeyPressed && !wasRightKeyPressed) 
		{
			animationGobjs[modelIndex]->SetActive(false);
			modelIndex = (modelIndex + 1) % animationGobjs.size();
			animationGobjs[modelIndex]->SetActive(true);

			if (animationIndex == 0)
			{
				animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
					->SetAnimation(animationSlots1[modelIndex]);
			}
			else
			{
				animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
					->SetAnimation(animationSlots2[modelIndex]);
			}
		}
		wasRightKeyPressed = isRightKeyPressed;

		bool isLeftKeyPressed = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
		if (isLeftKeyPressed && !wasLeftKeyPressed)
		{
			animationGobjs[modelIndex]->SetActive(false);
			modelIndex = (modelIndex - 1) % animationGobjs.size();
			animationGobjs[modelIndex]->SetActive(true);

			if (animationIndex == 0)
			{
				animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
					->SetAnimation(animationSlots1[modelIndex]);
			}
			else
			{
				animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
					->SetAnimation(animationSlots2[modelIndex]);
			}
		}
		wasLeftKeyPressed = isLeftKeyPressed;

		bool isSpaceKeyPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
		if (isSpaceKeyPressed && !wasSpaceKeyPressed) 
		{
			animationIndex = (animationIndex + 1) % 2;
			if (animationIndex == 0)
			{
				animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
					->SetAnimation(animationSlots1[modelIndex]);
			}
			else
			{
				animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
					->SetAnimation(animationSlots2[modelIndex]);
			}
		}
		wasSpaceKeyPressed = isSpaceKeyPressed;


		bool isBKeyPressed = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;
		if (isBKeyPressed && !wasBKeyPressed)
		{
			// bone drawing on/off.
			IHCEngine::Graphics::RenderSystem::debugBonesEnabled = !IHCEngine::Graphics::RenderSystem::debugBonesEnabled;
		}
		wasBKeyPressed = isBKeyPressed;

		bool isMKeyPressed = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;
		if (isMKeyPressed && !wasMKeyPressed)
		{
			// mesh drawing on/off.
			IHCEngine::Graphics::RenderSystem::animationMeshEnabled = !IHCEngine::Graphics::RenderSystem::animationMeshEnabled;
		}
		wasMKeyPressed = isMKeyPressed;
	}

	void AnimationTester::FixedUpdate()
	{
		
	}

	void AnimationTester::OnEnable()
	{
		
	}

	void AnimationTester::OnDisable()
	{
		
	}
}
