
#pragma once
#include "../../../../Engine/src/Core/Scene/GameObject.h"
#include "../../../../Engine/src/Graphics/GraphicsManager.h"
#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"

namespace IHCEngine::Graphics
{
	class Animation;
}

namespace SampleApplication
{
    class AnimationTester : public IHCEngine::Component::CustomBehavior
    {
    public:
        AnimationTester();
        void Awake() override;
        void Start() override;
        void Update() override;
        void FixedUpdate() override;
        void OnEnable() override;
        void OnDisable() override;

        void AddAnimationGobjs(IHCEngine::Core::GameObject* g)
        {
            animationGobjs.push_back(g);
        }
        void AddAnimationSlots1(IHCEngine::Graphics::Animation* a)
        {
            animationSlots1.push_back(a);
        }
        void AddAnimationSlots2(IHCEngine::Graphics::Animation* a)
        {
            animationSlots2.push_back(a);
        }

    private:
        GLFWwindow* window = nullptr;

        // Make each animation Models has corresponding 2 animations
        std::vector<IHCEngine::Core::GameObject*> animationGobjs;
        std::vector<IHCEngine::Graphics::Animation*> animationSlots1;
        std::vector<IHCEngine::Graphics::Animation*> animationSlots2;
        int modelIndex = 0;
        int animationIndex = 0;


        bool wasRightKeyPressed = false;
        bool wasLeftKeyPressed = false;
        bool wasSpaceKeyPressed = false;

        bool wasBKeyPressed = false;
        bool wasMKeyPressed = false;

        bool wasAsteriskKeyPressed = false;
        bool wasSlashKeyPressed = false;
    };
}
