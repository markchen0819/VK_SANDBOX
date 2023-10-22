#pragma once
#include <GLFW/glfw3.h>
#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"
namespace IHCEngine::Graphics
{
	class Animation;
	class Camera;
}
namespace SampleApplication
{
    class AnimationViewer : public IHCEngine::Component::CustomBehavior
    {
    public:
        AnimationViewer();
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
        IHCEngine::Graphics::Camera* camera = nullptr;

        // Make each animation Models has corresponding 2 animations
        void HandleModelInput();
    	std::vector<IHCEngine::Core::GameObject*> animationGobjs;
        std::vector<IHCEngine::Graphics::Animation*> animationSlots1;
        std::vector<IHCEngine::Graphics::Animation*> animationSlots2;
        int modelIndex = 0;
        int animationIndex = 0;

        // Animation Viewer Input
        void HandleAnimationViewerInput();
        glm::vec3 centerPoint = glm::vec3(0.0);
        float angleRespectToCenterPoint;
        float distanceToCenterPoint;
        float angleSpeed = 50.0;
        float movementSpeed = 10;
        float zoomSpeed = 0.03f;
    };
}
