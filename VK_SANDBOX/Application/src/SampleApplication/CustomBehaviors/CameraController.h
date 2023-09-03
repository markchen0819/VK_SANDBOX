#pragma once
#include <GLFW/glfw3.h>
#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"
namespace IHCEngine::Graphics
{
    class Camera;
}
namespace SampleApplication
{
	class CameraController : public IHCEngine::Component::CustomBehavior
	{
    public:
        CameraController();
        void Awake() override;
        void Start() override;
        void Update() override;
        void FixedUpdate() override;
        void OnEnable() override;
        void OnDisable() override; 

    private:

        GLFWwindow* window = nullptr;
        IHCEngine::Graphics::Camera* camera=nullptr;
        float movementSpeed = 0.02;
        float rotationSpeed = 0.1;

        void HandleInput();
	};
}
